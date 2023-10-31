#ifndef EchonetWindowCoveringDelegate_CPP_IHOUSE
#define EchonetWindowCoveringDelegate_CPP_IHOUSE

#include "include/EchonetWindowCoveringDelegate_IHouse.h"

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WindowCovering;

CHIP_ERROR EchonetWindowCoveringDelegate_IHouse::HandleMovement(WindowCoveringType type, int openPercent)
{
    printf("\n\n\n[[[[[[[[[[[ EchonetWindowCoveringDelegate_IHouse HandleMovement %d p=%d]]]]]]]]]]]]]\n\n\n\n\n\n",(int)type,openPercent);

    if(openPercent>50)
    {
        echonetEndpoint->SetGET(0xE0, {0x41});
    } else echonetEndpoint->SetGET(0xE0, {0x42});


    return CHIP_NO_ERROR; 
}
CHIP_ERROR EchonetWindowCoveringDelegate_IHouse::HandleStopMotion()
{
    printf("\n\n\n[[[[[[[[[[[ EchonetWindowCoveringDelegate_IHouse HandleStopMotion]]]]]]]]]]]]]\n\n\n\n\n\n");
    echonetEndpoint->SetGET(0xE0, {0x43});

    return CHIP_NO_ERROR;
}






#endif