#include <math.h>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

const int KERNEL_DIMENSION = 3;


void apply_convolution(cv::Mat src, cv::Mat & dest,  int kernel[][KERNEL_DIMENSION], int normalization ){
    for (int y = 0; y < dest.rows - KERNEL_DIMENSION; y++){
            for (int x = 0; x < dest.cols - KERNEL_DIMENSION; x++){
                int value = 0;
                for (int i = 0; i < KERNEL_DIMENSION; i++){
                    for (int j = 0; j < KERNEL_DIMENSION; j++){
                        int src_value = (int) src.at<uchar>(y + i, x + j);
                        value += kernel[i][j]*src_value;
                    }
                }
                value /= (normalization);
                if (value < 0){
                    value = 0;
                }
                else if (value > 255){
                    value = 255;
                }
                dest.at<uchar>(y,x) = (uchar) value;
            }
        }
}

void calculate_gaussian_blur(cv::Mat src, cv::Mat & dest){
    int kernel[KERNEL_DIMENSION][KERNEL_DIMENSION] = { {1, 2, 1}, {2, 4 , 2}, {1, 2, 1} };;
    apply_convolution(src, dest, kernel,16);
}

void calculate_image_derivative_x(cv::Mat src, cv::Mat & dest){
    int kernel[KERNEL_DIMENSION][KERNEL_DIMENSION] = { {-1, 0, 1}, {-2, 0 , 2}, {-1, 0, 1} };
    apply_convolution(src, dest, kernel,1);
}

void calculate_image_derivative_y(cv::Mat src, cv::Mat & dest){
    int kernel[KERNEL_DIMENSION][KERNEL_DIMENSION] = { {1, 2, 1}, {0, 0 ,0}, {-1, -2, -1} };
    apply_convolution(src, dest, kernel,1);
}

void calculate_image_derivative(cv::Mat der_x, cv::Mat der_y, cv::Mat & dest){
    for (int y = 0; y < dest.rows; y++){
        for (int x = 0; x < dest.cols; x++){
            uchar der_x_value = der_x.at<uchar>(y,x);    
            uchar der_y_value = der_y.at<uchar>(y,x);
            uchar value = (uchar) sqrt(pow(der_x_value,2) + pow(der_y_value,2)); 
            if (value < 0){
                value = 0;
            }
            else if (value > 255){
                value = 255;
            }
            dest.at<uchar>(y,x) = value;
        }
    }
}

void calculate_image_edges(cv::Mat src, cv::Mat & dest){
    dest = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat src_grayscale = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat src_gauss_blur = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat x_derived = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat y_derived = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::cvtColor(src, src_grayscale, cv::COLOR_BGR2GRAY);

    calculate_gaussian_blur(src_grayscale, src_gauss_blur);
    calculate_image_derivative_x(src_gauss_blur, x_derived);
    calculate_image_derivative_y(src_gauss_blur, y_derived);
    calculate_image_derivative(x_derived, y_derived, dest);
}

int main(){
    cv::Mat src_8uc3_img_valve = cv::imread("valve.png", cv::IMREAD_COLOR);
   
    if (src_8uc3_img_valve.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }
    
    cv::Mat src_8uc1_img_edges;
    calculate_image_edges(src_8uc3_img_valve, src_8uc1_img_edges);
    
    cv::imshow("Original", src_8uc3_img_valve);
    cv::imshow("Result", src_8uc1_img_edges);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
