#pragma once
#ifndef ECHONETENDPOINTINFO_DELEGATE_H
#define ECHONETENDPOINTINFO_DELEGATE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "CommissionableInit.h"
#include "Device.h"
#include "main.h"
#include <app/server/Server.h>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <map>
//#include "Utils.h"
//#include <app/clusters/mode-select-server/supported-modes-manager.h>
//#include "MatterMetaData.h"
#include <app/util/af.h>
#include <app/util/config.h>
#include <iostream>
#include <memory>
#include "openecho/OpenECHO.h"
#include <mutex>
#include "AttributePropertyAdapter.h"
using namespace std;
// OpenECHOのnamespaceは"sonycsl_openecho"
using namespace sonycsl_openecho;
using namespace chip;
using namespace std;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;


//#include "../../EchonetEndpoint.h"
class EchonetEndpointDelegate  {
public:
    void* myEchonetEndpointpointer;
    EchonetEndpointDelegate(void* myEchonetEndpointpointer_) {this->myEchonetEndpointpointer = myEchonetEndpointpointer_;};
    virtual ~EchonetEndpointDelegate(){};
    virtual int ReadProperty(chip::ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength) =0;
    virtual EmberAfStatus WriteProperty(chip::AttributeId attributeId, ClusterId clusterId, const EmberAfAttributeMetadata * attributeMetadata,uint8_t * buffer) =0; 
    virtual EmberAfStatus SetGET(unsigned char epc, vector<unsigned char> value) =0;
};

#endif