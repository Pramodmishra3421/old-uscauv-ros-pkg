/***************************************************************************
 *  include/seabee3_driver/bee_stem3_driver.h
 *  --------------------
 *
 *  Copyright (c) 2011,
 *    Edward T. Kaszubski ( ekaszubski@gmail.com )
 *    Rand Voorhies
 *    Michael Montalbo
 *
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
 *  * Neither the name of seabee3-ros-pkg nor the names of its
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

#ifndef SEABEE3DRIVER_BEESTEM3DRIVER_H_
#define SEABEE3DRIVER_BEESTEM3DRIVER_H_

#include <seabee3_driver/bee_stem3.h>
#include <ros/ros.h>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <sstream>

#define HEADING_K 0
#define HEADING_P 15
#define HEADING_I 0
#define HEADING_D 0

#define DEPTH_K 0
#define DEPTH_P 33
#define DEPTH_I 0
#define DEPTH_D 0

using namespace seabee3_common::movement;

class BeeStem3Driver
{
public:

    struct FiringDeviceParams
    {
        int trigger_time_;
        int trigger_value_;
    };

    struct BeeStemFlags
    {
        bool position_initialized_;
        bool port_connected_;

        BeeStemFlags()
        :
            position_initialized_ ( false ),
            port_connected_( false )
        {}
    };

    BeeStem3Driver();
    BeeStem3Driver( std::string const & port );
    ~BeeStem3Driver();

    void connect( std::string const & port, bool const & force_reconnect = false );
    void reconnect( std::string const & port );
    void reconnect();

    bool const & connected() const;

    void readPressure( int & intl_pressure, int & extl_pressure );
    void readKillSwitch( int8_t & kill_switch );

    bool const & getDeviceStatus( int const & device_id ) const;
    void fireDevice( int device_id );

    void setThruster( int id, int value );

    bool dropper1_ready_;
    bool dropper2_ready_;
    bool shooter1_ready_;
    bool shooter2_ready_;

    FiringDeviceParams shooter1_params_, shooter2_params_, dropper1_params_, dropper2_params_;

private:
    BeeStem3 bee_stem_3_;
    void initPose();

    std::string port_;

    BeeStemFlags flags_;
};

#endif // SEABEE3DRIVER_BEESTEM3DRIVER_H_
