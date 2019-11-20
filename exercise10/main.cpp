#include <math.h>
#include <algorithm> 

#include <opencv2/opencv.hpp>

using namespace std;



int main(){
    cv::Mat src_8uc3_img = cv::imread("uneq.jpg", cv::IMREAD_COLOR);
    if (src_8uc3_img.empty()){
        printf("Unable to read input file (%s, %d).", __FILE__, __LINE__);
        return -1;
    }
    
    cv::waitKey( 0 ); // wait until keypressed

    return 0;
}