#include "flea3/single_node.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Imu.h"

void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg);

flea3::SingleNode* single_node_ptr;

int main(int argc, char** argv) {
  ros::init(argc, argv, "flea3_single");
  ros::NodeHandle pnh("~");

   try {
    single_node_ptr = new flea3::SingleNode(pnh);
    single_node_ptr->Run();
    ros::Subscriber sub = pnh.subscribe("/imu/imu", 500, callback_trigger);
    ros::spin();
    single_node_ptr->End();
  } catch (const std::exception& e) {
   ROS_ERROR("%s: %s", pnh.getNamespace().c_str(), e.what());
  }
}

void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg)
{
  single_node_ptr->Acquire();
}
