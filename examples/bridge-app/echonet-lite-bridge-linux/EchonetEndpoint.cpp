#ifndef ECHONETENDPOINTINFO_C
#define ECHONETENDPOINTINFO_C
#include"EchonetEndpoint.h"
#include "include/Device.h"
#include <iostream>
//#include "Utils.h"
//#include "MatterMetaData.h"
#include "EchonetEndpointDefines.h"
#include <string>
#include <cassert>
#include "EchonetDevicesManager.h"

#include "include/delegates/EchonetEndpointDelegate_IHouseWindowCovering.h"
#include "include/delegates/EchonetEndpointDelegate_IHouseWindow.h"
#include "include/delegates/EchonetEndpointDelegate_IHouseSwitch.h"

#include <ctime>
#include <chrono>

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace std;
using namespace ModeSelect;
template <typename T>
using List               = app::DataModel::List<T>;

EchonetEndpoint::EchonetEndpoint(string address_,pair<string,unsigned int>& eoj_pair_)//, string& masterIpAddress, string& myIpAddress)
{
    this->address = address_;
    this->eoj_pair = eoj_pair_;
    echoClassCode = (unsigned short)(eoj_pair_.second>>8);
    instanceCode =  (unsigned char)(eoj_pair_.second%256);
    delegate = NULL;
    device= NULL;
    lasttimeAlive = std::chrono::system_clock::now();
}

