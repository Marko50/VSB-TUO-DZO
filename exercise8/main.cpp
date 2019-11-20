#include <math.h>

#include <opencv2/opencv.hpp>

using namespace std;

struct RLDUserData {
    cv::Mat & src_8uc3_img;
    cv::Mat & undistorted_8uc3_img;
    int K1;
    int K2;

    RLDUserData( const int K1, const int K2, cv::Mat & src_8uc3_img, cv::Mat & undistorted_8uc3_img ) : K1( K1 ), K2( K2 ), src_8uc3_img( src_8uc3_img ), undistorted_8uc3_img( undistorted_8uc3_img ) {

    }
};

void image_center(int * center_x, int * center_y, cv::Mat src_8uc3_img){
    *center_x = src_8uc3_img.cols/2;
    *center_y = src_8uc3_img.rows/2;
}

void coordinate_means(double * x_mean, double * y_mean, int center_x, int center_y, int deformed_x , int deformed_y){
    double R = sqrt(center_x * center_x + center_y * center_y);
    *x_mean = (deformed_x - center_x)/R ; 
    *y_mean = (deformed_y - center_y)/R ; 
}

double taylor_r(double x_mean, double y_mean){
    return x_mean * x_mean + y_mean * y_mean;
}

double taylor(double t_r, double K1, double K2){
    return (1 + K1*t_r + K2*t_r*t_r);
}

void geom_dist( cv::Mat & src_8uc3_img, cv::Mat & dst_8uc3_img, bool bili, double K1 = 1.0, double K2 = 1.0 )
{
    int center_x,center_y;
    image_center(&center_x, &center_y , src_8uc3_img);

    for (int y = 0; y < dst_8uc3_img.rows; y++){
        for (int x = 0; x < dst_8uc3_img.cols ; x++){
            double x_mean, y_mean;
            coordinate_means(&x_mean, &y_mean, center_x, center_y, x ,y);
            double t_r = taylor_r(x_mean, y_mean);
            double expansion = taylor(t_r, K1, K2);
            double original_x = (x - center_x)*(1/expansion) + center_x;
            double original_y = (y - center_y)*(1/expansion) + center_y;
            dst_8uc3_img.at<cv::Vec3b>(y,x) = src_8uc3_img.at<cv::Vec3b>((int)original_y, (int)original_x); //NEAREST NEIGHBOURS

        }
        
    }
    
}

void apply_rld( int id, void * user_data )
{
    RLDUserData *rld_user_data = (RLDUserData*)user_data;
    geom_dist( rld_user_data->src_8uc3_img, rld_user_data->undistorted_8uc3_img, !false, rld_user_data->K1 / 100.0, rld_user_data->K2 / 100.0 );
    cv::imshow( "Geom Dist", rld_user_data->undistorted_8uc3_img );
}

int ex_rld()
{
    cv::Mat src_8uc3_img, geom_8uc3_img;
    RLDUserData rld_user_data( 3.0, 1.0, src_8uc3_img, geom_8uc3_img );

    src_8uc3_img = cv::imread( "distorted_window.jpg", CV_LOAD_IMAGE_COLOR );
    if ( src_8uc3_img.empty() )
    {
        printf( "Unable to load image!\n" );
        exit( -1 );
    }
	
    cv::namedWindow( "Original Image" );
    cv::imshow( "Original Image", src_8uc3_img );

    geom_8uc3_img = src_8uc3_img.clone();
    apply_rld( 0, (void*)&rld_user_data );

    cv::namedWindow( "Geom Dist" );
    cv::imshow( "Geom Dist", geom_8uc3_img );

    cv::createTrackbar( "K1", "Geom Dist", &rld_user_data.K1, 100, apply_rld, &rld_user_data );
    cv::createTrackbar( "K2", "Geom Dist", &rld_user_data.K2, 100, apply_rld, &rld_user_data );

    cv::waitKey( 0 );

    return 0;
}

int main(){
    ex_rld();
    return 0;
}


