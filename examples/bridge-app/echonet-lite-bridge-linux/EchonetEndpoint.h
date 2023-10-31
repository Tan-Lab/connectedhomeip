#pragma once
#ifndef ECHONETENDPOINTINFO_H
#define ECHONETENDPOINTINFO_H
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
#include "Utils.h"
#include <app/util/af.h>
#include <app/util/config.h>
#include "include/EchonetSelectModeSupportedModes.h"
#include <iostream>
#include <memory>
#include "openecho/OpenECHO.h"
#include <mutex>
#include "AttributePropertyAdapter.h"
#include "include/delegates/EchonetEndpointDelegate_IHouseWindowCovering.h"
#include "include/delegates/EchonetEndpointDelegate_IHouseWindow.h"
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

struct PropertyValues{
    vector<unsigned char> echonetValue;
    string matterValueString;
};

class EchonetEndpoint
{
    public:
        
        string address;
        unsigned short echoClassCode;
        unsigned char instanceCode;
        pair<string,unsigned int> eoj_pair;
        MatterEchonetLITECombineEndpointType type;

        Device* device;
        EmberAfEndpointType* emberAfEndpointType;
        Span<const EmberAfDeviceType> deviceTypeList;
        Span<DataVersion> dataVersionStorage;
        EchonetOptionType echonetOptionType; 

        vector<ModeSelect::Structs::ModeOptionStruct::Type> selectModeOptions;

        map<unsigned char,PropertyValues> GET_properties; 
        map<unsigned char,PropertyValues> SET_properties;
        map<unsigned char,PropertyValues> INF_properties;
        vector<unsigned char> intervalRequestingGETproperties;
        map<pair<chip::ClusterId,chip::AttributeId>,AttributePropertyAdapter*> attributePropertyAdapters; 
        EchonetEndpointDelegate* delegate;

        std::chrono::_V2::system_clock::time_point lasttimeAlive;
        
        unsigned short currentWaitingTID =0xFFFF;
        unsigned char currentWaitingPropertyId = 0xFF;

        bool isStartupDone = false;
        bool isAddedToMatter = false;
    private:
        mutex GET_properties_mutex;
    public:
        EchonetEndpoint(){device= NULL;};
        virtual ~EchonetEndpoint(){};
        EchonetEndpoint(string address_,pair<string,unsigned int>& eoj_pair_);
        void CreateMatterDeviceEndpointOBJ();
        vector<unsigned char> GetGET(unsigned char epc)     {    return GET_properties[epc].echonetValue;    };  
        virtual EmberAfStatus SetGET(unsigned char epc, vector<unsigned char> value);
        EmberAfAttributeMetadata* GenerateBridgedDeviceBasicAttrs(EmberAfAttributeMetadata* onOffAttrs, EmberAfAttributeMetadata* descriptorAttrs, const unsigned int* onOffIncomingCommands);
        void RequestGETPropertyData_Asynchronous(unsigned char epc);
        void RequestGETPropertiesData_Asynchronous(); 
        virtual int ReadProperty(chip::ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength);
        virtual EmberAfStatus WriteProperty(chip::AttributeId attributeId, ClusterId clusterId, const EmberAfAttributeMetadata * attributeMetadata,uint8_t * buffer); 

        AttributePropertyAdapter* CreateDeviceEchonetAdapter(chip::ClusterId matterClusterId_,  chip::AttributeId matterAttributeId_,unsigned char echonetPropertyId_,       uint16_t matterDataType_ = ZAP_TYPE(INT8U),uint16_t matterDataLength_=1, uint16_t echonetDataType_ =ZAP_TYPE(INT8U), uint16_t echonetDataLength_=1);
        //Preset rules
        AttributePropertyAdapter* AddPresetEchonetAdapter_OnOff(unsigned char echoId=0x80);
        AttributePropertyAdapter* AddPresetEchonetAdapter_LevelControl(unsigned char echoId=0xB0,float scale=2.54f);
        AttributePropertyAdapter* AddPresetEchonetAdapter_ModeSelect(unsigned char echoId=0xE0,EchonetOptionType presetOptionTypes = EchonetOptionType::OPEN_CLOSE_STOP_41_42_43);
        AttributePropertyAdapter* AddPresetEchonetAdapter_ModeSelect_Description(const char* description);


        
        void InsertGETProperty(unsigned char,EchoProperty* );
        
        shared_ptr<DeviceObject> GetDeviceObject();
        shared_ptr<DeviceObject> GetDeviceObject(unsigned short echoClassCode_ ,unsigned char instanceCode_);
        void CalcEndpointType();
        bool HealthCheck();
        bool onSetProperty(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success);
        
        //JSON IO
        Json::Value ToJson();
        string GetName();
        string PropertiesInfoToString();
        string PropertiesPairsInfoToString();
        
};




#endif