// This is the core function to establish the mapping rules
void EchonetEndpoint::CreateMatterDeviceEndpointOBJ()
{
    AttributePropertyAdapter* apt;
    this->device = NULL;
    switch (this->type)
    {
    case  MatterEchonetLITECombineEndpointType::ONOFF_LIGHT:
        this->device = new DeviceEchonetAdapter(("LIGHT " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedLightEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedOnOffDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gLightDataVersions_);
        //intervalRequestingGETproperties.push_back(0x80);

        //intervalRequestingGETproperties.insert(intervalRequestingGETproperties.end(), { 0x80 });
        AddPresetEchonetAdapter_OnOff();
        break;
    case  MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH:
    
        //this->device = new DeviceEchonetAdapter("ONOFF LIGHT SWITCH","myroom");
        this->device = new DeviceEchonetAdapter(("ONOFF SWITCH " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedLightSwitchEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedOnOffSwitchDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gLightDataSwitchVersions_);
        AddPresetEchonetAdapter_OnOff();
        break;
    case MatterEchonetLITECombineEndpointType::DIMMABLE_LIGHT:
        this->device = new DeviceEchonetAdapter(("DIMMABLE LIGHT " + this->GetName()).c_str(),"myroom");
        
        this->emberAfEndpointType = &bridgedDimmableLightEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gDimmableBridgedOnOffDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gDimmableLightDataVersions_);
        AddPresetEchonetAdapter_OnOff();
        AddPresetEchonetAdapter_LevelControl();
        break;
    case  MatterEchonetLITECombineEndpointType::COLOR_TEMPERATURE_LIGHT:
    
        this->device = new DeviceEchonetAdapter(("COLOR TEMPERATURE LIGHT " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedTemperatureLightEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gColorTemperatureBridgedLightDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gColorTemperatureLightDataVersions_);
        AddPresetEchonetAdapter_OnOff();
        AddPresetEchonetAdapter_LevelControl();
        AddPresetEchonetAdapter_ModeSelect(0xB1, EchonetOptionType::LIGHT_COLOR_SETTING_LAMP_WHITE_DAYLIGHTWHITE_DAYLIGHCOLOR_OTHER_41_42_43_44_40);
        AddPresetEchonetAdapter_ModeSelect_Description("Change light mode");

        break;
    case MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR:
    case MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR_CO2: 
        this->device = new DeviceEchonetAdapter((type==MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR?"TEMPERATURE SENSOR "+ this->GetName():"CO2 SENSOR"+ this->GetName()).c_str(), "myroom");
        

        this->emberAfEndpointType = &bridgedTempSensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedTempSensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gTempSensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(TemperatureMeasurement::Id,TemperatureMeasurement::Attributes::MeasuredValue::Id,0xE0,
        ZAP_TYPE(INT16S), 2,ZAP_TYPE(INT16S) );

        if(this->type == MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR)
            apt->valueMultiplierForEchonetValue = 10;


        this->attributePropertyAdapters.insert({make_pair(TemperatureMeasurement::Id,apt->matterAttributeId),apt} );
        break;
    case MatterEchonetLITECombineEndpointType::ILLUMINANCE_SENSOR: 
        this->device = new  DeviceEchonetAdapter(("ILLUMINANCE SENSOR " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedIlluminanceSensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedIlluminanceSensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gIlluminanceSensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(IlluminanceMeasurement::Id,IlluminanceMeasurement::Attributes::MeasuredValue::Id,0xE0,
        ZAP_TYPE(INT16U), 2,ZAP_TYPE(INT16U ),2);
        this->attributePropertyAdapters.insert({make_pair(IlluminanceMeasurement::Id,apt->matterAttributeId),apt} );

        break;


    case MatterEchonetLITECombineEndpointType::HUMIDITY_SENSOR: 
        this->device = new  DeviceEchonetAdapter(("HUMIDITY SENSOR " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedHumiditySensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedHumiditySensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gHumiditySensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(RelativeHumidityMeasurement::Id,RelativeHumidityMeasurement::Attributes::MeasuredValue::Id,0xE0,
        ZAP_TYPE(INT8U), 1,ZAP_TYPE(INT8U ));
        apt->valueMultiplierForEchonetValue = 10;
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );
        break;
   
    case MatterEchonetLITECombineEndpointType::FLOW_SENSOR: 
        this->device = new  DeviceEchonetAdapter(("FLOW SENSOR " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedFlowSensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedFlowSensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gFlowSensorDataVersions_);

        //toanstt: quick fix 
        apt = CreateDeviceEchonetAdapter(FlowMeasurement::Id,FlowMeasurement::Attributes::MeasuredValue::Id,0xE2,
        ZAP_TYPE(INT16U), 2,ZAP_TYPE(INT32U ), 4);
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );
        break;

    case MatterEchonetLITECombineEndpointType::FLOW_SENSOR_for_ELECTRIC_ENERGY_SENSOR: 
        this->device = new  DeviceEchonetAdapter(("ELECTRIC ENERGY SENSOR " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedFlowSensorElectricEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedFlowElectricSensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gFlowSensorElectricDataVersions_);

        apt = CreateDeviceEchonetAdapter(FlowMeasurement::Id,FlowMeasurement::Attributes::MeasuredValue::Id,0xE0,
        ZAP_TYPE(INT16U), 2,ZAP_TYPE(INT32U ), 4);
        apt->valueMultiplierForEchonetValue = 0.001f;
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );

        break;

    case MatterEchonetLITECombineEndpointType::PRESSURE_SENSOR_for_STORAGE_BATTERY: 
        this->device = new  DeviceEchonetAdapter(("PRESSURE SENSOR bat " + this->GetName()).c_str(),"myroom");

        this->emberAfEndpointType = &bridgedPressureBatterySensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedPressureBatterySensorDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gPressureBatterySensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(PressureMeasurement::Id,PressureMeasurement::Attributes::MeasuredValue::Id,0xA2,
        ZAP_TYPE(INT16S), 2,ZAP_TYPE(INT32U ), 4);
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );

        break;

    case MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR:
        this->device = new  DeviceEchonetAdapter (("OCCUPANCY SENSOR "+ this->GetName()).c_str(),"myroom");

        this->emberAfEndpointType = &bridgedOccupancySensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedOccupancyDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gOccupancySensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(OccupancySensing::Id,OccupancySensing::Attributes::Occupancy::Id,0xB1,
        ZAP_TYPE(BITMAP8), 1, ZAP_TYPE(ENUM8),1 );

        apt->AddPairOfmapValue({0x41},{1}); 
        apt->AddPairOfmapValue({0x42},{0}); 


        this->attributePropertyAdapters.insert({make_pair(OccupancySensing::Id,apt->matterAttributeId),apt} );

        break;

    case MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR_EMGERGENCY_BUTTON:
        this->device = new  DeviceEchonetAdapter (("EMGERGENCY BUTTON occusensor"+ this->GetName()).c_str(),"myroom");

        this->emberAfEndpointType = &bridgedOccupancySensorEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedOccupancyDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gOccupancySensorDataVersions_);

        apt = CreateDeviceEchonetAdapter(OccupancySensing::Id,OccupancySensing::Attributes::Occupancy::Id,0xB1,
        ZAP_TYPE(BITMAP8), 1, ZAP_TYPE(ENUM8) );
        this->attributePropertyAdapters.insert({make_pair(OccupancySensing::Id,apt->matterAttributeId),apt} );

        break;
    case MatterEchonetLITECombineEndpointType::WINDOW_COVERING:
        this->device = new  DeviceWindowCovering (("WINDOW COVERING " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &windowCoveringEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedWindowCoveringDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gwindowCoveringDataVersions_);


        AddPresetEchonetAdapter_ModeSelect_Description("WINDOW COVERING mode select");
        AddPresetEchonetAdapter_ModeSelect();

        apt = CreateDeviceEchonetAdapter(WindowCovering::Id,WindowCovering::Attributes::CurrentPositionLift::Id ,0xE0, 
        ZAP_TYPE(INT8U), 1, ZAP_TYPE(INT8U));
        apt->AddPairOfmapValue({0x41},{100}); 
        apt->AddPairOfmapValue({0x42},{0}); 
        apt->AddPairOfmapValue({0x43},{101}); 
        this->attributePropertyAdapters.insert({make_pair(WindowCovering::Id,apt->matterAttributeId),apt} );
        break;
    case MatterEchonetLITECombineEndpointType::WINDOW_COVERING_IHOUSE:
        this->device = new  DeviceWindowCovering (("WINDOW COVERING i " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &windowCoveringEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedWindowCoveringDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gwindowCoveringDataVersions_);


        AddPresetEchonetAdapter_ModeSelect_Description("WINDOW COVERING mode select");
        AddPresetEchonetAdapter_ModeSelect();

        apt = CreateDeviceEchonetAdapter(WindowCovering::Id,WindowCovering::Attributes::CurrentPositionLift::Id ,0xE0, 
        ZAP_TYPE(INT8U), 1, ZAP_TYPE(INT8U));
        apt->AddPairOfmapValue({0x41},{100}); 
        apt->AddPairOfmapValue({0x42},{0}); 
        apt->AddPairOfmapValue({0x43},{101}); 
        this->attributePropertyAdapters.insert({make_pair(WindowCovering::Id,apt->matterAttributeId),apt} );

        this->delegate = new EchonetEndpointDelegate_IHouseWindowCovering(this);

        break;


    case MatterEchonetLITECombineEndpointType::WINDOW:
    case MatterEchonetLITECombineEndpointType::MODESELECT:
        //test
        this->device = new  DeviceEchonetAdapter (("GENERIC WINDOW " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedGenericEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgeGenericCurtainDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gGenericDataVersions_);

        
        AddPresetEchonetAdapter_ModeSelect_Description("WINDOW COVERING mode select");
        AddPresetEchonetAdapter_ModeSelect();
        break;
    case MatterEchonetLITECombineEndpointType::MODESELECT_IHOUSE:
        //test
        this->device = new  DeviceEchonetAdapter (("GENERIC WINDOW i " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedGenericEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgeGenericCurtainDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gGenericDataVersions_);

        
        AddPresetEchonetAdapter_ModeSelect_Description("WINDOW COVERING modeselect i ");
        AddPresetEchonetAdapter_ModeSelect();

        this->delegate = new EchonetEndpointDelegate_IHouseWindow(this);

        break;

    case MatterEchonetLITECombineEndpointType::HEATING_COOLING_UNIT:
        this->device = new  DeviceEchonetAdapter (("HEATING COOLING UNIT " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &heatingColingEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gHeatingCoolingDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(heatingColingDataVersions_);

        AddPresetEchonetAdapter_OnOff();
        apt = CreateDeviceEchonetAdapter(Thermostat::Id,Thermostat::Attributes::SystemMode::Id ,0xB0, 
        ZAP_TYPE(INT8U), 1, ZAP_TYPE(INT8U));
        apt->AddPairOfmapValue({0x40},{9}); //Other -> Sleep
        apt->AddPairOfmapValue({0x41},{1}); //auto
        apt->AddPairOfmapValue({0x42},{3}); //cooling
        apt->AddPairOfmapValue({0x43},{4}); //heating
        apt->AddPairOfmapValue({0x44},{8}); //Dehumidification == Dry
        apt->AddPairOfmapValue({0x45},{7}); //Air circulation == Fan only

        apt->mapValueMatter2Echonet.insert({{0},{0x40}}); // Off -> Other
        apt->mapValueMatter2Echonet.insert({{5},{0x40}}); // EmergencyHeat -> Other
        apt->mapValueMatter2Echonet.insert({{6},{0x40}}); // Precooling -> Other 
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );


        AddPresetEchonetAdapter_ModeSelect(0x8F,EchonetOptionType::POWER_SAVING_OPERATION_SETTING_41_42 );


        //Measured value of roomtemperature <==> LocalTemperature (Matter)
        apt = CreateDeviceEchonetAdapter(Thermostat::Id,Thermostat::Attributes::LocalTemperature::Id  ,0xBB,
        ZAP_TYPE(INT16S), 2, ZAP_TYPE(INT8S),1);
        apt->valueMultiplierForEchonetValue = 100.0f; 
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );


        //Measured outdoor air temperature <==> OutdoorTemperature (Matter)
        apt = CreateDeviceEchonetAdapter(Thermostat::Id,Thermostat::Attributes::OutdoorTemperature::Id  ,0xBE,
        ZAP_TYPE(INT16S), 2, ZAP_TYPE(INT8S),1);
        apt->valueMultiplierForEchonetValue = 100.0f; 
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );



        //Set temperature value 0xB3 <==> LevelControl
        AddPresetEchonetAdapter_LevelControl(0xB3, 1);

        //Fan control
        apt = CreateDeviceEchonetAdapter(FanControl::Id,FanControl::Attributes::FanMode::Id  ,0xA0,
        ZAP_TYPE(ENUM8), 1, ZAP_TYPE(ENUM8),1);
        apt->AddPairOfmapValue({0x41},{5});
        apt->AddPairOfmapValue({0x31},{0});
        apt->AddPairOfmapValue({0x32},{1});
        apt->AddPairOfmapValue({0x35},{2});
        apt->AddPairOfmapValue({0x38},{3});
        apt->AddPairOfmapValue({0x35},{4});
        apt->AddPairOfmapValue({0x41},{6});
        
        apt->mapValueEchonet2Matter.insert({{0x33},{2}});
        apt->mapValueEchonet2Matter.insert({{0x34},{2}});
        apt->mapValueEchonet2Matter.insert({{0x36},{2}});
        apt->mapValueEchonet2Matter.insert({{0x37},{2}});

        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );


        //Fan contorl percent setting
        apt = CreateDeviceEchonetAdapter(FanControl::Id,FanControl::Attributes::PercentSetting::Id  ,0xEE,
        ZAP_TYPE(ENUM8), 1, ZAP_TYPE(ENUM8),1);
 
        apt->defaultValue = (uint8_t *)malloc(1);
        apt->defaultValue[0] = 1;
        this->attributePropertyAdapters.insert({make_pair(apt->matterClusterId,apt->matterAttributeId),apt} );




        break;

    case  MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH_IHOUSE:
        this->device = new DeviceEchonetAdapter(("ONOFF SWITCH i " + this->GetName()).c_str(),"myroom");
        this->emberAfEndpointType = &bridgedLightSwitchEndpoint_;
        this->deviceTypeList = Span<const EmberAfDeviceType>(gBridgedOnOffSwitchDeviceTypes_);
        this->dataVersionStorage = Span<DataVersion>(gLightDataSwitchVersions_);
        //AddPresetEchonetAdapter_OnOff();

        this->delegate = new EchonetEndpointDelegate_IHouseSwitch(this);

        break;

    default:
            this->device = new DeviceEchonetAdapter(("UNKNOW DEVICE " + this->GetName()).c_str(),"myroom");
        break;
    }
    this->device->SetReachable(true);
    this->device->echonetEndpointInfoPointer = (void*) this;
}



const int kNodeLabelSize = 32;
EmberAfAttributeMetadata* EchonetEndpoint::GenerateBridgedDeviceBasicAttrs(EmberAfAttributeMetadata* onOffAttrs, EmberAfAttributeMetadata* descriptorAttrs, const unsigned int* onOffIncomingCommands)
{
    return NULL;
}

 EmberAfStatus EchonetEndpoint::SetGET(unsigned char epc, vector<unsigned char> value)
 {
    if(delegate!=NULL) return delegate->SetGET(epc,value);

    EmberAfStatus errorCode = EMBER_ZCL_STATUS_SUCCESS;
    //value= {0x30};
    
    EchoFrame eFrame= GetDeviceObject().get()->set(true).reqSetProperty(epc,value).send();

    printf("____ EchonetEndpoint::SetGET epc=0x%02x  value=%s tid=%d\n",epc, ConvertEchonetValueToHexString(value).c_str(), eFrame.getTID());
    //test only
    currentWaitingTID = eFrame.getTID();
    TimeManager::GetInstance()->RecordTime(TimeRecordType::START_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE, echoClassCode,instanceCode,epc ,eoj_pair.second>>8,eoj_pair.second%256, ConvertToUnsignedInt(value) );

    if (STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE== false)
    {
        for(int i =0; i < 500; i++)
        {
            if(currentWaitingTID  == 0xFFFF) 
            {
                TimeManager::GetInstance()->RecordTime(TimeRecordType::END_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE, echoClassCode,instanceCode,epc ,eoj_pair.second>>8,eoj_pair.second%256, ConvertToUnsignedInt(value) );
                break;
            }
            usleep(10 * 1000); //sleep 10 miliseconds
        }
        if(currentWaitingTID != 0xFFFF) errorCode = EMBER_ZCL_STATUS_DEPRECATED83; //for test only
    }
    
    GET_properties[epc].echonetValue = value;
    return errorCode;
 }
void EchonetEndpoint::RequestGETPropertiesData_Asynchronous()
{
    //RequestGETPropertyData_Asynchronous(0x80);

    auto setter = GetDeviceObject()->get();
    //map<unsigned char,PropertyValues>::iterator it;
    //for (it = SET_properties.begin(); it != SET_properties.end(); it++)
    
    // {
    //     printf("bb %d \n", it->first);
    //     setter.reqGetProperty(it->first);
    // }
    for(vector<unsigned char>::iterator iter = intervalRequestingGETproperties.begin(); iter < intervalRequestingGETproperties.end(); iter++)
    {
        setter.reqGetProperty(*iter);
    }
    setter.send();
    
}
 void EchonetEndpoint::RequestGETPropertyData_Asynchronous(unsigned char epc)
 {
    GetDeviceObject()->get().reqGetProperty(epc).send();


 }
 int EchonetEndpoint::ReadProperty(chip::ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength)
 {
    if(delegate!=NULL) return delegate->ReadProperty(clusterId,attributeId, buffer,maxReadLength );

    //Check (clusterId,attributeId) is available or not
    if(this->attributePropertyAdapters.find(make_pair(clusterId,attributeId)) == this->attributePropertyAdapters.end())
    {
        printf("\n\n[WARNING ReadProperty] toanstt: key not existed in the map clusterId=0x%02x attributeId=0x%06x\n\n",(int)clusterId, (int)attributeId);
        return 0;
    }
    AttributePropertyAdapter* apt = this->attributePropertyAdapters[make_pair(clusterId,attributeId)];
    

    //Return constant value if any
    if(this->GET_properties.find(apt->echonetPropertyId) == this->GET_properties.end())
    {
        if(apt->defaultValue!=NULL)
        {
            if(apt->matterDataType == ZAP_TYPE(CHAR_STRING))
            {
                MutableByteSpan zclNameSpan(buffer, maxReadLength);
                MakeZclCharString(zclNameSpan, (char*)apt->defaultValue);
            } else
            {
                if(maxReadLength>1) printf("\n[TOANSTT TODO HERE: Extend this case \n");
                //*((unsigned char*)buffer) = (unsigned char)(apt->defaultValue[0]);
            }
            return 0;
        } else printf("\n\n[WARNING ReadProperty] apt->defaultValue is NULL  clusterId=0x%02x attributeId=0x%02x echonetPropertyId=0x%02x\n\n",clusterId,attributeId,apt->echonetPropertyId);
        return 0;
    }


    //Try to get new value from echonetLITE device
    //This step can be skip to have better speed. however, be carefull when some devices may change their value without any notification.
    currentWaitingPropertyId = apt->echonetPropertyId;
    GetDeviceObject()->get().reqGetProperty(currentWaitingPropertyId).send();
    TimeManager::GetInstance()->RecordTime(TimeRecordType::START_SEND_READ_COMMAND_TO_ECHONET_DEVICE, echoClassCode, instanceCode, apt->echonetPropertyId,(unsigned short)clusterId, attributeId,(unsigned int)buffer[0]);
    if (STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ== false)
    {
        for(int i =0; i < 500; i++)
        {
            if(i%50==0)
            printf("[ReadProperty] Waiting for response (sleeping 50ms) PropertyId=0x%02x \n",currentWaitingPropertyId );
            if(currentWaitingPropertyId  == 0xFF) 
            {
                TimeManager::GetInstance()->RecordTime(TimeRecordType::END_SEND_READ_COMMAND_TO_ECHONET_DEVICE, echoClassCode,instanceCode,currentWaitingPropertyId ,eoj_pair.second>>8,eoj_pair.second%256, ConvertToUnsignedInt(this->GET_properties[apt->echonetPropertyId].echonetValue) );
                break;
            }
            usleep(10 * 1000); //sleep 10 miliseconds
        }
    }

    
    vector<unsigned char> echonetValue = this->GET_properties[apt->echonetPropertyId].echonetValue;
    unsigned int val;
    short val_shorts;
    unsigned long val_longu;
    int val_int;
    unsigned int val_intu;
    unsigned char val_charu;
    char val_char;
    //check dictionary
    if(apt->mapValueEchonet2Matter.find(echonetValue)!=apt->mapValueEchonet2Matter.end())
    {
        if(apt->matterDataType == ZAP_TYPE(BOOLEAN))
            *buffer = (uint8_t)ConvertToUnsignedChar(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(INT8U))
        {
            

            *((uint8_t*)buffer) = (uint8_t)ConvertToUnsignedChar(apt->mapValueEchonet2Matter[echonetValue]);
        }
        else if(apt->matterDataType == ZAP_TYPE(INT16U))
            *((unsigned short*)buffer) = ConvertToUnsignedShort(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(INT16S))
            *((short*)buffer) = ConvertToShort(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(INT8S))
            *((int8_t*)buffer) = (int8_t)ConvertToChar(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(INT32U))
            *((unsigned int*)buffer) = ConvertToUnsignedInt(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(INT32S))
            *((int*)buffer) = ConvertToInt(apt->mapValueEchonet2Matter[echonetValue]);
        else if(apt->matterDataType == ZAP_TYPE(ENUM8))
            *((unsigned char*)buffer) = ConvertToUnsignedChar(apt->mapValueEchonet2Matter[echonetValue]);
        else printf("\n\n[ERROR 1] toanstt's error with matterDataLength=%d \n\n", (int)apt->matterDataLength);


        return 0;
    }

    //Convert values
    switch (apt->echonetDataType)
    {
    case ZAP_TYPE(ENUM8):
        val = ConvertToInt(echonetValue);
        memcpy(buffer, &val, apt->matterDataLength);
        return 0;
    case ZAP_TYPE(INT16S):
        if(apt->matterDataType == ZAP_TYPE(INT8U) || apt->matterDataType == ZAP_TYPE(INT16U) ||apt->matterDataType == ZAP_TYPE(INT32U))  printf("\n\n[ERROR] Expected to handle signed number here \n\n");

        val_shorts = (short)ConvertToInt(echonetValue);
        if(apt->matterDataLength==4)
            *((int*)buffer) = val_shorts;
        else if(apt->matterDataLength==2)
            *((short*)buffer) = val_shorts;
        else if(apt->matterDataLength==1)
            *buffer = (int8_t)val_shorts;
        else printf("\n\n[ERROR 2] toanstt's error with matterDataLength=%d \n\n", (int)apt->matterDataLength);
        return 0;
    case ZAP_TYPE(INT16U):

        
        if(apt->matterDataType == ZAP_TYPE(INT8S) || apt->matterDataType == ZAP_TYPE(INT16S) ||apt->matterDataType == ZAP_TYPE(INT32S))  printf("\n\n[ERROR] Expected to handle unsigned number here \n\n");
        
        val_longu = ConvertToShort(echonetValue);
        if(apt->valueMultiplierForEchonetValue!=0) val_longu = (unsigned long)((double)val_longu*(double)apt->valueMultiplierForEchonetValue);


        printf("\n\n\n [INT16U] test size=%d apt->matterDataLength=%d\n\n\n",(int)echonetValue.size(),apt->matterDataLength );
        
        *((unsigned long*)buffer) = (unsigned long)val_longu;

        return 0;
    case ZAP_TYPE(INT32U):
        
        if(apt->matterDataType == ZAP_TYPE(INT8S) || apt->matterDataType == ZAP_TYPE(INT16S) ||apt->matterDataType == ZAP_TYPE(INT32S))  printf("\n\n[ERROR] Expected to handle unsigned number here \n\n");
        
        val_longu = ConvertToUnsignedInt(echonetValue);
        if(apt->valueMultiplierForEchonetValue!=0) val_longu = (unsigned long)((double)val_longu*(double)apt->valueMultiplierForEchonetValue);


        printf("\n\n\n [INT32U] test size=%d apt->matterDataLength=%d origial=%d originalc=%lu new=%lu Multiplier=%f\n\n\n",(int)echonetValue.size(),apt->matterDataLength,
            ConvertToUnsignedInt(echonetValue),
            (unsigned long)ConvertToUnsignedInt(echonetValue),
            val_longu,
            apt->valueMultiplierForEchonetValue
         );



        *((unsigned long*)buffer) = (unsigned long)val_longu;
        return 0;

    case ZAP_TYPE(INT8U):
        if(apt->matterDataType == ZAP_TYPE(INT8S) || apt->matterDataType == ZAP_TYPE(INT16S) ||apt->matterDataType == ZAP_TYPE(INT32S))  printf("\n\n[ERROR 572] Expected to handle unsigned number here \n\n");
        val_intu=val_charu =echonetValue.size()>0?echonetValue[0]:0;
        

        if(apt->valueMultiplierForEchonetValue!=0) val_intu = (unsigned int)(val_charu*apt->valueMultiplierForEchonetValue);


        if(apt->matterDataLength==4)
            *((unsigned int*)buffer) = val_intu;
        else if(apt->matterDataLength==2)
            *((unsigned short*)buffer) = (unsigned short)val_intu;
        else if(apt->matterDataLength==1)
            *buffer = (uint8_t)val_intu;
        else printf("\n\n[ERROR 4] toanstt's error with matterDataLength=%d \n\n", (int)apt->matterDataLength);

        return 0;
    case ZAP_TYPE(INT8S):
        if(apt->matterDataType == ZAP_TYPE(INT8U) || apt->matterDataType == ZAP_TYPE(INT16U) ||apt->matterDataType == ZAP_TYPE(INT32U))  printf("\n\n[ERROR 572] Expected to handle signed number here \n\n");
        val_int=val_char =echonetValue.size()>0?echonetValue[0]:0;

        if(apt->valueMultiplierForEchonetValue!=0) val_int = (int)(val_char*apt->valueMultiplierForEchonetValue);

        if(apt->matterDataLength==4)
            *((int*)buffer) = (int)val_int;
        else if(apt->matterDataLength==2)
            *((short*)buffer) = (short)val_int;
        else if(apt->matterDataLength==1)
            *buffer = (uint8_t)val_int;
        else printf("\n\n[ERROR 4] toanstt's error with matterDataLength=%d \n\n", (int)apt->matterDataLength);

        return 0;
    default:
        printf("\n\n[WARNING] toanstt not implemented this case: echonetDataType=%d \n\n", (int)apt->echonetDataType);
        break;
    }
    string s = "dddddd";
    memcpy(buffer, s.c_str(), s.size());


    
    return 0;
 }


 EmberAfStatus EchonetEndpoint::WriteProperty(chip::AttributeId attributeId, ClusterId clusterId, const EmberAfAttributeMetadata * attributeMetadata,uint8_t * buffer)
 {
    //Call WriteProperty of warper if exists
   if(delegate!=NULL) return delegate->WriteProperty(attributeId, clusterId, attributeMetadata, buffer);

   //Check (clusterId,attributeId) is available or not
    if(this->attributePropertyAdapters.find(make_pair(clusterId,attributeId)) == this->attributePropertyAdapters.end())
    {
        printf("\n\n[WARNING WriteProperty] toanstt: key not existed in the map %d \n\n", (int)attributeId);
        return EMBER_ZCL_STATUS_DEPRECATED82;
    }

    
    AttributePropertyAdapter* apt = this->attributePropertyAdapters[make_pair(clusterId,attributeId)];
    //Check the coresponding SET property is available or not
    if(this->SET_properties.find(apt->echonetPropertyId) == this->SET_properties.end())
    {
        printf("\n\n[WARNING WriteProperty] toanstt: key not existed in SET_properties %d \n\n", (int)attributeId);
        return EMBER_ZCL_STATUS_DEPRECATED82;
    }
    
    //Check dictionary
    vector<unsigned char> newData = {0x00};
    if(apt->mapValueMatter2Echonet.find({*buffer}) != apt->mapValueMatter2Echonet.end())
    {
        newData = apt->mapValueMatter2Echonet[{*buffer}];
        
        return SetGET(apt->echonetPropertyId,newData );
    }


    //Convert data
    if(apt->matterDataType == ZAP_TYPE(INT8U)) 
    {
        uint8_t val_uint8_t = *buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_uint8_t = (uint8_t)(val_uint8_t*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_uint8_t)}; 
    }
    else if(apt->matterDataType == ZAP_TYPE(INT16U)) 
    {
        unsigned short val_unsignedshort = (unsigned short)*buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_unsignedshort = (unsigned short)(val_unsignedshort*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_unsignedshort)};
    }
    else if(apt->matterDataType == ZAP_TYPE(INT32U)) 
    {
        unsigned int val_unsignedint= (unsigned int)*buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_unsignedint = (unsigned int)((float)val_unsignedint*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_unsignedint)};
    }
    else if(apt->matterDataType == ZAP_TYPE(INT8S)) 
    {
        char val_int8_t = *buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_int8_t = (uint8_t)(val_int8_t*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_int8_t)}; 
    }
    else if(apt->matterDataType == ZAP_TYPE(INT16S)) 
    {
        short val_short = (short)*buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_short = (short)(val_short*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_short)};
    }
    else if(apt->matterDataType == ZAP_TYPE(INT32S)) 
    {
        int val_int= (int)*buffer;
        if(apt->valueMultiplierForEchonetValue!=0) val_int = (int)((float)val_int*1.0f/apt->valueMultiplierForEchonetValue);
        newData = {(unsigned char)(val_int)};
    } 
    
    SetGET(apt->echonetPropertyId,newData );
    return EMBER_ZCL_STATUS_SUCCESS;
 }

