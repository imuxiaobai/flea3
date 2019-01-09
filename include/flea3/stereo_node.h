#ifndef FLEA3_STEREO_NODE_H_
#define FLEA3_STEREO_NODE_H_

#include "flea3/flea3_ros.h"
#include "flea3/Flea3DynConfig.h"
#include "camera_base/camera_node_base.h"
#include "flea3/Setshuttertime.h"
#include "std_msgs/Float64.h"

namespace flea3 {

class StereoNode : public camera_base::CameraNodeBase<Flea3DynConfig> {
 public:
  explicit StereoNode(const ros::NodeHandle &pnh);

  virtual void Acquire() override;
  virtual void Setup(Flea3DynConfig &config) override;

  void Setshuttertime(double& shuttertime, double& gain);
  ros::ServiceServer setshuttertime_srv;
  ros::Publisher camera_shuttertime_pub;
  ros::Publisher camera_gain_pub;

  bool setst_srv_handle(flea3::Setshuttertime::Request &req, flea3::Setshuttertime::Response &res);

 private:
  Flea3Ros left_ros_;
  Flea3Ros right_ros_;
  ros::NodeHandle nph_;
  std_msgs::Float64 shuttertime_;
  std_msgs::Float64 gain_;
};

}  // namespace flea3

#endif  // FLEA3_STEREO_NODE_H_
