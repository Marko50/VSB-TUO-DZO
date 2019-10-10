#include <stdio.h>

#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "moon.jpg", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar) 1 channel
    cv::Mat src_32fc1_img; // declare variable to hold grayscale version of img variable, gray levels wil be represented using 32 bits (float)
    
    src_8uc1_img.convertTo(src_32fc1_img, CV_32FC1, 1.0/255.0);
    
    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    cv::Mat result_32cu1_img = src_32fc1_img.clone();
    cv::Mat result_32cu1_img_2 = src_32fc1_img.clone();
    cv::Mat result_32cu1_img_3 = src_32fc1_img.clone();

    float gamma = 0.5f;

    for (size_t y = 0; y < result_32cu1_img.rows; y++)
    {
        for (size_t x = 0; x < result_32cu1_img.cols; x++)
        {
            float input = result_32cu1_img.at<float>(y,x);
            result_32cu1_img.at<float>(y,x) = pow(input, gamma);        
        }
    }

    gamma = 1.5f;

    for (size_t y = 0; y < result_32cu1_img_2.rows; y++)
    {
        for (size_t x = 0; x < result_32cu1_img_2.cols; x++)
        {
            float input = result_32cu1_img_2.at<float>(y,x);
            result_32cu1_img_2.at<float>(y,x) = pow(input, gamma);        
        }
    }

    gamma = 2.0f;

    for (size_t y = 0; y < result_32cu1_img_3.rows; y++)
    {
        for (size_t x = 0; x < result_32cu1_img_3.cols; x++)
        {
            float input = result_32cu1_img_3.at<float>(y,x);
            result_32cu1_img_3.at<float>(y,x) = pow(input, gamma);        
        }
    }

    cv::imshow("ORIGINAL", src_32fc1_img);
    cv::imshow("RESULT 0.5f", result_32cu1_img);
    cv::imshow("RESULT 1.5f", result_32cu1_img_2);
    cv::imshow("RESULT 2.0f", result_32cu1_img_3);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
