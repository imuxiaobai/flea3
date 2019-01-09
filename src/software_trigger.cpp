#include "ros/ros.h"
#include "flea3/flea3_ros.h"
#include <sensor_msgs/fill_image.h>

#include <flycapture/FlyCapture2.h>

//using namespace FlyCapture2;


int main(int argc, char **argv){
    ros::init(argc, argv, "trigger_client");
    ros::NodeHandle n;
    ros::ServiceClient client = n.serviceClient<sensor_msgs::fill_image>("trigger", 1000);
    ros::Rate loop_rate(1);
    while (ros::ok()){
        flea3camer().FaireSoftwareTrigger(&cam);
        // FaireSoftwareTrigger(&cam);
        ros::spinOnce();
        loop_rate.sleep();
    }
    
return 0;
}
