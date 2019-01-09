#ifndef FLEA3_TRIGGER_H_
#define FLEA3_TRIGGER_H_

#include <camera_base/camera_ros_base.h>
#include <camera_base/camera_node_base.h>
#include "flea3/flea3_ros.h"
#include "flea3/Flea3DynConfig.h"
#include <ros/ros.h>
namespace flea3 {

using Config = ::flea3::Flea3DynConfig;

// class Trigger {
 class Trigger : public camera_base::CameraNodeBase<Config> {
 public:
  //explicit Trigger(const ros::NodeHandle &pnh);
  Trigger(const ros::NodeHandle &pnh); 
  
  // bool _trigger() {
    // return true;
  // }

  void Acquire() override;
  // {
  //   ros::Rate loop_rate(10);
  //   while(is_acquire() && ros::ok())
  //     {
  //       if (flea3_trigger_.RequestSingle()) {

  //       const auto expose_duration =
  //           ros::Duration(flea3_trigger_.camera().GetShutterTimeSec() / 2);
  //       const auto time = ros::Time::now() + expose_duration;
  //       flea3_trigger_.PublishCamera(time);
  //       // ros::spinOnce();
  //       loop_rate.sleep();
  //     }
  //   }
  // }
  void Setup(Config &config) override;
  // {
  // flea3_trigger_.Stop();
  // flea3_trigger_.camera().Configure(config);
  // flea3_trigger_.set_fps(config.fps);
  // flea3_trigger_.Start();
  // }

 private:
  Flea3Ros flea3_trigger_;
};

}  // namespace flea3

#endif
