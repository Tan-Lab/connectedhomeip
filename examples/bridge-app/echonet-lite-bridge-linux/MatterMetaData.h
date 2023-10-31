#pragma once
#ifndef MATTERMETADATA_H
#define MATTERMETADATA_H

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

const int kNodeLabelSize = 32;
// Current ZCL implementation of Struct uses a max-size array of 254 bytes
const int kDescriptorAttributeArraySize = 254;



std::vector<Room *> gRooms;
std::vector<Action *> gActions;

#if CHIP_DEVICE_LAYER_TARGET_LINUX
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
DeviceLayer::NetworkCommissioning::LinuxThreadDriver sThreadDriver;
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD

#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
DeviceLayer::NetworkCommissioning::LinuxWiFiDriver sWiFiDriver;
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI

DeviceLayer::NetworkCommissioning::LinuxEthernetDriver sEthernetDriver;
#endif // CHIP_DEVICE_LAYER_TARGET_LINUX

#if CHIP_DEVICE_LAYER_TARGET_DARWIN
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
DeviceLayer::NetworkCommissioning::DarwinWiFiDriver sWiFiDriver;
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI

DeviceLayer::NetworkCommissioning::DarwinEthernetDriver sEthernetDriver;
#endif // CHIP_DEVICE_LAYER_TARGET_DARWIN

#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
Clusters::NetworkCommissioning::Instance sWiFiNetworkCommissioningInstance(0, &sWiFiDriver);
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
Clusters::NetworkCommissioning::Instance sThreadNetworkCommissioningInstance(0, &sThreadDriver);
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD

Clusters::NetworkCommissioning::Instance sEthernetNetworkCommissioningInstance(0, &sEthernetDriver);



                                                            
const int16_t minMeasuredValue     = -27315;
const int16_t maxMeasuredValue     = 32766;
const int16_t initialMeasuredValue = 100;

// ENDPOINT DEFINITIONS:
// =================================================================================
//
// Endpoint definitions will be reused across multiple endpoints for every instance of the
// endpoint type.
// There will be no intrinsic storage for the endpoint attributes declared here.
// Instead, all attributes will be treated as EXTERNAL, and therefore all reads
// or writes to the attributes must be handled within the emberAfExternalAttributeWriteCallback
// and emberAfExternalAttributeReadCallback functions declared herein. This fits
// the typical model of a bridge, since a bridge typically maintains its own
// state database representing the devices connected to it.

// Device types for dynamic endpoints: TODO Need a generated file from ZAP to define these!
// (taken from matter-devices.xml)
#define DEVICE_TYPE_BRIDGED_NODE 0x0013
// (taken from lo-devices.xml)
#define DEVICE_TYPE_LO_ON_OFF_LIGHT 0x0100
#define DEVICE_TYPE_DIMMABLE_LIGHT 0x0101
#define DEVICE_TYPE_EXTENDED_COLOR_LIGHT 0x010D
// (taken from matter-devices.xml)
#define DEVICE_TYPE_POWER_SOURCE 0x0011
// (taken from matter-devices.xml)
#define DEVICE_TYPE_TEMP_SENSOR 0x0302

// Device Version for dynamic endpoints:
#define DEVICE_VERSION_DEFAULT 1



