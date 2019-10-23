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

void shift_pixels_power_spectrum(cv::Mat* src){
    for (int y = 0; y < src->rows/2; y++){
        for (int x = 0; x < src->cols/2; x++){
            double copy = src->at<double>(y,x);
            src->at<double>(y,x) = src->at<double>(y + src->rows/2,x + src->cols/2);
            src->at<double>(y + src->rows/2,x + src->cols/2) = copy;
        }
    }
    for (int y = 0 ; y < src->rows/2; y++){
        for (int x = src->cols - 1; x > src->cols/2; x--){
            double copy = src->at<double>(y,x);
            src->at<double>(y,x) = src->at<double>(y + src->rows/2, x - src->cols/2);
            src->at<double>(y + src->rows/2,x - src->cols/2) = copy;
        }
    }
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

void calculate_phase_spectrum(cv::Mat fourrier, cv::Mat* phase_spectrum){
    for (int y = 0; y < fourrier.rows; y++){
        for (int x = 0; x < fourrier.cols; x++){
            double pix_imaginary = fourrier.at<cv::Vec2d>(y,x)[1];
            double pix_real = fourrier.at<cv::Vec2d>(y,x)[0];
            phase_spectrum->at<double>(y,x) = atan2(pix_imaginary, pix_real);  
        }
    }
}

void calculate_power_spectrum(cv::Mat fourrier, cv::Mat* power_spectrum){
    for (int y = 0; y < fourrier.rows; y++){
        for (int x = 0; x < fourrier.cols; x++){
            double pix_imaginary = fourrier.at<cv::Vec2d>(y,x)[1];
            double pix_real = fourrier.at<cv::Vec2d>(y,x)[0];
            power_spectrum->at<double>(y,x) = log(pix_real*pix_real + pix_imaginary*pix_imaginary);  
        }
    }
    shift_pixels_power_spectrum(power_spectrum);
    normalize_power_values(power_spectrum);
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

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "lena64.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }
    
    cv::Mat src_64fc1_img;
    src_8uc1_img.convertTo(src_64fc1_img, CV_64FC1, 1/255.0);

    cv::Mat phase_spectrum = src_64fc1_img.clone();
    cv::Mat power_spectrum = src_64fc1_img.clone();
    cv::Mat original = src_64fc1_img.clone();

    cv::Mat fourrier = calculate_fourrier(src_64fc1_img);
    calculate_phase_spectrum(fourrier, &phase_spectrum);
    calculate_power_spectrum(fourrier, &power_spectrum);
    calculate_original(fourrier, &original);
   
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Power", cv::WINDOW_NORMAL);
    cv::namedWindow("Phase", cv::WINDOW_NORMAL);
    cv::namedWindow("Reverse", cv::WINDOW_NORMAL);
    cv::imshow("Original", src_64fc1_img);
    cv::imshow("Phase", phase_spectrum);
    cv::imshow("Power", power_spectrum);
    cv::imshow("Reverse", original);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
