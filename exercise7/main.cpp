#include <math.h>

#include <opencv2/opencv.hpp>

using namespace std;

void normalize_power_values(cv::Mat* src){
    double min,max;
    cv::minMaxLoc(*src, &min, &max);
    for (int y = 0; y < src->rows; y++){
        for (int x = 0; x < src->cols ; x++){
            src->at<double>(y,x) = (src->at<double>(y,x) - min)/(max - min);
        }  
    }   
}


template <typename T> void shift_quadrants(cv::Mat* src){
    for (int y = 0; y < src->rows/2; y++){
        for (int x = 0; x < src->cols/2; x++){
            T copy = src->at<T>(y,x);
            src->at<T>(y,x) = src->at<T>(y + src->rows/2,x + src->cols/2);
            src->at<T>(y + src->rows/2,x + src->cols/2) = copy;
        }
    }
    for (int y = 0 ; y < src->rows/2; y++){
        for (int x = src->cols - 1; x >= src->cols/2; x--){
            T copy = src->at<T>(y,x);
            src->at<T>(y,x) = src->at<T>(y + src->rows/2, x - src->cols/2);
            src->at<T>(y + src->rows/2,x - src->cols/2) = copy;
        }
    }
}

void calculate_power_spectrum(cv::Mat fourrier, cv::Mat* power_spectrum){
    for (int y = 0; y < fourrier.rows; y++){
        for (int x = 0; x < fourrier.cols; x++){
            double pix_imaginary = fourrier.at<cv::Vec2d>(y,x)[1];
            double pix_real = fourrier.at<cv::Vec2d>(y,x)[0];
            power_spectrum->at<double>(y,x) = log((pix_real*pix_real + pix_imaginary*pix_imaginary) + 1);  
        }
    }
    normalize_power_values(power_spectrum);
    shift_quadrants<double>(power_spectrum);
}

cv::Mat calculate_fourrier(cv::Mat src){
    cv::Mat result_64fc2_fourrier = cv::Mat::zeros(src.rows, src.cols, CV_64FC2);

    double M = (double) src.rows;
    double N = (double) src.cols;

    for (int y = 0; y < M; y++){
        for (int x = 0; x < N; x++){
            double pix_real = 0.0;
            double pix_imaginary = 0.0;
            for (int m = 0; m < M; m++){
                for (int n = 0; n < N; n++){
                    double value = src.at<double>(m,n);
                    double basis_real = cos(2*M_PI*((double)m*(double)y/M + (double)n*(double)x/N))/sqrt(M*N);
                    double basis_imaginary = -sin(2*M_PI*((double)m*(double)y/M + (double)n*(double)x/N))/sqrt(M*N);
                    pix_real += value*basis_real;
                    pix_imaginary += value*basis_imaginary;
                }
            }
            result_64fc2_fourrier.at<cv::Vec2d>(y,x)[0] = pix_real;
            result_64fc2_fourrier.at<cv::Vec2d>(y,x)[1] = pix_imaginary;
        }   
    }
    return result_64fc2_fourrier;
}

void calculate_original(cv::Mat fourier_transform, cv::Mat* original){
    double M = (double) fourier_transform.rows;
    double N = (double) fourier_transform.cols;

    for (int y = 0; y < M; y++){
        for (int x = 0; x < N; x++){
            double pix_real = 0.0;
            double pix_imaginary = 0.0;

            for (int m = 0; m < M; m++){
                for (int n = 0; n < N; n++){
                    double src_real = fourier_transform.at<cv::Vec2d>(m,n)[0];
                    double src_imaginary = fourier_transform.at<cv::Vec2d>(m,n)[1];
                    double basis_real = cos(2*M_PI*((double)m*(double)y/M + (double)n*(double)x/N))/sqrt(M*N);
                    double basis_imaginary = sin(2*M_PI*((double)m*(double)y/M + (double)n*(double)x/N))/sqrt(M*N);

                    double result_real = src_real*basis_real - src_imaginary*basis_imaginary;
                    double result_imaginary = src_imaginary*basis_real + src_real*basis_imaginary;

                    pix_real+=result_real;
                    pix_imaginary+=result_imaginary;
                }
            }
            original->at<double>(y,x) = sqrt(pix_real*pix_real + pix_imaginary*pix_imaginary);
        }   
    }
}

