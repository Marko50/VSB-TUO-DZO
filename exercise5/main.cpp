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

map<string, cv::Mat> calculate_spectrum(cv::Mat src){

    cv::Mat result_64fc1_phase_spectrum = src.clone();
    cv::Mat result_64fc1_power_spectrum = src.clone();

    double M = (double) src.rows;
    double N = (double) src.cols;

    for (int y = 0; y < M - 1; y++){
        for (int x = 0; x < N - 1; x++){
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
            result_64fc1_phase_spectrum.at<double>(y,x) = atan2(pix_imaginary, pix_real);
            result_64fc1_power_spectrum.at<double>(y,x) = log(pix_real*pix_real + pix_imaginary*pix_imaginary);
        }   
    }
    normalize_power_values(&result_64fc1_power_spectrum);
    map<string, cv::Mat> mp;
    mp.insert({"phase", result_64fc1_phase_spectrum});
    mp.insert({"power", result_64fc1_power_spectrum});
    
    return mp;
}

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "lena64.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }
    
    cv::Mat src_64fc1_img;
    src_8uc1_img.convertTo(src_64fc1_img, CV_64FC1, 1/255.0);

    map<string, cv::Mat> map = calculate_spectrum(src_64fc1_img);
    cv::Mat result_64fc1_phase_spectrum = map["phase"];
    cv::Mat result_64fc1_power_spectrum = map["power"];


    // diplay images
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Power", cv::WINDOW_NORMAL);
    cv::namedWindow("Phase", cv::WINDOW_NORMAL);
    cv::imshow("Original", src_64fc1_img);
    cv::imshow("Phase", result_64fc1_phase_spectrum);
    cv::imshow("Power", result_64fc1_power_spectrum);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
