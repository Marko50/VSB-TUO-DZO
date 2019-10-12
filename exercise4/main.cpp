#include <stdio.h>
#include <math.h>

#include <opencv2/opencv.hpp>

const double TETA = 0.015;
const double LAMBDA = 0.1;

double calculate_conductance(double intensity, double neighbour){
    return exp(- ((intensity - neighbour)*(intensity - neighbour))/(TETA*TETA));
}

double calculate_output_intensity(double intensity, double north, double south, double west, double east){
    
    double cn = calculate_conductance(intensity , north);
    double cs = calculate_conductance(intensity, south);
    double cw = calculate_conductance(intensity, west);
    double ce = calculate_conductance(intensity, east);

    double first_term = intensity*( 1 - LAMBDA * (cn + cs + cw + ce));
    double second_term = LAMBDA * (cn*north + cs*south + ce*east + cw*west);
    return first_term + second_term;
}


int main()
{
    cv::Mat src_8uc1_img = cv::imread( "valve.png", cv::IMREAD_GRAYSCALE ); // load color image from file system to Mat variable, this will be loaded using 8 bits (uchar)
    
    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }
    
    cv::Mat src_32fc1_img;
    src_8uc1_img.convertTo(src_32fc1_img, CV_64FC1, 1/255.0);

    cv::Mat result_32fc1_img = src_32fc1_img.clone(); // declare variable to hold grayscale version of img variable, gray levels wil be represented using 8 bits (uchar)

    for (int i = 0; i < 500; i++)
    {
        for (int y = 1; y < result_32fc1_img.rows; y++){
            for (int x = 1; x < result_32fc1_img.cols; x++){
                double intensity = result_32fc1_img.at<double>(y,x);
                double north_intensity = result_32fc1_img.at<double>(y - 1, x);
                double south_intensity = result_32fc1_img.at<double>(y + 1, x);
                double west_intensity = result_32fc1_img.at<double>(y, x - 1);
                double east_intensity = result_32fc1_img.at<double>(y, x + 1);
                result_32fc1_img.at<double>(y,x) = calculate_output_intensity(intensity, north_intensity, south_intensity, west_intensity, east_intensity);
            }      
        }
    }
    
    // diplay images
    cv::imshow("Original", src_32fc1_img);
    cv::imshow( "Result", result_32fc1_img);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
