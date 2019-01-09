#include "flea3/stereo_node.h"

namespace flea3 {

StereoNode::StereoNode(const ros::NodeHandle& pnh)
    : CameraNodeBase(pnh), left_ros_(pnh, "left"), right_ros_(pnh, "right"), nph_(pnh) {
      setshuttertime_srv = nph_.advertiseService("/setshuttertime", &StereoNode::setst_srv_handle, this);
      camera_shuttertime_pub = nph_.advertise<std_msgs::Float64>("/shuttertime", 10);
      camera_gain_pub = nph_.advertise<std_msgs::Float64>("/gain", 10);
    }

void StereoNode::Acquire() {
  while (is_acquire() && ros::ok()) {
    //if (left_ros_.RequestSingle()&right_ros_.RequestSingle()) {
    if (left_ros_.RequestSingle()){
      //left_ros_.RequestSingle(); 
      ros::Duration expose_duration =
          ros::Duration(left_ros_.camera().GetShutterTimeSec() / 2);
      ros::Duration right_expose_duration = ros::Duration(right_ros_.camera().GetShutterTimeSec()/2);
      // ROS_INFO("%.6f,  %.6f", expose_duration.toSec(), right_expose_duration.toSec());
      ros::Time time_now = ros::Time::now();
      ros::Time time = time_now + expose_duration;
      left_ros_.PublishCamera(time);
      ros::Time time2 = time_now + right_expose_duration;
      right_ros_.PublishCamera(time2);
      shuttertime_.data = 1000 * expose_duration.toSec() * 2;
      gain_.data = left_ros_.camera().GetGain();
      // ROS_INFO("%.6f, %.6f", shuttertime_.data, gain_.data);
      camera_shuttertime_pub.publish(shuttertime_);
      camera_gain_pub.publish(gain_);
      Sleep();
    }
  }
  
}

void StereoNode::Setup(Flea3DynConfig& config) {
  left_ros_.Stop();
  right_ros_.Stop();
  // config.fps = 60;
  left_ros_.set_fps(config.fps);
  right_ros_.set_fps(config.fps);
  Flea3DynConfig config_cpy = config;
  config_cpy.trigger_mode = 0;
  config_cpy.strobe_control = 2;
  // config_cpy.brightness = 10;
  left_ros_.camera().Configure(config_cpy);
  config.trigger_mode = 0;
  // config.brightness = 10;
  config.trigger_source = 2;
  right_ros_.camera().Configure(config);
  left_ros_.Start();
  right_ros_.Start();
}

void StereoNode::Setshuttertime(double& shuttertime, double& gain) {
  bool no_auto_shutter_and_gain = false;
  left_ros_.Stop();
  right_ros_.Stop();
  left_ros_.camera().SetShutter(no_auto_shutter_and_gain , shuttertime);
  right_ros_.camera().SetShutter(no_auto_shutter_and_gain , shuttertime);
  left_ros_.camera().SetGain(no_auto_shutter_and_gain , gain);
  right_ros_.camera().SetGain(no_auto_shutter_and_gain , gain);
  left_ros_.Start();
  right_ros_.Start();
}

bool StereoNode::setst_srv_handle(flea3::Setshuttertime::Request &req,
                                  flea3::Setshuttertime::Response &res) {
                                    double st = req.shuttertime;
                                    double g = req.gain;
                                    Setshuttertime(st,g);
                                    res.success = true;
                                    return true;
                                  }

}  // namespace flea3