AttributePropertyAdapter* EchonetEndpoint::CreateDeviceEchonetAdapter(
    chip::ClusterId matterClusterId_,
  chip::AttributeId matterAttributeId_,
  unsigned char echonetPropertyId_,       
  uint16_t matterDataType_ ,
  uint16_t matterDataLength_, 
  uint16_t echonetDataType_ , 
  uint16_t echonetDataLength_)
{
    AttributePropertyAdapter* apt = new AttributePropertyAdapter(matterClusterId_,matterAttributeId_,echonetPropertyId_,matterDataType_,matterDataLength_,echonetDataType_,echonetDataLength_);
    intervalRequestingGETproperties.push_back(echonetPropertyId_);
    return apt;
}
Structs::ModeOptionStruct::Type toanBuildModeOptionStruct(const char * label, uint8_t mode,
                                                      const List<const Structs::SemanticTagStruct::Type> & semanticTags)
{
    Structs::ModeOptionStruct::Type option;
    option.label        = CharSpan::fromCharString(label);
    option.mode         = mode;
    option.semanticTags = semanticTags;
    return option;
}

AttributePropertyAdapter* EchonetEndpoint::AddPresetEchonetAdapter_OnOff(unsigned char echoId)
{
    
    AttributePropertyAdapter*apt = CreateDeviceEchonetAdapter(OnOff::Id,OnOff::Attributes::OnOff::Id,echoId,
    ZAP_TYPE(BOOLEAN), 1,ZAP_TYPE(INT8U) );
    apt->mapValueEchonet2Matter.insert({{0x30},{1}});
    apt->mapValueEchonet2Matter.insert({{0x31},{0}});
    apt->mapValueMatter2Echonet.insert({{1},{0x30}});
    apt->mapValueMatter2Echonet.insert({{0},{0x31}});
    this->attributePropertyAdapters.insert({make_pair(OnOff::Id,apt->matterAttributeId),apt} );

    //intervalRequestingGETproperties.push_back(echoId);
    return apt;
}
AttributePropertyAdapter* EchonetEndpoint::AddPresetEchonetAdapter_LevelControl(unsigned char echoId,float scale)
{
    AttributePropertyAdapter*apt = CreateDeviceEchonetAdapter(LevelControl::Id,LevelControl::Attributes::CurrentLevel::Id,echoId,
    ZAP_TYPE(INT8U), 1,ZAP_TYPE(INT8U ),1);
    apt->valueMultiplierForEchonetValue =scale;
    this->attributePropertyAdapters.insert({make_pair(LevelControl::Id,apt->matterAttributeId),apt} );

    //intervalRequestingGETproperties.push_back(echoId);
    return apt;
}
AttributePropertyAdapter* EchonetEndpoint::AddPresetEchonetAdapter_ModeSelect(unsigned char echoId,EchonetOptionType presetOptionTypes)
{
    AttributePropertyAdapter*apt = CreateDeviceEchonetAdapter(ModeSelect::Id,ModeSelect::Attributes::CurrentMode::Id,echoId,
    ZAP_TYPE(INT8U), 1,ZAP_TYPE(ENUM8) );
    this->attributePropertyAdapters.insert({make_pair(ModeSelect::Id,apt->matterAttributeId),apt} );
    this->echonetOptionType = presetOptionTypes;

    //intervalRequestingGETproperties.push_back(echoId);
    return apt;
}
AttributePropertyAdapter* EchonetEndpoint::AddPresetEchonetAdapter_ModeSelect_Description(const char* description)
{
    AttributePropertyAdapter*apt = CreateDeviceEchonetAdapter(ModeSelect::Id, ModeSelect::Attributes::Description::Id,0xFF);
    apt ->SetDefaultValue(description);
    this->attributePropertyAdapters.insert({make_pair(ModeSelect::Id,apt->matterAttributeId),apt} );
    return apt;
}

