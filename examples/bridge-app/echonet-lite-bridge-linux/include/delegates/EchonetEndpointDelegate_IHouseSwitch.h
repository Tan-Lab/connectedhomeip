#pragma once
#ifndef EchonetEndpointDelegate_IHouseSwitch_H
#define EchonetEndpointDelegate_IHouseSwitch_H

#include "EchonetEndpointDelegate.h"
class EchonetEndpointDelegate_IHouseSwitch : public EchonetEndpointDelegate {
public:

    //EchonetEndpoint* ep;
    EchonetEndpointDelegate_IHouseSwitch(void* ep_):EchonetEndpointDelegate(ep_)
    {

    };
    int ReadProperty(chip::ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength);
    EmberAfStatus WriteProperty(chip::AttributeId attributeId, ClusterId clusterId, const EmberAfAttributeMetadata * attributeMetadata,uint8_t * buffer); 
    EmberAfStatus SetGET(unsigned char epc, vector<unsigned char> value);
};

#endif