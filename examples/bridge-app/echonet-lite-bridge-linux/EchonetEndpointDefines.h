 /*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This file includes static configs to describe Matter endpoints
 * These configs are based from orifginal example project
 */

#pragma once
#ifndef MATTERMETADATA_ECHONET_H
#define MATTERMETADATA_ECHONET_H


#include <AppMain.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/EventLogging.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/reporting/reporting.h>
#include <app/util/af-types.h>
#include <app/util/af.h>
#include <app/util/attribute-storage.h>
#include <app/util/util.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/ZclString.h>
#include <platform/CommissionableDataProvider.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>

#if CHIP_DEVICE_LAYER_TARGET_DARWIN
#include <platform/Darwin/NetworkCommissioningDriver.h>
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
#include <platform/Darwin/WiFi/NetworkCommissioningWiFiDriver.h>
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI
#endif // CHIP_DEVICE_LAYER_TARGET_DARWIN

#if CHIP_DEVICE_LAYER_TARGET_LINUX
#include <platform/Linux/NetworkCommissioningDriver.h>
#endif // CHIP_DEVICE_LAYER_TARGET_LINUX

#include <pthread.h>
#include <sys/ioctl.h>

#include "CommissionableInit.h"
#include "Device.h"
#include "main.h"
#include <app/server/Server.h>

