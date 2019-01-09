#include "flea3/single_node_.h"

namespace flea3 {

Trigger::Trigger(const ros::NodeHandle &pnh)
    : CameraNodeBase(pnh), flea3_trigger_(pnh) {}

void Trigger::Acquire() {
  // ros::Rate loop_rate(10);
  while (is_acquire() && ros::ok()) {

    // if (flea3_trigger_.RequestSingle()) {
      const auto expose_duration =
          ros::Duration(flea3_trigger_.camera().GetShutterTimeSec() / 2);
      const auto time = ros::Time::now() + expose_duration;
      flea3_trigger_.PublishCamera(time);
      // loop_rate.sleep();
      //      flea3_ros_.PublishImageMetadata(time);
      Sleep();
    // }
  }
}

void Trigger::Setup(Config &config) {
  flea3_trigger_.Stop();
  flea3_trigger_.camera().Configure(config);
  flea3_trigger_.set_fps(config.fps);
  flea3_trigger_.Start();
}

}  // namespace flea3