const EmberAfDeviceType gBridgedOnOffDeviceTypes[] = { { DEVICE_TYPE_LO_ON_OFF_LIGHT, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };
const EmberAfDeviceType gDimmableBridgedOnOffDeviceTypes[] = { { 0x0101, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gColorTemperatureBridgedLightDeviceTypes[] = { { 0x010C, DEVICE_VERSION_DEFAULT },
                                                       { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedComposedDeviceTypes[] = { { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gComposedTempSensorDeviceTypes[] = { { DEVICE_TYPE_TEMP_SENSOR, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gComposedPowerSourceDeviceTypes[] = { { DEVICE_TYPE_POWER_SOURCE, DEVICE_VERSION_DEFAULT } };

const EmberAfDeviceType gBridgedTempSensorDeviceTypes[] = { { DEVICE_TYPE_TEMP_SENSOR, DEVICE_VERSION_DEFAULT },
                                                            { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT } };


// ---------------------------------------------------------------------------
//
// LIGHT ENDPOINT: contains the following clusters:
//   - On/Off
//   - Descriptor
//   - Bridged Device Basic Information

// Declare On/Off cluster attributes
// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(onOffAttrs)
// DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::OnOff::Id, BOOLEAN, 1, 0), /* on/off */
//     DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(dimmableAttrs)
// DECLARE_DYNAMIC_ATTRIBUTE(LevelControl::Attributes::CurrentLevel::Id,INT32U,1, 0), /* LevelControl */
//     DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(colorControlAttrs)
// DECLARE_DYNAMIC_ATTRIBUTE(ColorControl::Attributes::ColorMode::Id, INT32U,1,0),
//     DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(extendedColorAttrs)
// DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::OnOff::Id, BOOLEAN, 1, 0), /* on/off */
// DECLARE_DYNAMIC_ATTRIBUTE(LevelControl::Attributes::CurrentLevel::Id,BITMAP32, 4, 0), /* LevelControl */
// DECLARE_DYNAMIC_ATTRIBUTE(ColorControl::Attributes::ColorMode::Id,BITMAP32, 4, 0), /* ColorControl */
//     DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Descriptor cluster attributes
// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(descriptorAttrs)
//     DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::DeviceTypeList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* device list */
//     DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ServerList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* server list */
//     DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ClientList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* client list */
//     DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::PartsList::Id, ARRAY, kDescriptorAttributeArraySize, 0),  /* parts list */
// DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Bridged Device Basic Information cluster attributes
// DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(bridgedDeviceBasicAttrs2)
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::NodeLabel::Id, CHAR_STRING, kNodeLabelSize, 0), /* NodeLabel */
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::Reachable::Id, BOOLEAN, 1, 0),              /* Reachable */
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::FeatureMap::Id, BITMAP32, 4, 0), /* feature map */
//     //toanstt
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorName::Id, CHAR_STRING, kNodeLabelSize, 0), //0x8A
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorID::Id, INT32U, 4, 0),  //0x8A
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::ProductName::Id, CHAR_STRING, kNodeLabelSize, 0), //0x8C
//     //DECLARE_DYNAMIC_ATTRIBUTE(0x0006, CHAR_STRING, kNodeLabelSize, 0), //0x8C
//     //DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::Location::Id, CHAR_STRING, kNodeLabelSize, 0), //0x81
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SoftwareVersion::Id, INT32U, 4, 0), // 0x82
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SoftwareVersionString::Id, CHAR_STRING, kNodeLabelSize, 0), //0x82
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::ManufacturingDate::Id, CHAR_STRING, kNodeLabelSize, 0), //0x8E
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::PartNumber::Id, INT32U, 4, 0), // 0x8D
//     DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::SerialNumber::Id, INT32U, 4, 0), // 0x83

    
// DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Cluster List for Bridged Light endpoint
// TODO: It's not clear whether it would be better to get the command lists from
// the ZAP config on our last fixed endpoint instead.
constexpr CommandId onOffIncomingCommands[] = {
    app::Clusters::OnOff::Commands::Off::Id,
    app::Clusters::OnOff::Commands::On::Id,
    app::Clusters::OnOff::Commands::Toggle::Id,
    app::Clusters::OnOff::Commands::OffWithEffect::Id,
    app::Clusters::OnOff::Commands::OnWithRecallGlobalScene::Id,
    app::Clusters::OnOff::Commands::OnWithTimedOff::Id,
    kInvalidCommandId,
};

constexpr CommandId levelControlIncomingCommands[] = {
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
constexpr CommandId colorControlIncomingCommands[] = {
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


Room room1("Room 1", 0xE001, Actions::EndpointListTypeEnum::kRoom, true);
Room room2("Room 2", 0xE002, Actions::EndpointListTypeEnum::kRoom, true);
Room room3("Zone 3", 0xE003, Actions::EndpointListTypeEnum::kZone, false);

Action action1(0x1001, "Room 1 On", Actions::ActionTypeEnum::kAutomation, 0xE001, 0x1, Actions::ActionStateEnum::kInactive, true);
Action action2(0x1002, "Turn On Room 2", Actions::ActionTypeEnum::kAutomation, 0xE002, 0x01, Actions::ActionStateEnum::kInactive,true);
Action action3(0x1003, "Turn Off Room 1", Actions::ActionTypeEnum::kAutomation, 0xE003, 0x01, Actions::ActionStateEnum::kInactive,false);


} // namespace
#endif