void EchonetEndpoint::InsertGETProperty(unsigned char c,EchoProperty* ep)
{

    lasttimeAlive = std::chrono::system_clock::now();
    unsigned int clusterId =0;
    unsigned int attributeId =0;
    map<pair<chip::ClusterId,chip::AttributeId>,AttributePropertyAdapter*>::iterator it;
    for (it = attributePropertyAdapters.begin(); it != attributePropertyAdapters.end(); it++)
    {
        if(it->second->echonetPropertyId == c)
        {
            clusterId = it->first.first;
            attributeId = it->first.second;
        }
    }

    TimeManager::GetInstance()->RecordTime(TimeRecordType::RECEIVE_ECHONET_PROPERTY_VALUE, echoClassCode,instanceCode, ep->epc,clusterId,attributeId, ConvertToUnsignedInt(ep->edt) );
    if(GET_properties.find(c) == GET_properties.end())
    {
        
        GET_properties.insert({c,{ep->edt,""}});   
    }
    else 
    {
        //printf("\nhere 2\n");
        GET_properties[c].echonetValue = ep->edt;
    }
    currentWaitingPropertyId = 0xFF;
    if(isStartupDone==false)
    {
        isStartupDone = true;
        EchonetDevicesManager::GetInstance()->CheckStartupDone();
    }
}

