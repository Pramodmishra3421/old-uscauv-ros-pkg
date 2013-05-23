/***************************************************************************
 *  include/auv_tasks/teleop_policy.h
 *  --------------------
 *
 *  Copyright (c) 2013, Dylan Foster
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of USC AUV nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>


#define TELEOPPOLICY_CHECK_ENABLED()					\
  if ( !(initialized_ && cached_) ){					\
    ROS_DEBUG("Teleop policy [ %s ] is not ready.", name_.c_str() ); return false; }

class TeleopPolicy
{
 private:
  typedef std::map<std::string, std::string> _ButtonMap;
  typedef std::map<std::string, unsigned int> _ButtonIndexMap;
  typedef sensor_msgs::Joy _JoyMsg;
  typedef XmlRpc::XmlRpcValue _XmlVal;
  
  /// ROS interfaces
  ros::NodeHandle nh_rel_;
  ros::Subscriber joy_sub_;

  /// variables
  std::string name_;

  bool initialized_, cached_;
  long unsigned int msg_count_;

  /// TODO: Initialize to zero
  _JoyMsg last_joystick_message_, second_last_joystick_message_;
  
  _ButtonIndexMap axes_msg_map_, button_msg_map_;

  _ButtonMap axes_map_, button_map_;

 public:
  TeleopPolicy(std::string const & name):
  nh_rel_("~"),
  name_(name),
  initialized_( false ),
  cached_( false ),
  msg_count_( 0 )
    {}

  void init()
  {
    ros::NodeHandle nh;
    _XmlVal assignments;

    /// subscribe to joystick messages
    joy_sub_ = nh_rel_.subscribe("joy", 1, &TeleopPolicy::joyCallback, this );

    
    /// load the button mappings ------------------------------------
    if( !nh.getParam( "joystick/assignments", assignments ))
      {
	ROS_WARN("Teleop policy [ %s ] failed to load joystick assignments.", name_.c_str());
	return;
      }

    _XmlVal & high_level = assignments["high_level"];
    _XmlVal & low_level = assignments["low_level"]
;
    _XmlVal & axes = high_level["axes"];
    _XmlVal & buttons = high_level["buttons"];
    _XmlVal & axes_idx = low_level["axes"];
    _XmlVal & buttons_idx = low_level["buttons"];
    
    for( _XmlVal::iterator axes_it = axes.begin(); axes_it != axes.end(); ++axes_it )
      {
    	axes_map_[axes_it->first] = std::string(axes_it->second);
      }
    
    for( _XmlVal::iterator buttons_it = buttons.begin(); buttons_it != buttons.end(); ++buttons_it )
      {
    	button_map_[buttons_it->first] = std::string(buttons_it->second);
      }

    for( _XmlVal::iterator axes_it = axes_idx.begin(); axes_it != axes_idx.end(); ++axes_it )
      {
    	axes_msg_map_[axes_it->first] = int(axes_it->second);
      }
    
    for( _XmlVal::iterator buttons_it = buttons_idx.begin(); buttons_it != buttons_idx.end(); ++buttons_it )
      {
    	button_msg_map_[buttons_it->first] = int(buttons_it->second);
      }
    
    initialized_ = true;
    return;
  }
  
  /// TODO: Fix cached criteria
  void joyCallback(const _JoyMsg::ConstPtr & msg )
  {
    second_last_joystick_message_ = last_joystick_message_;

    last_joystick_message_ = *msg;

    msg_count_++;

    if ( msg_count_ > 1)
      cached_ = true;
    
    return;
  }

  bool getButton(std::string const & button_name )
  {
    TELEOPPOLICY_CHECK_ENABLED();
    
    return last_joystick_message_.buttons[ button_msg_map_ [ button_map_[ button_name ] ] ];   
  }

  bool getButtonAquired(std::string const & button_name )
  {
    TELEOPPOLICY_CHECK_ENABLED();
    
    unsigned int const & button_idx = button_msg_map_ [ button_map_[ button_name ] ];

    return last_joystick_message_.buttons[ button_idx ]
      && ! second_last_joystick_message_.buttons[ button_idx ];   
  }

  bool getButtonReleased(std::string const & button_name )
  {
    TELEOPPOLICY_CHECK_ENABLED();
    
    unsigned int const & button_idx = button_msg_map_ [ button_map_[ button_name ] ];

    return ! last_joystick_message_.buttons[ button_idx ]
      && second_last_joystick_message_.buttons[ button_idx ];   
  }

  float getAxis(std::string const & axis_name )
  {
    TELEOPPOLICY_CHECK_ENABLED();

    return last_joystick_message_.axes[ axes_msg_map_[ axes_map_[ axis_name ] ] ];
  }

  float getButtonsAsAxis(std::string const & name1, std::string const & name2)
  {
    TELEOPPOLICY_CHECK_ENABLED();

    return last_joystick_message_.buttons[ button_msg_map_[ button_map_[ name1 ] ] ],
      - last_joystick_message_.buttons[ button_msg_map_[ button_map_[ name2 ] ]];   
  }

};