#include <cassert>
#include <iostream>
#include <vector>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
namespace {

const int kNodeLabelSize_ = 32;
const int kDescriptorAttributeArraySize_ = 254;


#define DEVICE_TYPE_BRIDGED_NODE 0x0013
#define DEVICE_TYPE_LO_ON_OFF_LIGHT 0x0100
#define DEVICE_TYPE_ON_OFF_LIGHT_SWITCH 0x0103
#define DEVICE_TYPE_DIMMABLE_LIGHT 0x0101
#define DEVICE_TYPE_COLOR_TEMPERATURE_LIGHT 0x010C
#define DEVICE_TYPE_EXTENDED_COLOR_LIGHT 0x010D
#define DEVICE_TYPE_WINDOW_COVERING 0x0202

#define DEVICE_TYPE_POWER_SOURCE 0x0011
#define DEVICE_TYPE_TEMP_SENSOR 0x0302
#define DEVICE_TYPE_PRESSURE_SENSOR 0x0305
#define DEVICE_TYPE_FLOW_SENSOR 0x0306
#define DEVICE_TYPE_HUMIDITY_SENSOR 0x0307
#define DEVICE_TYPE_ILLUMINANCE_SENSOR 0x0106
#define DEVICE_TYPE_OCCUPANCY_SENSOR 0x0107
#define DEVICE_TYPE_MODE_SELECT_DEVICE 0x0027
#define DEVICE_TYPE_HEATING_COOLING_DEVICE 0x0300
#define DEVICE_VERSION_DEFAULT 1
#define DEVICE_TYPE_PRESSURE_SENSOR_for_STORAGE_BATTERY_DEVICE 0x0305


const EmberAfDeviceType gBridgedOnOffDeviceTypes_[] = { { DEVICE_TYPE_LO_ON_OFF_LIGHT, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gBridgedOnOffSwitchDeviceTypes_[] = { { DEVICE_TYPE_ON_OFF_LIGHT_SWITCH, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };                                                       
const EmberAfDeviceType gDimmableBridgedOnOffDeviceTypes_[] = { { DEVICE_TYPE_DIMMABLE_LIGHT, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gColorTemperatureBridgedLightDeviceTypes_[] = { { DEVICE_TYPE_COLOR_TEMPERATURE_LIGHT, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedComposedDeviceTypes_[] = { { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gComposedTempSensorDeviceTypes_[] = { { DEVICE_TYPE_TEMP_SENSOR, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gComposedPowerSourceDeviceTypes_[] = { { DEVICE_TYPE_POWER_SOURCE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedTempSensorDeviceTypes_[] = { { DEVICE_TYPE_TEMP_SENSOR, DEVICE_VERSION_DEFAULT },
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gBridgedIlluminanceSensorDeviceTypes_[] = { { DEVICE_TYPE_ILLUMINANCE_SENSOR, DEVICE_VERSION_DEFAULT },
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gBridgedHumiditySensorDeviceTypes_[] = { { DEVICE_TYPE_HUMIDITY_SENSOR, DEVICE_VERSION_DEFAULT },
{ DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gBridgedFlowSensorDeviceTypes_[] = { { DEVICE_TYPE_FLOW_SENSOR, DEVICE_VERSION_DEFAULT },
{ DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gBridgedFlowElectricSensorDeviceTypes_[] = { { DEVICE_TYPE_FLOW_SENSOR, DEVICE_VERSION_DEFAULT },
{ DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedPressureSensorDeviceTypes_[] = { { DEVICE_TYPE_PRESSURE_SENSOR, DEVICE_VERSION_DEFAULT },
{ DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedPressureBatterySensorDeviceTypes_[] = { { DEVICE_TYPE_PRESSURE_SENSOR_for_STORAGE_BATTERY_DEVICE, DEVICE_VERSION_DEFAULT },
{ DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };



const EmberAfDeviceType gBridgedWindowCoveringDeviceTypes_[] = { { DEVICE_TYPE_WINDOW_COVERING, DEVICE_VERSION_DEFAULT }, 
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedOccupancyDeviceTypes_[] = { { DEVICE_TYPE_OCCUPANCY_SENSOR, DEVICE_VERSION_DEFAULT },
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgeGenericCurtainDeviceTypes_[] = { { DEVICE_TYPE_MODE_SELECT_DEVICE, DEVICE_VERSION_DEFAULT }, 
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gHeatingCoolingDeviceTypes_[] = { { DEVICE_TYPE_HEATING_COOLING_DEVICE, DEVICE_VERSION_DEFAULT }, 
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };





DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(onOffAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::OnOff::Id, BOOLEAN, 1, 0), /* on/off */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(dimmableAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(LevelControl::Attributes::CurrentLevel::Id,INT8U,1, 0), /* LevelControl */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(colorControlAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(ColorControl::Attributes::ColorMode::Id, INT32U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(windowCoveringAttrs_)
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::Type::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::ConfigStatus::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::EndProductType::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::Mode::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::CurrentPositionLift::Id, INT16U,2,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::CurrentPositionTilt::Id, INT16U,2,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::CurrentPositionLiftPercentage::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::CurrentPositionTiltPercentage::Id, INT8U,1,0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();


DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(descriptorAttrs_)
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::DeviceTypeList::Id, ARRAY, kDescriptorAttributeArraySize_, 0), /* device list */
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ServerList::Id, ARRAY, kDescriptorAttributeArraySize_, 0), /* server list */
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ClientList::Id, ARRAY, kDescriptorAttributeArraySize_, 0), /* client list */
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::PartsList::Id, ARRAY, kDescriptorAttributeArraySize_, 0),  /* parts list */
DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(bridgedDeviceBasicAttrs_)
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::NodeLabel::Id, CHAR_STRING, kNodeLabelSize_, 0), /* NodeLabel */
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::Reachable::Id, BOOLEAN, 1, 0),              /* Reachable */
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::FeatureMap::Id, BITMAP32, 4, 0), /* feature map */
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorName::Id, CHAR_STRING, kNodeLabelSize_, 0), //0x8A
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorID::Id, INT32U, 4, 0),  //0x8A
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::ProductName::Id, CHAR_STRING, kNodeLabelSize_, 0), //0x8C
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SoftwareVersion::Id, INT32U, 4, 0), // 0x82
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SoftwareVersionString::Id, CHAR_STRING, kNodeLabelSize_, 0), //0x82
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::ManufacturingDate::Id, CHAR_STRING, kNodeLabelSize_, 0), //0x8E
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::PartNumber::Id, INT8U, 1, 0), // 0x8D
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SerialNumber::Id, INT8U, 1, 0), // 0x83
DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();


constexpr CommandId onOffIncomingCommands_[] = {
    app::Clusters::OnOff::Commands::Off::Id,
    app::Clusters::OnOff::Commands::On::Id,
    app::Clusters::OnOff::Commands::Toggle::Id,
    app::Clusters::OnOff::Commands::OffWithEffect::Id,
    app::Clusters::OnOff::Commands::OnWithRecallGlobalScene::Id,
    app::Clusters::OnOff::Commands::OnWithTimedOff::Id,
    kInvalidCommandId,
};

constexpr CommandId levelControlIncomingCommands_[] = {
    app::Clusters::LevelControl::Commands::MoveToLevel::Id,
    app::Clusters::LevelControl::Commands::Move::Id,
    app::Clusters::LevelControl::Commands::Step::Id,
    app::Clusters::LevelControl::Commands::Stop::Id,
    app::Clusters::LevelControl::Commands::MoveToLevelWithOnOff::Id,
    app::Clusters::LevelControl::Commands::MoveWithOnOff::Id,
    app::Clusters::LevelControl::Commands::StepWithOnOff::Id,
    app::Clusters::LevelControl::Commands::StopWithOnOff::Id,  
    kInvalidCommandId,
};
constexpr CommandId colorControlIncomingCommands_[] = {
    app::Clusters::ColorControl::Commands::MoveHue::Id ,
    app::Clusters::ColorControl::Commands::MoveHue::Id ,
    app::Clusters::ColorControl::Commands::StepHue::Id ,
    app::Clusters::ColorControl::Commands::MoveToSaturation::Id ,
    app::Clusters::ColorControl::Commands::StepSaturation::Id ,
    app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::Id ,
    app::Clusters::ColorControl::Commands::MoveColor::Id ,
    app::Clusters::ColorControl::Commands::StepColor::Id ,
    app::Clusters::ColorControl::Commands::MoveColorTemperature::Id ,
    app::Clusters::ColorControl::Commands::EnhancedMoveToHue::Id ,
    app::Clusters::ColorControl::Commands::EnhancedMoveHue::Id ,
    app::Clusters::ColorControl::Commands::EnhancedStepHue::Id ,
    app::Clusters::ColorControl::Commands::EnhancedMoveToHueAndSaturation::Id ,
    app::Clusters::ColorControl::Commands::ColorLoopSet::Id ,
    app::Clusters::ColorControl::Commands::StopMoveStep::Id ,
    app::Clusters::ColorControl::Commands::MoveColorTemperature::Id ,
    app::Clusters::ColorControl::Commands::StepColorTemperature::Id ,
    kInvalidCommandId,
};

constexpr CommandId windowCoveringIncomingCommands_[] = {
    app::Clusters::WindowCovering::Commands::UpOrOpen::Id, 
    app::Clusters::WindowCovering::Commands::DownOrClose::Id,
    app::Clusters::WindowCovering::Commands::GoToTiltValue::Id,
    app::Clusters::WindowCovering::Commands::GoToLiftValue::Id ,
    app::Clusters::WindowCovering::Commands::GoToLiftPercentage::Id,
    app::Clusters::WindowCovering::Commands::GoToTiltPercentage::Id,
    app::Clusters::WindowCovering::Commands::StopMotion::Id,
    kInvalidCommandId,
};


DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedLightClusters_)
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs_, onOffIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedLightEndpoint_, bridgedLightClusters_); 
DataVersion gLightDataVersions_[ArraySize(bridgedLightClusters_)]; 

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedLightSwitchClusters_)
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs_, onOffIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedLightSwitchEndpoint_, bridgedLightSwitchClusters_); 
DataVersion gLightDataSwitchVersions_[ArraySize(bridgedLightSwitchClusters_)]; 



DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedDimmableLightClusters_)
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs_, onOffIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(LevelControl::Id, dimmableAttrs_, levelControlIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedDimmableLightEndpoint_, bridgedDimmableLightClusters_); 
DataVersion gDimmableLightDataVersions_[ArraySize(bridgedDimmableLightClusters_)]; 




DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(windowCoveringClusters_)
    DECLARE_DYNAMIC_CLUSTER(WindowCovering::Id, windowCoveringAttrs_, windowCoveringIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(windowCoveringEndpoint_, windowCoveringClusters_); 
DataVersion gwindowCoveringDataVersions_[ArraySize(windowCoveringClusters_)]; 




DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(tempSensorAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::MeasuredValue::Id, INT16S, 2, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::MinMeasuredValue::Id, INT16S, 2, 0), /* Min Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::MaxMeasuredValue::Id, INT16S, 2, 0), /* Max Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();
DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedTempSensorClusters_)
DECLARE_DYNAMIC_CLUSTER(TemperatureMeasurement::Id, tempSensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedTempSensorEndpoint_, bridgedTempSensorClusters_);
DataVersion gTempSensorDataVersions_[ArraySize(bridgedTempSensorClusters_)];


DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(humiditySensorAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(RelativeHumidityMeasurement::Attributes::MeasuredValue::Id, INT16U, 2, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(RelativeHumidityMeasurement::Attributes::MinMeasuredValue::Id, INT16U, 2, 0), /* Min Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(RelativeHumidityMeasurement::Attributes::MaxMeasuredValue::Id, INT16U, 2, 0), /* Max Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(RelativeHumidityMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();
DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedHumiditySensorClusters_)
DECLARE_DYNAMIC_CLUSTER(RelativeHumidityMeasurement::Id, humiditySensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedHumiditySensorEndpoint_, bridgedHumiditySensorClusters_);
DataVersion gHumiditySensorDataVersions_[ArraySize(bridgedHumiditySensorClusters_)];



DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(flowSensorAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MeasuredValue::Id, INT16U, 2, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MinMeasuredValue::Id, INT16U, 2, 0), /* Min Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MaxMeasuredValue::Id, INT16U, 2, 0), /* Max Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();
DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedFlowSensorClusters_)
DECLARE_DYNAMIC_CLUSTER(FlowMeasurement::Id, flowSensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedFlowSensorEndpoint_, bridgedFlowSensorClusters_);
DataVersion gFlowSensorDataVersions_[ArraySize(bridgedFlowSensorClusters_)];



DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(flowSensorElectricAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MeasuredValue::Id, INT16U, 2, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MinMeasuredValue::Id, INT16U, 2, 0), /* Min Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::MaxMeasuredValue::Id, INT16U, 2, 0), /* Max Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(FlowMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();
DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedFlowSensorElectricClusters_)
DECLARE_DYNAMIC_CLUSTER(FlowMeasurement::Id, flowSensorElectricAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedFlowSensorElectricEndpoint_, bridgedFlowSensorElectricClusters_);
DataVersion gFlowSensorElectricDataVersions_[ArraySize(bridgedFlowSensorElectricClusters_)];


DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(pressureBatterySensorAttrs_)
DECLARE_DYNAMIC_ATTRIBUTE(PressureMeasurement::Attributes::MeasuredValue::Id, INT16S, 2, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(PressureMeasurement::Attributes::MinMeasuredValue::Id, INT16S, 2, 0), /* Min Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(PressureMeasurement::Attributes::MaxMeasuredValue::Id, INT16S, 2, 0), /* Max Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(PressureMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();
DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedPressureBatterySensorClusters_)
DECLARE_DYNAMIC_CLUSTER(PressureMeasurement::Id, pressureBatterySensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedPressureBatterySensorEndpoint_, bridgedPressureBatterySensorClusters_);
DataVersion gPressureBatterySensorDataVersions_[ArraySize(bridgedPressureBatterySensorClusters_)];





DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(illuminanceSensorAttrs_)
     DECLARE_DYNAMIC_ATTRIBUTE(IlluminanceMeasurement::Attributes::MeasuredValue::Id, INT16U, 2, 0),        /* Measured Value */
     DECLARE_DYNAMIC_ATTRIBUTE(IlluminanceMeasurement::Attributes::MinMeasuredValue::Id, INT16U, 2, 0), /* Min Measured Value */
     DECLARE_DYNAMIC_ATTRIBUTE(IlluminanceMeasurement::Attributes::MaxMeasuredValue::Id, INT16U, 2, 0), /* Max Measured Value */
     DECLARE_DYNAMIC_ATTRIBUTE(IlluminanceMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();


DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedIlluminanceSensorClusters_)
DECLARE_DYNAMIC_CLUSTER(IlluminanceMeasurement::Id, illuminanceSensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedIlluminanceSensorEndpoint_, bridgedIlluminanceSensorClusters_);
DataVersion gIlluminanceSensorDataVersions_[ArraySize(bridgedIlluminanceSensorClusters_)];




DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN( OccupancySensorAttrs_)
    DECLARE_DYNAMIC_ATTRIBUTE(OccupancySensing::Attributes::Occupancy::Id, BITMAP8, 1, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(OccupancySensing::Attributes::OccupancySensorType::Id, ENUM8, 1, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(OccupancySensing::Attributes::OccupancySensorTypeBitmap::Id, BITMAP8, 1, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedOccupancySensorClusters_)
    DECLARE_DYNAMIC_CLUSTER(OccupancySensing::Id, OccupancySensorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedOccupancySensorEndpoint_, bridgedOccupancySensorClusters_);
DataVersion gOccupancySensorDataVersions_[ArraySize(bridgedOccupancySensorClusters_)];






constexpr CommandId GenericAttrsIncomingCommands_[] = {
    app::Clusters::ModeSelect::Commands::ChangeToMode::Id,
    kInvalidCommandId,
};

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN( GenericAttrs_)
    DECLARE_DYNAMIC_ATTRIBUTE(ModeSelect::Attributes::Description::Id, CHAR_STRING, kDescriptorAttributeArraySize_, 0),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(ModeSelect::Attributes::StandardNamespace::Id,ENUM16,2,0 ),        /* Measured Value */
    DECLARE_DYNAMIC_ATTRIBUTE(ModeSelect::Attributes::SupportedModes::Id,ARRAY,kDescriptorAttributeArraySize_,0 ), 
    DECLARE_DYNAMIC_ATTRIBUTE(ModeSelect::Attributes::CurrentMode::Id,INT8U,1,0 ), 
    DECLARE_DYNAMIC_ATTRIBUTE(TemperatureMeasurement::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedGenericClusters_)
    DECLARE_DYNAMIC_CLUSTER(ModeSelect::Id, GenericAttrs_, GenericAttrsIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedGenericEndpoint_, bridgedGenericClusters_);
DataVersion gGenericDataVersions_[ArraySize(bridgedGenericClusters_)];

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedTemperatureLightClusters_)
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs_, onOffIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(LevelControl::Id, dimmableAttrs_, levelControlIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(ColorControl::Id, colorControlAttrs_ , colorControlIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(ModeSelect::Id, GenericAttrs_, GenericAttrsIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(bridgedTemperatureLightEndpoint_, bridgedTemperatureLightClusters_); 
DataVersion gColorTemperatureLightDataVersions_[ArraySize(bridgedTemperatureLightClusters_)]; 



//air conditioner
DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(thermostatAttrs_)
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::LocalTemperature::Id, INT16S, 2, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::OutdoorTemperature::Id, INT16S, 2, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::Occupancy::Id, BITMAP8, 1, ZAP_ATTRIBUTE_MASK(WRITABLE) | ZAP_ATTRIBUTE_MASK(NULLABLE)), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::ControlSequenceOfOperation::Id, ENUM8, 1, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::SystemMode::Id, ENUM8, 1, ZAP_ATTRIBUTE_MASK(WRITABLE) | ZAP_ATTRIBUTE_MASK(NULLABLE)), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();


constexpr CommandId thermostatIncomingCommands_[] = {
    kInvalidCommandId,
};


DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(fancontrolAttrs_)
     DECLARE_DYNAMIC_ATTRIBUTE(FanControl::Attributes::FanMode::Id, ENUM8, 1, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(FanControl::Attributes::FanModeSequence::Id, ENUM8, 1, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(FanControl::Attributes::PercentSetting::Id, INT8U, 1, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(FanControl::Attributes::PercentCurrent::Id, INT8U, 1, 0), 
     DECLARE_DYNAMIC_ATTRIBUTE(Thermostat::Attributes::FeatureMap::Id, BITMAP32, 4, 0),     /* FeatureMap */
DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

constexpr CommandId fancontrolIncomingCommands_[] = {
     kInvalidCommandId,
};

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(heatingColingClusters_)
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs_, onOffIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Thermostat::Id, thermostatAttrs_, thermostatIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(FanControl::Id, fancontrolAttrs_, fancontrolIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(LevelControl::Id, dimmableAttrs_, levelControlIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(ModeSelect::Id, GenericAttrs_, GenericAttrsIncomingCommands_, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs_, nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs_, nullptr,nullptr) 
DECLARE_DYNAMIC_CLUSTER_LIST_END;
DECLARE_DYNAMIC_ENDPOINT(heatingColingEndpoint_, heatingColingClusters_); 
DataVersion heatingColingDataVersions_[ArraySize(heatingColingClusters_)]; 

} // namespace
#endif