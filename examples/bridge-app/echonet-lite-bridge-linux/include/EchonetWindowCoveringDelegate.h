#pragma once



#ifndef EchonetWindowCoveringDelegate_H
#define EchonetWindowCoveringDelegate_H



#include <app/clusters/window-covering-server/window-covering-server.h>
#include <app/clusters/window-covering-server/window-covering-delegate.h>
#include "../EchonetEndpoint.h"

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WindowCovering;


class EchonetWindowCoveringDelegate: public Delegate 
{
    public:
    EchonetEndpoint* echonetEndpoint;
    EchonetWindowCoveringDelegate(EchonetEndpoint* echonetEndpoint_) {echonetEndpoint=echonetEndpoint_;};
    CHIP_ERROR HandleMovement(WindowCoveringType type, int openPercent);
    CHIP_ERROR HandleStopMotion();
};




#endif