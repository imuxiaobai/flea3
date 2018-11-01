#include "flea3/stereo_node.h"
#include "sensor_msgs/Imu.h"

void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg);
flea3::StereoNode* stereo_node_ptr;

int main(int argc, char** argv) {
  ros::init(argc, argv, "flea3_stereo");
  ros::NodeHandle pnh("~");

  try {
    stereo_node_ptr = new flea3::StereoNode(pnh);
    stereo_node_ptr->Run();
    ros::Subscriber sub = pnh.subscribe("/imu/imu", 500, callback_trigger);
    ros::spin();
    // while(ros::ok()){
    //  stereo_node_ptr->Acquire();
      // usleep(10000);
      // ros::spinOnce();
    // }

    stereo_node_ptr->End();
  } catch (const std::exception& e) {
    ROS_ERROR("%s: %s", pnh.getNamespace().c_str(), e.what());
  }
}


void callback_trigger(const sensor_msgs::ImuConstPtr & imu_msg)
{
  stereo_node_ptr->Acquire();
}

// flea3::SingleNode single_node(pnh);
//     stereo_node.Run();
//     ros::spin();
//     stereo_node.End();