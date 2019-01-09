#include "ros/ros.h"
#include <cstdlib>
#include "flea3/Setshuttertime.h"
#include <unistd.h>
#include "std_msgs/Float64.h"
#include "std_msgs/Int16.h"

#include "sensor_msgs/Image.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

std_msgs::Float64 shuttertime;
std_msgs::Float64 gain;
const double max_shuttertime = 19.0;
const double min_gain = 15.0;
const int brightness_targ = 70;
double gain_next;
int brightness_curr = 0;
std_msgs::Int16 brightness_pub;

ros::ServiceClient client;
// flea3::Setshuttertime* srv_ptr;
// ros::ServiceClient* client_ptr;
ros::Time last_cb_timestamp;

void image_left_cb(const sensor_msgs::ImageConstPtr& image_left_msg) {
  //  if(shuttertime.data == 0 || gain.data == 0) {
  //    return;
  //  }
   if (ros::Time::now() - last_cb_timestamp > ros::Duration(0.1)) {
     last_cb_timestamp = ros::Time::now();
   } else {
     return;
   }

   cv_bridge::CvImagePtr cv_ptr;
   try
   {
     cv_ptr = cv_bridge::toCvCopy(image_left_msg, sensor_msgs::image_encodings::MONO8);
   }
   catch (cv_bridge::Exception& e)
   {
     ROS_ERROR("cv_bridge exception: %s", e.what());
     return;
   } 

   // 1024 row          1280 col
   cv::Mat image_before_median(20, 40, CV_8UC1);
   cv::Mat image_after_median(20, 40, CV_8UC1);

  //  cv::medianBlur(cv_ptr->image, image_median, 7);
   int temp_brightness = 0;
   cv::Mat mat_mean, mat_stddev;
   for(int i = 0; i <= 39; i++) {
    //  for (int j = 0; j<= 21; j++) {
     for (int j = 0; j<= 19; j++) {
      //  image_before_median.at<uchar>(j , i) = cv_ptr->image.at<uchar>(16 + 32 * j, 16 + 32 * i) ;
      cv::meanStdDev(cv_ptr->image(cv::Range(1 + 32 * j, 32 + 32 * j), cv::Range(1 + 32 * i, 32 + 32 * i)), mat_mean, mat_stddev) ;
      image_before_median.at<uchar>(j , i) =  mat_mean.at<double>(0, 0);
      //  temp_brightness += image_median.at<uchar>(16 + 32 * j, 16 + 32 * i);
     }
   }
  cv::medianBlur(image_before_median, image_after_median, 5);
  // image_after_median = image_before_median;
   for(int i = 0; i <= 39; i++) {
    //  for (int j = 0; j<= 21; j++) {
     for (int j = 0; j<= 19; j++) {
       temp_brightness += image_after_median.at<uchar>(j, i);
     }
   }
   brightness_curr = temp_brightness / (40 * 20);
   
  //  printf("brightness: %d \n", brightness_curr);

  //  if(brightness_curr == 0){
  //    return;
  //  }

   double shuttertime_curr;
   shuttertime_curr = shuttertime.data;
   double gain_curr;
  //  gain_curr = gain.data;
  gain_curr = 15.0f;

  double shuttertime_next = 0.9 * shuttertime_curr * brightness_targ / (brightness_curr + 0.1f);
  shuttertime_next = std::min(shuttertime_next, max_shuttertime);
  double delta_shuttertime = std::abs(shuttertime_curr - shuttertime_next);

  double gain_st = 0.0f;
  if (shuttertime_next > shuttertime_curr) {
    gain_st = 1.0f;
  } else {
    gain_st = -1.0f;
  }
  // ROS_INFO("dst:  %.2f",delta_shuttertime);
  // ROS_INFO("st_ne:%.2f",shuttertime_next);
  // ROS_INFO("st_cr:%.2f",shuttertime_curr);
  if (delta_shuttertime > 1.75f) {
    shuttertime_next = shuttertime_curr + std::min(delta_shuttertime, 1.95) * gain_st;
    flea3::Setshuttertime srv;
    srv.request.shuttertime = shuttertime_next;
    srv.request.gain = gain_curr;
    if (client.call(srv)) {
      // ROS_INFO("%.4f",shuttertime_next);
      ROS_INFO_ONCE("CALL SRV");
    } else {
      ROS_ERROR("Failed to call service setshuttertime"); 
    }
  }

  // if(brightness_curr < brightness_targ){
  //   if(brightness_curr >= 60 && brightness_curr <= 80){
  //     shuttertime_next = shuttertime_curr;
  //     gain_next = gain_curr;
  //   } else {
  //     shuttertime_next = shuttertime_curr * brightness_targ / brightness_curr;
  //     if(shuttertime_next > max_shuttertime) {
  //       gain_next = gain_curr * brightness_targ / brightness_curr;
  //       shuttertime_next = max_shuttertime;
  //     } else {
  //       gain_next = gain_curr;
  //     }
  //   }
  // } else {
  //   if(brightness_curr >= 60 && brightness_curr <= 80) {
  //     shuttertime_next = shuttertime_curr;
  //     gain_next = gain_curr;
  //   } else {
  //     gain_next = gain_curr * brightness_targ / brightness_curr;
  //     if(gain_next < min_gain) {
  //       shuttertime_next = shuttertime_curr * brightness_targ / brightness_curr;
  //       gain_next = min_gain;
  //     } else {
  //       shuttertime_next = shuttertime_curr;
  //     }
  //   }
  //  }
   //  printf("shuttertime: %f , gain: %f \n", shuttertime_next, gain_next);
  //  srv_ptr->request.shuttertime = shuttertime_next;
  //  srv_ptr->request.gain = gain_next;
  //  client_ptr->call(*srv_ptr);


}

