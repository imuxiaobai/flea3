#include "flea3/single_node_.h"

// void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg);

// flea3::Trigger* trigger_ptr;

int main(int argc, char** argv) {
  ros::init(argc, argv, "trigger");
  ros::NodeHandle pnh("~");

   try {
    // trigger_ptr = new flea3::Trigger(pnh);
    flea3::Trigger trigger(pnh);
    trigger.Run();
    // ros::Subscriber sub = pnh.subscribe("/imu/imu", 500, callback_trigger);
    ros::spin();
    trigger.End();
  } catch (const std::exception& e) {
   ROS_ERROR("%s: %s", pnh.getNamespace().c_str(), e.what());
  }
}

// void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg)
// {
  // single_node_ptr->Acquire();
// }
