#include <iostream>
#include <fstream>

#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>

#include <image_transport/image_transport.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_; 
//void initialize_callbacks()
static const std::string OPENCV_WINDOW = "QUADCOPTER POV";

void images_callback(const sensor_msgs::ImageConstPtr &imageMsg) 
{
    cv_bridge::CvImagePtr cv_ptr; 

    try {
        cv_ptr = cv_bridge::toCvCopy(imageMsg, sensor_msgs::image_encodings::RGB8);
        
    } catch (cv_bridge::Exception &e) {
        throw std::runtime_error(std::string("FUCKED: cv_bridge exception: ") + std::string(e.what()));
    }
    
    cv::namedWindow(OPENCV_WINDOW);

    // Draw a crosshair so it looks legit
    if (cv_ptr->image.rows > 0 && cv_ptr->image.cols > 0)
      cv::drawMarker(cv_ptr->image, cv::Point(cv_ptr->image.cols/2, cv_ptr->image.rows/2), CV_RGB(255,000,0), 0, 40, 2);

    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());
//
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "cv_node");
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_(nh_);
    image_transport::Subscriber image_sub_ = it_.subscribe("/camera1/image_raw", 1, images_callback);
    image_pub_ = it_.advertise("/out", 1);
    ros::spin();

    cv::destroyWindow(OPENCV_WINDOW);
    return 0;
}