void shuttertime_cb(const std_msgs::Float64ConstPtr& shuttertime_msg) {
  shuttertime.data = shuttertime_msg->data;
}

void gain_cb(const std_msgs::Float64ConstPtr& gain_msg) {
  gain.data = gain_msg->data;
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "test_srv");
    ros::NodeHandle n;
    // flea3::Setshuttertime srv;
    // ros::ServiceClient client = n.serviceClient<flea3::Setshuttertime>("/setshuttertime");
    client = n.serviceClient<flea3::Setshuttertime>("/setshuttertime");

    // srv_ptr = &srv;
    // client_ptr = &client;
    
    ros::Subscriber shuttertime_sub = n.subscribe("/shuttertime", 2, shuttertime_cb);
    ros::Subscriber gain_sub = n.subscribe("/gain", 2, gain_cb);
    ros::Subscriber image_left_sub = n.subscribe("/camera_ns/stereo/left/image_raw", 2, image_left_cb);
    ros::Publisher brightness_curr_pub = n.advertise<std_msgs::Int16>("/brightness_curr", 10);
    //ros::spin();
    last_cb_timestamp = ros::Time::now();

    ros::Rate loop_rate(10);
    while(ros::ok()) {
      brightness_pub.data = brightness_curr; 
      brightness_curr_pub.publish(brightness_pub);
      ros::spinOnce();
      loop_rate.sleep();
      // if(shuttertime_next == 0 || gain_next == 0) {
      //   continue;
      // }
      // srv.request.shuttertime = shuttertime_next;
      // srv.request.gain = gain_next;
      // // printf("shuttertime_next: %f , gain_next: %f \n", shuttertime_next, gain_next);
      // if (brightness_curr > 80 || brightness_curr < 60) {
      //   if(client.call(srv)) {
      //     ROS_INFO_ONCE("CALL SRV");
      //   } else {
      //     ROS_ERROR("Failed to call service setshuttertime");
      //     return 1;
      //   }
      // }
    }

    // std_msgs::Float64 shutter_time;
    // std_msgs::Float64 cam_gain;
    // srv.request.shuttertime = shutter_time.data;
    // srv.request.gain = cam_gain.data;
    // while(ros::ok()) {
    //     if(client.call(srv)) {
    //         shutter_time.data++;
    //         srv.request.shuttertime = shutter_time.data;
    //         srv.request.gain = cam_gain.data;
    //         if(shutter_time.data > 40) {
    //             return 0;
    //         }
    //     } else {
    //         ROS_ERROR("Failed to call service setshuttertime");
    //         return 1;
    //     }
    //     sleep(1);
    //    ros::spinOnce();
    //    sleep(0.5);
    // }
}