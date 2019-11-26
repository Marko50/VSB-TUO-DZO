#include <math.h>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

const int NUMBER_OF_POINTS = 4;

const int LEFT_HAND_SIDE_ROWS = 8;
const int LEFT_HAND_SIDE_COLS = 8;
const int RIGHT_HAND_SIDE_ROWS = 8;
const int RIGHT_HAND_SIDE_COLS = 1;
const int TRANSFORMATION_ROWS = 3;
const int TRANSFORMATION_COLS = 3;

const int FIN_X[NUMBER_OF_POINTS] = { 66, 228, 227, 69 };
const int FIN_Y[NUMBER_OF_POINTS] = { 134, 122, 76, 107};

const int ORI_X[NUMBER_OF_POINTS] = {0, 323, 323, 0};
const int ORI_Y[NUMBER_OF_POINTS] = { 215, 215, 0, 0};

void generate_linear_system(cv::Mat & left_hand_side, cv::Mat & right_hand_side){
    int k = 0;
    for (int y; y < NUMBER_OF_POINTS; y++){
        int left_data_first_row[left_hand_side.cols] = { FIN_Y[y], 1, 0, 0, 0, -ORI_X[y]*FIN_X[y], -ORI_X[y]*FIN_Y[y], -ORI_X[y] };
        int left_data_second_row[left_hand_side.cols] = { 0, 0, FIN_X[y], FIN_Y[y], 1, -ORI_Y[y]*FIN_X[y], -ORI_Y[y]*FIN_Y[y], -ORI_Y[y] };
        for (int x = 0; x < left_hand_side.cols; x++){
            left_hand_side.at<float>(k,x) = left_data_first_row[x];
            left_hand_side.at<float>(k + 1,x) = left_data_second_row[x];
        }

        right_hand_side.at<float>(k, 0) = -FIN_X[y];
        right_hand_side.at<float>(k + 1, 0) = 0;
        k+=2;
    }
}

void generate_perspective_transformation(cv::Mat result_of_linear_system, cv::Mat & dest){
    dest.at<float>(0,0) = 1.0;
    int k = 0;
    for (int y = 0; y < dest.rows; y++){
        for (int x = (y == 0) ? 1 : 0; x < dest.cols; x++, k++){
            dest.at<float>(y,x) = result_of_linear_system.at<float>(k,0);
        }
    }
}

void calculate_perspective_transform_image(cv::Mat & image_background, cv::Mat image_to_be_transformed, cv::Mat transformation){
    for (int y = 0; y < image_background.rows; y++){
        for (int x = 0; x < image_background.cols; x++){
            float coords[3] = {(float) x , (float) y ,1};
            cv::Mat coordinates = cv::Mat(3,1, CV_32F);
            coordinates.at<float>(0,0) = coords[0];
            coordinates.at<float>(1,0) = coords[1];
            coordinates.at<float>(2,0) = coords[2];

            cv::Mat result = transformation * coordinates;

            float phi = result.at<float>(0,0);
            float psi = result.at<float>(1,0);
            float omega = result.at<float>(2,0);

            int projected_x = phi/omega;
            int projected_y = psi/omega;

            if (projected_x <= image_to_be_transformed.cols && projected_y <= image_to_be_transformed.rows && projected_x >= 0 && projected_y >= 0){
                image_background.at<cv::Vec3b>(y, x) = image_to_be_transformed.at<cv::Vec3b>(projected_y,projected_x);
            }
        }
    }
    cout << "Image Background Rows: " << image_background.rows << endl;
    cout << "Image Background Cols: " << image_background.cols << endl;
}

int main(){
    cv::Mat src_8uc3_img_flag = cv::imread("flag.png", cv::IMREAD_COLOR);
    cv::Mat src_8uc3_img_vsb = cv::imread("vsb.jpg", cv::IMREAD_COLOR);

    if (src_8uc3_img_flag.empty() || src_8uc3_img_vsb.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }

    cv::Mat background = src_8uc3_img_vsb.clone();
    cv::Mat left_hand_side = cv::Mat(LEFT_HAND_SIDE_ROWS, LEFT_HAND_SIDE_COLS, CV_32F , (float) 0);
    cv::Mat right_hand_side = cv::Mat(RIGHT_HAND_SIDE_ROWS, RIGHT_HAND_SIDE_COLS, CV_32F, (float) 0);
    cv::Mat result;

    generate_linear_system(left_hand_side, right_hand_side);
    if (!solve(left_hand_side, right_hand_side, result)){
        printf("No valid solution for the system was found\n");
        return -1;
    }
    
    cv::Mat transformation = cv::Mat(TRANSFORMATION_ROWS, TRANSFORMATION_COLS, CV_32F);
    generate_perspective_transformation(result, transformation);
    calculate_perspective_transform_image(background, src_8uc3_img_flag, transformation);
    
    cv::imshow("VSB", src_8uc3_img_vsb);
    cv::imshow("Flag", src_8uc3_img_flag);
    cv::imshow("Result", background);

    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}
