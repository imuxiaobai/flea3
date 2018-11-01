
#include "flea3/trigger.h"
namespace flea3{

  //  Flea3Ros flea3_trigger;
  Trigger::Trigger(const ros::NodeHandle &pnh)
      : CameraNodeBase(pnh), flea3_trigger_(pnh) {}
   
  void Trigger::Acquire() {
    // ros::Rate loop_rate(10);
    while(is_acquire() && ros::ok())
      {
        if (flea3_trigger_.RequestSingle()) {

        const auto expose_duration =
            ros::Duration(flea3_trigger_.camera().GetShutterTimeSec() / 2);
        const auto time = ros::Time::now() + expose_duration;
        flea3_trigger_.PublishCamera(time);
        // ros::spinOnce();
        // loop_rate.sleep();
        Sleep();
      }
    }
  }
  void Trigger::Setup(Config &config) {
  flea3_trigger_.Stop();
  flea3_trigger_.camera().Configure(config);
  flea3_trigger_.set_fps(config.fps);
  flea3_trigger_.Start();
  }
  //  Trigger::Trigger(const ros::NodeHandle &pnh)
    // {}
 
}
 int main(int argc , char** argv){
    ros::init(argc,argv,"flea3_trigger");
    ros::NodeHandle pnh("~");
    flea3::Trigger trigger(pnh);
    trigger.Run();
    
        // trigger._trigger();
        ros::spin();
        trigger.End();
    //ros::ServiceServer service = pnh.advertiseService("trigger",trigger);
    return 0;
  
  }