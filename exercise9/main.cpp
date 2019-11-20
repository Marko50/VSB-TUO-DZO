#include <math.h>
#include <algorithm> 
#include <opencv2/opencv.hpp>

using namespace std;

void calculate_histogram(cv::Mat src, int* histogram, int hist_size){
    for (int i = 0; i < hist_size; i++){
        for (int y = 0; y < src.rows; y++){
            for (int x = 0; x < src.cols; x++){
                if (src.at<uchar>(y,x) == (uchar) i){
                    histogram[i] += 1;
                }
            }
        }
    }
}

void calculate_cumulative_histogram(int * histogram, int * cumulative_histogram, int size){
    int acumulator = 0;
    for (int i = 0; i < size; i++){
        cumulative_histogram[i] = histogram[i] + acumulator;
        acumulator = cumulative_histogram[i];
    }  
}

void calculate_equalized_image(cv::Mat src, cv::Mat * result, int * cumulative_histogram, int size){
    int* min_value_iterator = min_element(cumulative_histogram, cumulative_histogram + size);
    uchar lut[256] = {0};
    for (int i = 0; i < 256; i++){
	    lut[i] = (uchar) round((cumulative_histogram[i] - *min_value_iterator)*(256-1)/((src.rows * src.cols) - *min_value_iterator));
    }
    for (int y = 0; y < src.rows; y++){
        for (int x = 0; x < src.cols; x++){
            uchar original_value = src.at<uchar>(y,x);
            result->at<uchar>(y,x) = lut[original_value];
        }
    }
}

int main(){
    cv::Mat src_8uc3_img = cv::imread("uneq.jpg", cv::IMREAD_GRAYSCALE);
    if (src_8uc3_img.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }

    cv::Mat result_8uc3_img = src_8uc3_img.clone();
    int histogram[256] = {0};
    int cumulative_histogram[256];

    calculate_histogram(src_8uc3_img, histogram, 256);
    calculate_cumulative_histogram(histogram, cumulative_histogram, 256);
    calculate_equalized_image(src_8uc3_img, &result_8uc3_img, cumulative_histogram, 256);

    cv::imshow("Original", src_8uc3_img);
    cv::imshow("Result", result_8uc3_img);
    
    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}