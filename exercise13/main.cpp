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

void calculate_positive_laplace(cv::Mat src, cv::Mat & dest){
    int kernel[KERNEL_DIMENSION][KERNEL_DIMENSION] = { {0, 1, 0}, {1, -4 , 1}, {0, 1, 0} };
    apply_convolution(src, dest, kernel,1);
}

void calculate_negative_laplace(cv::Mat src, cv::Mat & dest){
    int kernel[KERNEL_DIMENSION][KERNEL_DIMENSION] = { {0, -1, 0}, {-1, 4 ,-1}, {0, -1, 0} };
    apply_convolution(src, dest, kernel,1);
}


void calculate_laplace(cv::Mat src, cv::Mat & dest){
    cv::Mat src_grayscale = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat positive_laplace = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::Mat negative_laplace = cv::Mat(src.rows, src.cols, CV_8UC1);
    cv::cvtColor(src, src_grayscale, cv::COLOR_BGR2GRAY);

    calculate_positive_laplace(src_grayscale, positive_laplace);
    calculate_negative_laplace(src_grayscale, negative_laplace);

    dest = negative_laplace;
}

int main(){
    cv::Mat src_8uc3_img_valve = cv::imread("valve.png", cv::IMREAD_COLOR);
   
    if (src_8uc3_img_valve.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }
    
    cv::Mat src_8uc1_img_edges;
    calculate_laplace(src_8uc3_img_valve, src_8uc1_img_edges);
    
    cv::imshow("Original", src_8uc3_img_valve);
    cv::imshow("Result", src_8uc1_img_edges);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
