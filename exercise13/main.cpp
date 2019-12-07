#include <math.h>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

void calculate_negative_laplace_operator(cv::Mat src, cv::Mat & dest ){
    int LAPLACIAN_DIMENSION = 3;
    int BOX_BLUR_NORMALIZATION = 9;
    int laplacian_mask[LAPLACIAN_DIMENSION][LAPLACIAN_DIMENSION] = { {0, -1, 0}, {-1, 4 , -1}, {0, -1, 0} };

    for (int y = 0; y < dest.rows - LAPLACIAN_DIMENSION; y++){
        for (int x = 0; x < dest.cols - LAPLACIAN_DIMENSION; x++){
            cv::Vec3b value = cv::Vec3b(0,0,0);
            for (int i = 0; i < LAPLACIAN_DIMENSION; i++){
                for (int j = 0; j < LAPLACIAN_DIMENSION; j++){
                    cv::Vec3b src_value = src.at<cv::Vec3b>(y + i, x + j);
                    value += laplacian_mask[i][j]*src_value;
                }
            }
            dest.at<cv::Vec3b>(y,x) = (cv::Vec3b) value;
        }
    }
}

void apply_laplace_operator(cv::Mat src, cv::Mat & dest){
    cv::Mat negative_laplace_operator = cv::Mat(src.rows, src.cols, CV_8UC3);
    calculate_negative_laplace_operator(src, negative_laplace_operator);
    dest = negative_laplace_operator;
}

int main(){
    cv::Mat src_8uc3_img_valve = cv::imread("valve.png", cv::IMREAD_COLOR);
   
    if (src_8uc3_img_valve.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }
    
    cv::Mat src_8uc3_img_edges;
    apply_laplace_operator(src_8uc3_img_valve, src_8uc3_img_edges);
    cv::imshow("Original", src_8uc3_img_valve);
    cv::imshow("Result", src_8uc3_img_edges);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