shared_ptr<DeviceObject> EchonetEndpoint::GetDeviceObject()
{
    return std::static_pointer_cast<DeviceObject>(Echo::getNode( address ).get()->getInstance(echoClassCode , instanceCode));
}
shared_ptr<DeviceObject> EchonetEndpoint::GetDeviceObject(unsigned short echoClassCode_ ,unsigned char instanceCode_)
{
    shared_ptr<EchoNode> node = Echo::getNode( address );
    if(node == nullptr) 
    {
        printf("\n\n\n\n [ERROR] Cannot GetDeviceObject: No node with ip = \"%s\"\n\n\n\n\n", address.c_str());
        return nullptr;
    }
    shared_ptr<DeviceObject> deviceObject = std::static_pointer_cast<DeviceObject>(node.get()->getInstance(echoClassCode_ , instanceCode_));
    if(deviceObject == nullptr) 
    {
        printf("\n\n\n\n [ERROR] Cannot GetDeviceObject: No instance on ip = \"%s\" with  class=0x%04x-%02x\n\n\n\n\n", address.c_str(),echoClassCode_,instanceCode_);
        return nullptr;
    }
    return deviceObject;
}
void EchonetEndpoint::CalcEndpointType()
{
    this->type = GetMatterEndpointTypeFromEchonetEndpointCode(this);
    CreateMatterDeviceEndpointOBJ();
}
bool EchonetEndpoint::onSetProperty(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success)
{
    if(tid == currentWaitingTID) currentWaitingTID = 0xFFFF;
    return success;
}
bool EchonetEndpoint::HealthCheck()
{
    //GetDeviceObjecpt()
    return true;
}



