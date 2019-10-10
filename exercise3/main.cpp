#include <stdio.h>

#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat src_8uc1_img = cv::imread( "lena.png", cv::IMREAD_GRAYSCALE ); // load grayscale image from file system to Mat variable, this will be loaded using 8 bits (uchar)
    
    int BOX_BLUR_DIMENSION = 3;
    int BOX_BLUR_NORMALIZATION = 9;
    int box_blur_data[BOX_BLUR_DIMENSION][BOX_BLUR_DIMENSION] = { {1, 1, 1}, {1, 1 , 1}, {1, 1, 1} };
    
    int GAUSSIAN_BLUR_DIMENSION = 3;
    int GAUSSIAN_BLUR_NORMALIZATION = 16;
    int gaussian_blur_data[GAUSSIAN_BLUR_DIMENSION][GAUSSIAN_BLUR_DIMENSION] = { {1, 2, 1}, {2, 4 , 2}, {1, 2, 1} };

    int GAUSSIAN_BLUR_DIMENSION_5 = 5;
    int GAUSSIAN_BLUR_NORMALIZATION_5 = 256;
    int gaussian_blur_data_5[GAUSSIAN_BLUR_DIMENSION_5][GAUSSIAN_BLUR_DIMENSION_5] = { {1, 4, 5, 4, 1}, {4, 16 , 24, 16, 4}, {6, 24, 36, 24, 6}, {4, 16, 24, 16, 4}, {1, 4, 6, 4, 1} };

    cv::Mat result_8uc1_img = src_8uc1_img.clone();
    cv::Mat result_8uc1_img2 = src_8uc1_img.clone();
    cv::Mat result_8uc1_img3 = src_8uc1_img.clone();

    if (src_8uc1_img.empty()) {
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
    }

    for (int y = 0; y < result_8uc1_img.rows - BOX_BLUR_DIMENSION; y++){
        for (int x = 0; x < result_8uc1_img.cols - BOX_BLUR_DIMENSION; x++){
            int value = 0;
            for (int i = 0; i < BOX_BLUR_DIMENSION; i++){
                for (int j = 0; j < BOX_BLUR_DIMENSION; j++){
                    int src_value = (int) src_8uc1_img.at<uchar>(y + i, x + j);
                    value += box_blur_data[i][j]*src_value;
                }
            }
            value *= 1.0f/(BOX_BLUR_NORMALIZATION);
            result_8uc1_img.at<uchar>(y,x) = (uchar) value;
        }
    }

    for (int y = 0; y < result_8uc1_img2.rows - GAUSSIAN_BLUR_DIMENSION; y++){
        for (int x = 0; x < result_8uc1_img2.cols - GAUSSIAN_BLUR_DIMENSION; x++){
            int value = 0;
            for (int i = 0; i < GAUSSIAN_BLUR_DIMENSION; i++){
                for (int j = 0; j < GAUSSIAN_BLUR_DIMENSION; j++){
                    int src_value = (int) src_8uc1_img.at<uchar>(y + i, x + j);
                    value += gaussian_blur_data[i][j]*src_value;
                }
            }
            value *= 1.0f/(GAUSSIAN_BLUR_NORMALIZATION);
            result_8uc1_img2.at<uchar>(y,x) = (uchar) value;
        }
    }

    for (int y = 0; y < result_8uc1_img3.rows - GAUSSIAN_BLUR_DIMENSION_5; y++){
        for (int x = 0; x < result_8uc1_img3.cols - GAUSSIAN_BLUR_DIMENSION_5; x++){
            int value = 0;
            for (int i = 0; i < GAUSSIAN_BLUR_DIMENSION_5; i++){
                for (int j = 0; j < GAUSSIAN_BLUR_DIMENSION_5; j++){
                    int src_value = (int) src_8uc1_img.at<uchar>(y + i, x + j);
                    value += gaussian_blur_data_5[i][j]*src_value;
                }
            }
            value *= 1.0f/(GAUSSIAN_BLUR_NORMALIZATION_5);
            result_8uc1_img3.at<uchar>(y,x) = (uchar) value;
        }
    }
    
    cv::imshow( "Original", src_8uc1_img );
    cv::imshow( "Box Blur Result", result_8uc1_img );
    cv::imshow( "Gaussian Blur Result 3x3", result_8uc1_img2 );
    cv::imshow( "Gaussian Blur Result 5x5", result_8uc1_img3 );

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
