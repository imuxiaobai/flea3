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
const double max_shuttertime = 16.0;
const double min_gain = 15.0;
const int brightness_targ = 70;
double shuttertime_next;
double gain_next;
int brightness_curr = 0;
std_msgs::Int16 brightness_pub;

flea3::Setshuttertime* srv_ptr;
ros::ServiceClient* client_ptr;
ros::Time last_cb_timestamp;

void image_left_cb(const sensor_msgs::ImageConstPtr& image_left_msg) {
  //  if(shuttertime.data == 0 || gain.data == 0) {
  //    return;
  //  }
   if (ros::Time::now() - last_cb_timestamp > ros::Duration(0.2)) {
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
   cv::Mat image_before_median(22, 40, CV_8UC1);
   cv::Mat image_after_median(22, 40, CV_8UC1);

  //  cv::medianBlur(cv_ptr->image, image_median, 7);
   int temp_brightness = 0;
   for(int i = 0; i <= 39; i++) {
     for (int j = 10; j<= 31; j++) {
       image_before_median.at<uchar>(j - 10, i) = cv_ptr->image.at<uchar>(16 + 32 * j, 16 + 32 * i) ;
      //  temp_brightness += image_median.at<uchar>(16 + 32 * j, 16 + 32 * i);
     }
   }
  cv::medianBlur(image_before_median, image_after_median, 5);
  // image_after_median = image_before_median;
   for(int i = 0; i <= 39; i++) {
     for (int j = 10; j<= 31; j++) {
       temp_brightness += image_after_median.at<uchar>(j, i);
     }
   }
   brightness_curr = temp_brightness / (40 * 22);
   
  //  printf("brightness: %d \n", brightness_curr);

  //  if(brightness_curr == 0){
  //    return;
  //  }

   double shuttertime_curr;
   shuttertime_curr = shuttertime.data;
   double gain_curr;
   gain_curr = gain.data;

  if(brightness_curr < brightness_targ){
    if(brightness_curr >= 60 && brightness_curr <= 80){
      shuttertime_next = shuttertime_curr;
      gain_next = gain_curr;
    } else {
      shuttertime_next = shuttertime_curr * brightness_targ / brightness_curr;
      if(shuttertime_next > max_shuttertime) {
        gain_next = gain_curr * brightness_targ / brightness_curr;
        shuttertime_next = max_shuttertime;
      } else {
        gain_next = gain_curr;
      }
    }
  } else {
    if(brightness_curr >= 60 && brightness_curr <= 80) {
      shuttertime_next = shuttertime_curr;
      gain_next = gain_curr;
    } else {
      gain_next = gain_curr * brightness_targ / brightness_curr;
      if(gain_next < min_gain) {
        shuttertime_next = shuttertime_curr * brightness_targ / brightness_curr;
        gain_next = min_gain;
      } else {
        shuttertime_next = shuttertime_curr;
      }
    }
   }
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
    flea3::Setshuttertime srv;
    ros::ServiceClient client = n.serviceClient<flea3::Setshuttertime>("/setshuttertime");

    srv_ptr = &srv;
    client_ptr = &client;
    
    ros::Subscriber shuttertime_sub = n.subscribe("/shuttertime", 20, shuttertime_cb);
    ros::Subscriber gain_sub = n.subscribe("/gain", 20, gain_cb);
    ros::Subscriber image_left_sub = n.subscribe("/stereo/left/image_raw", 2, image_left_cb);
    ros::Publisher brightness_curr_pub = n.advertise<std_msgs::Int16>("/brightness_curr", 10);
    //ros::spin();
    last_cb_timestamp = ros::Time::now();

    ros::Rate loop_rate(5);
    while(ros::ok()) {
      brightness_pub.data = brightness_curr; 
      brightness_curr_pub.publish(brightness_pub);
      ros::spinOnce();
      loop_rate.sleep();
      if(shuttertime_next == 0 || gain_next == 0) {
        continue;
      }
      srv.request.shuttertime = shuttertime_next;
      srv.request.gain = gain_next;
      // printf("shuttertime_next: %f , gain_next: %f \n", shuttertime_next, gain_next);
      if (brightness_curr > 80 || brightness_curr < 60) {
        if(client.call(srv)) {
          ROS_INFO_ONCE("CALL SRV");
        } else {
          ROS_ERROR("Failed to call service setshuttertime");
          return 1;
        }
      }
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