void calculate_circular_mask(cv::Mat * mask, int raius, float value){
    int centerx = mask->cols/2; 
    int centery = mask->rows/2;
    
    for (int y = 0; y < mask->rows; y++){
        for (int x = 0; x < mask->cols; x++){
            int y_dif = y - centery;
            int x_dif = x - centerx;
            float length = sqrt(x_dif*x_dif + y_dif*y_dif);
            if (length < raius){
                mask->at<float>(y,x) = (float) value;
            }
        }
    }
}

void calculate_linear_mask(cv::Mat * mask, float value){
    int centery = mask->rows/2;
    int centerx = mask->cols/2;
    for (int x = 0; x < mask->cols; x+=1){  
        mask->at<float>(centery, x) = (float) !value;   
    }
    for (int y = 0; y < mask->rows; y++){
        for (int x = 0; x < mask->cols; x++){
            int y_dif = y - centery;
            int x_dif = x - centerx;
            float length = sqrt(x_dif*x_dif + y_dif*y_dif);
            if (length < 5){
                mask->at<float>(y,x) = (float) value;
            }
        }
    }
}

void apply_mask(cv::Mat * src, cv::Mat mask){
    for (int y = 0; y < src->rows; y++){
        for (int x = 0; x < src->cols; x++){
            cv::Vec2d src_value = src->at<cv::Vec2d>(y,x);
            cv::Vec2d new_value = cv::Vec2d((double) (src_value[0] * mask.at<float>(y,x)), (double) (src_value[1] * mask.at<float>(y,x)));
            src->at<cv::Vec2d>(y,x) = new_value;
        }    
    }
}

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "lena64_noise.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    cv::Mat src_8uc1_img2 = cv::imread( "lena64_bars.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img2.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }
    
    cv::Mat src_64fc1_img_circular, src_64fc1_img_linear;
    src_8uc1_img.convertTo(src_64fc1_img_circular, CV_64FC1, 1/255.0);
    src_8uc1_img2.convertTo(src_64fc1_img_linear, CV_64FC1, 1/255.0);

    cv::Mat filtered_circular = src_64fc1_img_circular.clone();
    cv::Mat filtered_linear = src_64fc1_img_circular.clone();
    cv::Mat power_spectrum_circular = src_64fc1_img_circular.clone();
    cv::Mat power_spectrum_linear = src_64fc1_img_linear.clone();

    cv::Mat circular_mask = cv::Mat::zeros(src_64fc1_img_circular.rows, src_64fc1_img_circular.cols, CV_32FC1);
    cv::Mat line_mask = cv::Mat::ones(src_64fc1_img_circular.rows, src_64fc1_img_circular.cols, CV_32FC1);
   
    cv::Mat fourrier = calculate_fourrier(src_64fc1_img_circular);
    calculate_circular_mask(&circular_mask, 20, 1);
    calculate_power_spectrum(fourrier, &power_spectrum_circular);  
    shift_quadrants<cv::Vec2d>(&fourrier);
    apply_mask(&fourrier, circular_mask);
    shift_quadrants<cv::Vec2d>(&fourrier);
    calculate_original(fourrier, &filtered_circular);

    cv::Mat fourrier2 = calculate_fourrier(src_64fc1_img_linear);
    calculate_linear_mask(&line_mask, 1);
    calculate_power_spectrum(fourrier2, &power_spectrum_linear);
    shift_quadrants<cv::Vec2d>(&fourrier2);
    apply_mask(&fourrier2, line_mask);
    shift_quadrants<cv::Vec2d>(&fourrier2);
    calculate_original(fourrier2, &filtered_linear);

    cv::namedWindow("Power Circular", cv::WINDOW_NORMAL);
    cv::namedWindow("Original Circular", cv::WINDOW_NORMAL);
    cv::namedWindow("Circular Mask", cv::WINDOW_NORMAL);
    cv::namedWindow("Filtered Circular", cv::WINDOW_NORMAL);
    cv::namedWindow("Power Linear", cv::WINDOW_NORMAL);
    cv::namedWindow("Original Linear", cv::WINDOW_NORMAL);
    cv::namedWindow("Linear Mask", cv::WINDOW_NORMAL);
    cv::namedWindow("Filtered Linear", cv::WINDOW_NORMAL);
    
    cv::imshow("Power Circular", power_spectrum_circular);
    cv::imshow("Original Circular", src_64fc1_img_circular);
    cv::imshow("Circular Mask", circular_mask);
    cv::imshow("Filtered Circular", filtered_circular);
    cv::imshow("Power Linear", power_spectrum_linear);
    cv::imshow("Original Linear", src_64fc1_img_linear);
    cv::imshow("Linear Mask", line_mask);
    cv::imshow("Filtered Linear", filtered_linear);
    
    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
