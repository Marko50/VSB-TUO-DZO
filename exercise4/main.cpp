#include <stdio.h>

#include <opencv2/opencv.hpp>

double calculate_output_intensity(double intensity, double north, double south, double west, double east){
    
    return 0;
}


int main()
{
    cv::Mat src_8uc1_img = cv::imread( "valve.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    cv::Mat result_8uc1_img = src_8uc1_img.clone(); // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)

    for (int y = 1; y < result_8uc1_img.rows; y++){
        for (int x = 1; x < result_8uc1_img.cols; x++){
            double intensity = result_8uc1_img.at<double>(y,x);
            double north_intensity = result_8uc1_img.at<double>(y - 1, x);
            double south_intensity = result_8uc1_img.at<double>(y + 1, x);
            double west_intensity = result_8uc1_img.at<double>(y, x - 1);
            double east_intensity = result_8uc1_img.at<double>(y, x + 1);
        }      
    }
    
    // diplay images
    cv::imshow("Original", src_8uc1_img);
    cv::imshow( "Result", result_8uc1_img);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
