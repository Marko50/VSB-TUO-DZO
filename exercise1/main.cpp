#include <stdio.h>

#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "moon.jpg", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    cv::Mat result_8uc1_img = src_8uc1_img.clone(); // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)

    double min,max;
    cv::minMaxLoc(src_8uc1_img, &min, &max);
    
    for (int y = 0; y < result_8uc1_img.rows; y++) {
        for (int x = 0; x < result_8uc1_img.cols; x++) {
            uchar old = result_8uc1_img.at<uchar>(y,x);
            result_8uc1_img.at<uchar>(y,x) = (uchar) ((old - min) * (255/(max-min)));       
        }
    }

    // diplay images
    cv::imshow("Original", src_8uc1_img);
    cv::imshow( "Result", result_8uc1_img);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
