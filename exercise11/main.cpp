#include <math.h>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

void calculate_image_derivative_x(cv::Mat src, cv::Mat & dest){
    for (int y = 1; y < src.rows - 1; y++){
        for (int x = 1; x < src.cols - 1; x++){
            cv::Vec3b original_front = src.at<cv::Vec3b>(y,x + 1);
            cv::Vec3b original_back = src.at<cv::Vec3b>(y,x - 1);
            dest.at<cv::Vec3b>(y,x) = (original_front - original_back);
        }
    }   
}

void calculate_image_derivative_y(cv::Mat src, cv::Mat & dest){
    for (int x = 1; x < src.cols - 1; x++){
        for (int y = 1; y < src.rows - 1; y++){
            cv::Vec3b original_front = src.at<cv::Vec3b>(y + 1,x );
            cv::Vec3b original_back = src.at<cv::Vec3b>(y - 1,x);
            dest.at<cv::Vec3b>(y,x) = (original_front - original_back);
        }
    } 
}

void clean_borders(cv::Mat & dest){
    for (int x = 0; x < dest.cols; x++){
        dest.at<cv::Vec3b>(0,x) = cv::Vec3b(0,0,0);
        dest.at<cv::Vec3b>(dest.rows - 1,x) = cv::Vec3b(0,0,0);
    }
    for (int y = 0; y < dest.cols; y++){
        dest.at<cv::Vec3b>(y,0) = cv::Vec3b(0,0,0);
        dest.at<cv::Vec3b>(y,dest.cols - 1) = cv::Vec3b(0,0,0);
    }
}

void calculate_image_derivative(cv::Mat der_x, cv::Mat der_y, cv::Mat & dest){
    for (int y = 0; y < dest.rows; y++){
        for (int x = 0; x < dest.cols; x++){
            uchar der_x_1 = der_x.at<cv::Vec3b>(y,x)[0];
            uchar der_x_2 = der_x.at<cv::Vec3b>(y,x)[1];
            uchar der_x_3 = der_x.at<cv::Vec3b>(y,x)[2];

            uchar der_y_1 = der_y.at<cv::Vec3b>(y,x)[0];
            uchar der_y_2 = der_y.at<cv::Vec3b>(y,x)[1];
            uchar der_y_3 = der_y.at<cv::Vec3b>(y,x)[2];

            cv::Vec3b sum = cv::Vec3b( (pow(der_x_1,2) + pow(der_y_1,2))/3 , (pow(der_x_2, 2) + pow(der_y_2,2))/3, (pow(der_x_3,2) + pow(der_y_3,2))/3);

            dest.at<uchar>(y,x) = sqrt(sum[0] + sum[1] + sum[2]);
        }
    }
}

void calculate_image_edges(cv::Mat src, cv::Mat & dest){
    cv::Mat x_derived = cv::Mat(src.rows, src.cols, CV_8UC3);
    cv::Mat y_derived = cv::Mat(src.rows, src.cols, CV_8UC3);
    dest = cv::Mat(src.rows, src.cols, CV_8UC1);
    calculate_image_derivative_x(src, x_derived);
    calculate_image_derivative_y(src, y_derived);
    calculate_image_derivative(x_derived, y_derived, dest);
}

int main(){
    cv::Mat src_8uc3_img_valve = cv::imread("valve.png", cv::IMREAD_COLOR);
   
    if (src_8uc3_img_valve.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }
    
    cv::Mat src_8uc3_img_edges;
    calculate_image_edges(src_8uc3_img_valve, src_8uc3_img_edges);
    
    cv::imshow("Original", src_8uc3_img_valve);
    cv::imshow("Result", src_8uc3_img_edges);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