/*
-----------------------------------------------------------------------------
-------------------Write all mapping rules to JSON files---------------------
-----------------------------------------------------------------------------
*/


Json::Value EchonetEndpoint::ToJson()
{
    Json::Value json;
    json["eojGroupCode"] = ConvertUnsignedValueToHexString((unsigned char)(echoClassCode>>8));
    json["eojClassCode"] = ConvertUnsignedValueToHexString((unsigned char)(echoClassCode&0x00FF));
    
    json["eojString"] = ConvertUnsignedValueToHexString(echoClassCode) +ConvertUnsignedValueToHexString(instanceCode,false) ;
    json["matterDeviceName"] = GetMatterEndpointTypeName(type);
    json["matterDeviceId"] = ConvertUnsignedValueToHexString((unsigned short)type);
    json["echonetDeviceName"] = GetEchonetEndpointTypeName(echoClassCode);

    Json::Value listAdapters;
    map<pair<chip::ClusterId,chip::AttributeId>,AttributePropertyAdapter*>::iterator it;
    int i=0;
    for (it = attributePropertyAdapters.begin(); it != attributePropertyAdapters.end(); it++)
    {
        listAdapters[i++] = it->second->ToJson();
    }
    json["MatchingRules"] = listAdapters;
    return json;
}

string replace_first(std::string const& s,std::string const& toReplace,std::string const& replaceWith) 
{
    string s2 = s;
    while(true)
    {
        std::size_t pos = s2.find(toReplace);
        if (pos == std::string::npos) break;
        s2.replace(pos, toReplace.length(), replaceWith);
    }
    return s2;
}
string EchonetEndpoint::GetName()
{
    std::stringstream stream;
    stream << replace_first(address,".","") << " ";
    stream << ConvertUnsignedValueToHexString(echoClassCode);// << " ";
    stream << ConvertUnsignedValueToHexString(instanceCode,false);// << " " << instanceCode;
    std::string result( stream.str() );
    string s = stream.str();
  return s;
}
string EchonetEndpoint::PropertiesInfoToString()
{
    std::stringstream stream;
    map<unsigned char,PropertyValues>::iterator it;
    stream << "\n\t GET: ";
    char t[6];
    for (it = GET_properties.begin(); it != GET_properties.end(); it++)
    {
        sprintf(t,"%02x ", it->first);
        stream << t;
    }
            //stream << "0x" << std::hex << (unsigned char)it->first;
    stream << "\n\t SET: ";
    for (it = SET_properties.begin(); it != SET_properties.end(); it++)
    {
        sprintf(t,"%02x ", it->first);
        stream << t;
    }
    stream << "\n\t INF: ";
    for (it = INF_properties.begin(); it != INF_properties.end(); it++)
    {
        sprintf(t,"%02x ", it->first);
        stream << t;
    }
    stream << "\n";
  return stream.str();
}
string EchonetEndpoint::PropertiesPairsInfoToString()
{
    std::stringstream stream;
    map<unsigned char,PropertyValues>::iterator it;
    stream << "\n GET: ";
    char t[6];
    for (it = GET_properties.begin(); it != GET_properties.end(); it++)
    {
        sprintf(t,"%02x:", it->first);
        stream << t;
        stream << ConvertEchonetValueToHexString(it->second.echonetValue);
        stream << " ";
    }
    stream << "\n SET: ";
    for (it = SET_properties.begin(); it != SET_properties.end(); it++)
    {
        sprintf(t,"%02x:", it->first);
        stream << t;
    }
    stream << "\n INF: ";
    for (it = INF_properties.begin(); it != INF_properties.end(); it++)
    {
        sprintf(t,"%02x:", it->first);
        stream << t;
    }
    stream << "\n";
  return stream.str();
}
#endif
