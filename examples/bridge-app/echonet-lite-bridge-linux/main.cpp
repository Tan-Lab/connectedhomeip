/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This is a C++ program that delivered from Matter's examples/bridged-app/linux 
 * 
 */

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
#include <app-common/zap-generated/callback.h>


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
#include <cassert>
#include <app/clusters/mode-select-server/supported-modes-manager.h>
#include <app/clusters/window-covering-server/window-covering-server.h>
#include <app/clusters/window-covering-server/window-covering-delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ModeSelect;
using namespace chip::app::Clusters::WindowCovering;
//using chip::Protocols::InteractionModel::Status;

#include"EchonetEndpoint.h"
#include"EchonetDevicesManager.h"
#include"MatterMetaData.h"
#include"Utils.h"
#include "include/EchonetWindowCoveringDelegate.h"
#include "include/EchonetWindowCoveringDelegate_IHouse.h"
// REVISION DEFINITIONS:
// =================================================================================

#define ZCL_DESCRIPTOR_CLUSTER_REVISION (1u)
#define ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_CLUSTER_REVISION (1u)
#define ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_FEATURE_MAP (0u)
#define ZCL_FIXED_LABEL_CLUSTER_REVISION (1u)
#define ZCL_ON_OFF_CLUSTER_REVISION (4u)
#define ZCL_TEMPERATURE_SENSOR_CLUSTER_REVISION (1u)
#define ZCL_TEMPERATURE_SENSOR_FEATURE_MAP (0u)
#define ZCL_POWER_SOURCE_CLUSTER_REVISION (1u)

// ---------------------------------------------------------------------------
Device * gDevices[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];
EndpointId gCurrentEndpointId=3;
EndpointId gFirstDynamicEndpointId;
int AddDeviceEndpoint(Device * dev, EmberAfEndpointType * ep, const Span<const EmberAfDeviceType> & deviceTypeList,
                      const Span<DataVersion> & dataVersionStorage, chip::EndpointId parentEndpointId = chip::kInvalidEndpointId)
{
    uint16_t index = 0;
    while (index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT)
    {
        if (nullptr == gDevices[index])
        {
            gDevices[index] = dev;
            EmberAfStatus ret;
            while (true)
            {
                // Todo: Update this to schedule the work rather than use this lock
                DeviceLayer::StackLock lock;
                dev->SetEndpointId(gCurrentEndpointId);
                dev->SetParentEndpointId(parentEndpointId);
                ret =
                    emberAfSetDynamicEndpoint(index, gCurrentEndpointId, ep, dataVersionStorage, deviceTypeList, parentEndpointId);
                if (ret == EMBER_ZCL_STATUS_SUCCESS)
                {
                    ChipLogProgress(DeviceLayer, "Added device %s to dynamic endpoint %d (index=%d)", dev->GetName(),
                                    gCurrentEndpointId, index);
                    return index;
                }
                if (ret != EMBER_ZCL_STATUS_DUPLICATE_EXISTS)
                {
                    return -1;
                }
                // Handle wrap condition
                if (++gCurrentEndpointId < gFirstDynamicEndpointId)
                {
                    gCurrentEndpointId = gFirstDynamicEndpointId;
                }
            }
        }
        index++;
    }
    ChipLogProgress(DeviceLayer, "Failed to add dynamic endpoint: No endpoints available!");
    return -1;
}

int RemoveDeviceEndpoint(Device * dev)
{
    uint16_t index = 0;
    while (index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT)
    {
        if (gDevices[index] == dev)
        {
            // Todo: Update this to schedule the work rather than use this lock
            DeviceLayer::StackLock lock;
            EndpointId ep   = emberAfClearDynamicEndpoint(index);
            gDevices[index] = nullptr;
            ChipLogProgress(DeviceLayer, "Removed device %s from dynamic endpoint %d (index=%d)",dev==nullptr?"NONAME":dev->GetName(), ep, index);
            // Silence complaints about unused ep when progress logging
            // disabled.
            UNUSED_VAR(ep);
            return index;
        }
        index++;
    }
    return -1;
}

std::vector<EndpointListInfo> GetEndpointListInfo(chip::EndpointId parentId)
{
    std::vector<EndpointListInfo> infoList;

    for (auto room : gRooms)
    {
        if (room->getIsVisible())
        {
            EndpointListInfo info(room->getEndpointListId(), room->getName(), room->getType());
            int index = 0;
            while (index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT)
            {
                if ((gDevices[index] != nullptr) && (gDevices[index]->GetParentEndpointId() == parentId))
                {
                    std::string location;
                    if (room->getType() == Actions::EndpointListTypeEnum::kZone)
                    {
                        location = gDevices[index]->GetZone();
                    }
                    else
                    {
                        location = gDevices[index]->GetLocation();
                    }
                    if (room->getName().compare(location) == 0)
                    {
                        info.AddEndpointId(gDevices[index]->GetEndpointId());
                    }
                }
                index++;
            }
            if (info.GetEndpointListSize() > 0)
            {
                infoList.push_back(info);
            }
        }
    }

    return infoList;
}

std::vector<Action *> GetActionListInfo(chip::EndpointId parentId)
{
    return gActions;
}

namespace {
void CallReportingCallback(intptr_t closure)
{
    auto path = reinterpret_cast<app::ConcreteAttributePath *>(closure);
    MatterReportingAttributeChangeCallback(*path);
    Platform::Delete(path);
}

void ScheduleReportingCallback(Device * dev, ClusterId cluster, AttributeId attribute)
{
    auto * path = Platform::New<app::ConcreteAttributePath>(dev->GetEndpointId(), cluster, attribute);
    PlatformMgr().ScheduleWork(CallReportingCallback, reinterpret_cast<intptr_t>(path));
}
} // anonymous namespace

void HandleDeviceStatusChanged(Device * dev, Device::Changed_t itemChangedMask)
{
    if (itemChangedMask & Device::kChanged_Reachable)
    {
        ScheduleReportingCallback(dev, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
    }

    if (itemChangedMask & Device::kChanged_Name)
    {
        ScheduleReportingCallback(dev, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::NodeLabel::Id);
    }
}

// For better performance, we use dedicated function for return the bridged information
// These data only request once when adding the echonetLITE endpoint.
EmberAfStatus HandleReadBridgedDeviceBasicAttribute(Device * dev, chip::AttributeId attributeId, uint8_t * buffer,
                                                    uint16_t maxReadLength)
{
    using namespace BridgedDeviceBasicInformation::Attributes;

    ChipLogProgress(DeviceLayer, "HandleReadBridgedDeviceBasicAttribute: attrId=%d, maxReadLength=%d", attributeId, maxReadLength);

    if ((attributeId == Reachable::Id) && (maxReadLength == 1))
    {
        *buffer = dev->IsReachable() ? 1 : 0;
    }
    else if ((attributeId == NodeLabel::Id) && (maxReadLength == 32))
    {
        
        MutableByteSpan zclNameSpan(buffer, maxReadLength);
        MakeZclCharString(zclNameSpan, dev->GetName());
        //printf("\n\n\n\n TOANSTT IS HERE HandleReadBridgedDeviceBasicAttribute : %s \n\n\n",buffer );
    }
    else if ((attributeId == ClusterRevision::Id) && (maxReadLength == 2))
    {
        uint16_t rev = ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_CLUSTER_REVISION;
        memcpy(buffer, &rev, sizeof(rev));
    }
    else if ((attributeId == FeatureMap::Id) && (maxReadLength == 4))
    {
        uint32_t featureMap = ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_FEATURE_MAP;
        memcpy(buffer, &featureMap, sizeof(featureMap));
    } else if (attributeId == VendorName::Id ||  attributeId == ProductName::Id || attributeId==SoftwareVersionString::Id || attributeId==ManufacturingDate::Id )
    {
        

        EchonetEndpoint* info = (EchonetEndpoint*)(dev->echonetEndpointInfoPointer);
        MutableByteSpan zclNameSpan(buffer, maxReadLength);
        MakeZclCharString(zclNameSpan,ConvertToString(info->GET_properties[Map_MatterAttribute_To_EPC[attributeId]].echonetValue).c_str());
    }
    else if (attributeId == VendorID::Id || attributeId == PartNumber::Id || attributeId == SerialNumber::Id )
    {
        EchonetEndpoint* info = (EchonetEndpoint*)(dev->echonetEndpointInfoPointer);
       uint32_t val =  ConvertToUnsignedInt(info->GET_properties[Map_MatterAttribute_To_EPC[attributeId]].echonetValue);

        memcpy(buffer, &val, sizeof(val));
        
    }
    else
    {
        printf("\n\n\n TOANSTT: EMBER_ZCL_STATUS_FAILURE\n\n\n\n");
        return EMBER_ZCL_STATUS_FAILURE;
    }

    return EMBER_ZCL_STATUS_SUCCESS;
}

EmberAfStatus HandleReadDeviceGeneralAttribute(Device * dev, ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength)
{
    EchonetEndpoint* echonetEndpoint = (EchonetEndpoint*)dev->echonetEndpointInfoPointer;
    ChipLogProgress(DeviceLayer, "\n HandleReadDeviceGeneralAttribute: device=0x%04x%02x clusterId=%d(0x%02x)   attrId=%d(0x%02x), maxReadLength=%d", echonetEndpoint->echoClassCode, echonetEndpoint->instanceCode, clusterId,clusterId,attributeId,attributeId, maxReadLength);
    
    
    //Try to read and convert value from echonetLITE endpoint 
    int ret= echonetEndpoint->ReadProperty(clusterId,attributeId,buffer,maxReadLength );
    
    if(ret != 0 )
    {
        printf("\n\n\n\n [HandleReadDeviceGeneralAttribute] TOANSTT NOT IMPLEMENT YET \n\n");
        return EMBER_ZCL_STATUS_NOT_FOUND;
    }

    return EMBER_ZCL_STATUS_SUCCESS;

    
}
EmberAfStatus emberAfExternalAttributeReadCallback(EndpointId endpoint, ClusterId clusterId,
                                                   const EmberAfAttributeMetadata * attributeMetadata, uint8_t * buffer,
                                                   uint16_t maxReadLength)
{
    
    TimeManager::GetInstance()->RecordTime(TimeRecordType::START_COMMAND_READ_FROM_CHIPTOOL, 0, 0, 0,(unsigned short)clusterId, attributeMetadata->attributeId,(unsigned int)buffer[0] );

    printf("\n\n\n\n\n\nDDDDD    [ExternalAttributeReadCallback]     DDDDDDD %d %d buf= %d \n \n\n\n\n\n\n\n",clusterId,LevelControl::Id,*buffer);
    uint16_t endpointIndex = emberAfGetDynamicIndexFromEndpoint(endpoint);
    EmberAfStatus ret = EMBER_ZCL_STATUS_FAILURE;
    if ((endpointIndex < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT) && (gDevices[endpointIndex] != nullptr))
    {
        Device * dev = gDevices[endpointIndex];
        if (clusterId == BridgedDeviceBasicInformation::Id)
            ret = HandleReadBridgedDeviceBasicAttribute(dev, attributeMetadata->attributeId, buffer, maxReadLength);
       
        else ret = HandleReadDeviceGeneralAttribute(dev,clusterId, attributeMetadata->attributeId, buffer,maxReadLength);
    }
    TimeManager::GetInstance()->RecordTime(TimeRecordType::END_COMMAND_READ_FROM_CHIPTOOL, 0, 0, 0,(unsigned short)clusterId, attributeMetadata->attributeId,(unsigned int)buffer[0] );
    return ret;
}





EmberAfStatus HandleWriteDeviceGeneralAttribute(Device * dev,ClusterId clusterId, chip::AttributeId attributeId, uint8_t * buffer)
{
    ChipLogProgress(DeviceLayer, "HandleWriteDeviceModeSelectAttribute: attrId=%d", attributeId);
    EchonetEndpoint* echonetEndpoint = (EchonetEndpoint*)dev->echonetEndpointInfoPointer;
    
    TimeManager::GetInstance()->RecordTime(TimeRecordType::START_COMMAND_WRITE_FROM_CHIPTOOL, echonetEndpoint->echoClassCode, echonetEndpoint->instanceCode, 0,(unsigned short)clusterId, attributeId,(unsigned int)buffer[0] );

    //Try to convert and write value to echonetLITE endpoint 
    EmberAfStatus ret = echonetEndpoint->WriteProperty(attributeId,clusterId,NULL,buffer);

    TimeManager::GetInstance()->RecordTime(TimeRecordType::END_COMMAND_WRITE_FROM_CHIPTOOL, echonetEndpoint->echoClassCode, echonetEndpoint->instanceCode, 0,(unsigned short)clusterId, attributeId,(unsigned int)buffer[0] );
    if(ret != 0 )
    {
        printf("\n\n\n\n [HandleWriteDeviceModeSelectAttribute] TOANSTT NOT IMPLEMENT YET \n\n");
        return EMBER_ZCL_STATUS_NOT_FOUND;
    }
    

    return ret;
}

EmberAfStatus emberAfExternalAttributeWriteCallback(EndpointId endpoint, ClusterId clusterId,
                                                    const EmberAfAttributeMetadata * attributeMetadata, uint8_t * buffer)
{

    

    printf("\n emberAfExternalAttributeWriteCallback \n ");

    

    uint16_t endpointIndex = emberAfGetDynamicIndexFromEndpoint(endpoint);

    EmberAfStatus ret = EMBER_ZCL_STATUS_FAILURE;

    // ChipLogProgress(DeviceLayer, "emberAfExternalAttributeWriteCallback: ep=%d", endpoint);

    if (endpointIndex < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT)
    {
        Device * dev = gDevices[endpointIndex];
        ret = HandleWriteDeviceGeneralAttribute(dev,clusterId, attributeMetadata->attributeId, buffer);
    }
    
    return ret;
}



void runOnOffRoomAction(Room * room, bool actionOn, EndpointId endpointId, uint16_t actionID, uint32_t invokeID, bool hasInvokeID)
{
    
    if (hasInvokeID)
    {
        Actions::Events::StateChanged::Type event{ actionID, invokeID, Actions::ActionStateEnum::kActive };
        EventNumber eventNumber;
        chip::app::LogEvent(event, endpointId, eventNumber);
    }


    if (hasInvokeID)
    {
        Actions::Events::StateChanged::Type event{ actionID, invokeID, Actions::ActionStateEnum::kInactive };
        EventNumber eventNumber;
        chip::app::LogEvent(event, endpointId, eventNumber);
    }
}

bool emberAfActionsClusterInstantActionCallback(app::CommandHandler * commandObj, const app::ConcreteCommandPath & commandPath,
                                                const Actions::Commands::InstantAction::DecodableType & commandData)
{
    bool hasInvokeID      = false;
    uint32_t invokeID     = 0;
    EndpointId endpointID = commandPath.mEndpointId;
    auto & actionID       = commandData.actionID;

    if (commandData.invokeID.HasValue())
    {
        hasInvokeID = true;
        invokeID    = commandData.invokeID.Value();
    }

    if (actionID == action1.getActionId() && action1.getIsVisible())
    {
        // Turn On Lights in Room 1
        runOnOffRoomAction(&room1, true, endpointID, actionID, invokeID, hasInvokeID);
        commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::Success);
        return true;
    }
    if (actionID == action2.getActionId() && action2.getIsVisible())
    {
        // Turn On Lights in Room 2
        runOnOffRoomAction(&room2, true, endpointID, actionID, invokeID, hasInvokeID);
        commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::Success);
        return true;
    }
    if (actionID == action3.getActionId() && action3.getIsVisible())
    {
        // Turn Off Lights in Room 1
        runOnOffRoomAction(&room1, false, endpointID, actionID, invokeID, hasInvokeID);
        commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::Success);
        return true;
    }

    commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::NotFound);
    return true;
}

void ApplicationInit()
{
    const bool kThreadEnabled = {
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
        LinuxDeviceOptions::GetInstance().mThread
#else
        false
#endif
    };

    const bool kWiFiEnabled = {
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
        LinuxDeviceOptions::GetInstance().mWiFi
#else
        false
#endif
    };

    if (kThreadEnabled && kWiFiEnabled)
    {
        // Just use the Thread one.
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
        sThreadNetworkCommissioningInstance.Init();
#endif
    }
    else if (kThreadEnabled)
    {
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
        sThreadNetworkCommissioningInstance.Init();
#endif
    }
    else if (kWiFiEnabled)
    {
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
        sWiFiNetworkCommissioningInstance.Init();
#endif
    }
    else
    {
        sEthernetNetworkCommissioningInstance.Init();
    }
}
                                 
#define POLL_INTERVAL_MS (100)
uint8_t poll_prescale = 0;

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}


// Runtime controlling commands, for reset, show infomation,...
void * bridge_polling_thread(void * context)
{
    while (true)
    {
        if (kbhit())
        {
            int ch = getchar();
            if (ch == 'R')
            {
                printf("\n\n ==================== Perform Factory Reset ======== \n ");
                chip::Server::GetInstance().ScheduleFactoryReset();
            }
            else if (ch == 'E')
            {
                printf("\n\n ==================== Perform Exit ======== \n ");
                exit(0);
            }
            else if(ch == 'e')
            {
                printf("\n\n\n================ List of current Matter endpoints ===============\n");
                for(int i =0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++)
                {
                    if(gDevices[i]!=NULL)
                    {
                        Device * device = gDevices[i];
                        printf("%d: %s \n",(int)device->GetEndpointId(),device->GetName()) ;
                    }
                }
                printf("==================================================\n\n");
            }
            else if(ch == 'F')
            {
                printf("\n\n\n================ List of current Matter endpoints (detail) ===============\n");
                for(int i =0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++)
                {
                    if(gDevices[i]!=NULL)
                    {
                        Device * device = gDevices[i];
                        EchonetEndpoint *ep = (EchonetEndpoint *)(device->echonetEndpointInfoPointer);

                        printf("%d: %s%s",(int)device->GetEndpointId(),device->GetName(),ep->PropertiesInfoToString().c_str() ) ;
                        
                    }
                }
                printf("==================================================\n\n");
            }
           else if(ch == 'f')
            {
                printf("\n\n\n================ List of endpoints ===============\n");
                int counte =0;
                for(int i =0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++)
                {
                    if(gDevices[i]!=NULL)
                    {
                        Device * device = gDevices[i];
                        EchonetEndpoint *ep = (EchonetEndpoint *)(device->echonetEndpointInfoPointer);

                        printf("%d: %s%s",(int)device->GetEndpointId(),device->GetName(),ep->PropertiesPairsInfoToString().c_str() ) ;
                        counte++;
                    }
                }
                printf("======================count=%d============================\n\n",(int)counte);
            }
            else if(ch == 'g')
            {
                printf("\n\n\n================ List of all echonetLITE enpoints ===============\n");
                int counte =0;
                char t[128];
                map<pair<string,unsigned int>, EchonetEndpoint*>::iterator it;
                for (it = EchonetDevicesManager::GetInstance()->endpoints.begin(); it != EchonetDevicesManager::GetInstance()->endpoints.end(); it++)
                {
                    EchonetEndpoint *ep = it->second;
                    counte++;

                    int matterEPID = -1;
                    for(int i =0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++)
                    if(gDevices[i]!=NULL && (EchonetEndpoint *)(gDevices[i]->echonetEndpointInfoPointer) == ep )
                    {
                        matterEPID = (int)gDevices[i]->GetEndpointId();
                        break;
                    }

                    sprintf(t," %s: 0x%06x \n status=%s  type=\"%s\"  epId=%d  \n", it->first.first.c_str(),it->first.second, ep->isAddedToMatter?"ADDED":"NONE" , GetMatterEndpointTypeName(ep->type).c_str(),matterEPID  );
                    //printf("%s",t);
                    
                    printf("%s data:%s \n",t,ep->PropertiesPairsInfoToString().c_str() ) ;

                    
                }

                printf("======================count=%d============================\n\n",(int)counte);
            }
            else if(ch == 'h')
            {
                EchonetDevicesManager::GetInstance()->
                PrintEchonetDevicesSummary();
            }
            else if(ch == 'j')
            {
                
                printf("\n\n============Print OpenEcho Summary============\n");
                std::vector<std::shared_ptr<EchoNode> > nodes = Echo::getNodes();
                for(int it = 0;
                it <  (int)nodes.size(); it++ )
                {
                    printf(" %s \n",nodes[it]->getAddress().c_str() ) ;

                }
                printf("=========nNode=%d =========================================\n\n",(int)nodes.size());

            }
            else if(ch == 'k')
            {
                int countechoDevices =0;
                int nMapped =0;
                int nSkipped = 0;
                //nEP = 0;
                printf("\n\n============Print OpenEcho Summary (detail) ============\n");
                std::vector<std::shared_ptr<EchoNode> > nodes = Echo::getNodes();
                for(int it = 0;it <  (int)nodes.size(); it++ )
                {
                    std::shared_ptr<EchoNode> node = nodes[it];
                    printf("%s\n",node->getAddress().c_str() ) ;

                    std::vector<std::shared_ptr<DeviceObject> >  devices = node->
                    getDevices();

                    for(int it2 = 0;it2 <  (int)devices.size(); it2++ )
                    {
                        countechoDevices++;
                        std::shared_ptr<DeviceObject>  device = devices[it2];
                        unsigned int EchoClassCode = (unsigned int)device->getEchoClassCode();
                        unsigned short InstanceCode = device->getInstanceCode();
                        EchonetEndpoint*  ep = EchonetDevicesManager::GetInstance()->GetEchonetEndpointById(
                            make_pair(node->getAddress(),EchoClassCode*256+InstanceCode )
                        );

                        


                        //printf("\t0x%04x%02x ", EchoClassCode, InstanceCode);
                        printf("%04x%02x\t", EchoClassCode, InstanceCode);

                        if(ep!=NULL )
                        {
                            if(ep->device!=NULL)
                            {
                                int epId = (int)ep->device->GetEndpointId();
                                if(epId>0)
                                {
                                    printf("mapEP=%d",epId );
                                    nMapped++;
                                } else
                                {
                                    printf("mapEP=SKIPPED");   
                                    nSkipped++;
                                }
                            }
                            else printf("mapEP=NO_RESPONSE");
                        }

                        printf("\n");



                    }
                }
                printf("=========nNode=%d nDevices=%d nMapped=%d nSkipped=%d =========================================\n\n",(int)nodes.size(),countechoDevices,nMapped,nSkipped);

            }
            else if(ch == 'c')
            {
                printf("\n\n============Current configs============\n");
                printf("ASNCHRONOUS_COMISSION_WRITE\t\t: %s\n",STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE==true?"TRUE":"FALSE"  );
                printf("ASNCHRONOUS_COMISSION_READ\t\t: %s\n",STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ==true?"TRUE":"FALSE"  );
                printf("REQUEST_GET_INTERVAL\t\t: %d\n",STATIC_CONFIG_REQUEST_GET_INTERVAL  );
                printf("STATIC_CONFIG_ECHONET_CLASSCODE_FILTER\t\t: %d\n",STATIC_CONFIG_ECHONET_CLASSCODE_FILTER  );
                printf("STATIC_CONFIG_DEVICE_TIMEOUT_SECONDS\t\t: %d\n",STATIC_CONFIG_DEVICE_TIMEOUT_SECONDS  );
                
                printf("=======================================\n\n");
            }
            else if(ch == 'b')
            {
                EchonetDevicesManager::GetInstance()->isNeedToBroadcastDiscoverImmediately = true;
            }
            continue;
        }

        // Sleep to avoid tight loop reading commands
        
        usleep(POLL_INTERVAL_MS * 1000);
    }

    return nullptr;
}

//Delegate called when a valid echonetLITE endpoint is added
int OnAEchonetDeviceAdded(EchonetEndpoint *echonetEndpointInfo)
{
    if(echonetEndpointInfo->device!=NULL)
    {
        if(echonetEndpointInfo->emberAfEndpointType==NULL) printf("\n\n [WARNING ] emberAfEndpointType is NULL \n\n");
        AddDeviceEndpoint(echonetEndpointInfo->device, echonetEndpointInfo->emberAfEndpointType, echonetEndpointInfo->deviceTypeList ,echonetEndpointInfo->dataVersionStorage, 1);
        uint16_t endpointId = echonetEndpointInfo->device->GetEndpointId();

        //For ModeSelect cluster
        StaticSupportedModesManager::instance.AddEchonetSupportedOptionsByEndpoint(endpointId,echonetEndpointInfo->echonetOptionType );

        printf("EchonetEndpoint Added index = %d  optiontype=%d \n",  endpointId,echonetEndpointInfo->echonetOptionType);

        //For window covering, we need call extra Matter delegate when receving comission
        if(echonetEndpointInfo->type == MatterEchonetLITECombineEndpointType::WINDOW_COVERING)
        {
            EchonetWindowCoveringDelegate* delegate = new EchonetWindowCoveringDelegate(echonetEndpointInfo);
            delegate->SetEndpoint(endpointId);
            chip::app::Clusters::WindowCovering::SetDefaultDelegate(endpointId,delegate); 
        }
        else if(echonetEndpointInfo->type == MatterEchonetLITECombineEndpointType::WINDOW_COVERING_IHOUSE)
        {
            EchonetWindowCoveringDelegate_IHouse* delegate2 = new EchonetWindowCoveringDelegate_IHouse(echonetEndpointInfo);
            delegate2->SetEndpoint(endpointId);
            chip::app::Clusters::WindowCovering::SetDefaultDelegate(endpointId,delegate2); 
        }
    } else printf("[ERRROR]\n");
 
    
    return 0;
}
//Delegate called when a valid echonetLITE endpoint is removed
int OnAEchonetDeviceRemoved(EchonetEndpoint *echonetEndpointInfo)
{
    RemoveDeviceEndpoint(echonetEndpointInfo->device);
    return 0;
}
int main(int argc, char * argv[])
{
    ProceseParameters(argc, argv);
    argc =1;
    argv[argc] = NULL;


    TimeManager::GetInstance()->RecordTime(TimeRecordType::APP_START);

    //Initialize EchonetDevicesManager instance
    EchonetDevicesManager::instance = new EchonetDevicesManager();
    EchonetDevicesManager* manager = EchonetDevicesManager::GetInstance();
    manager->SetCallBackFunctions(&OnAEchonetDeviceAdded,&OnAEchonetDeviceRemoved);
    //Start thread to start OpenEchonet
    manager->FindEchonetDevices();

    memset(gDevices, 0, sizeof(gDevices));

    printf("\n\n %d \n\n",argc);


    
    


    if (ChipLinuxAppInit(argc, argv) != 0)
    {
        return -1;
    }

    // Init Data Model and CHIP App Server
    static chip::CommonCaseDeviceServerInitParams initParams;
    (void) initParams.InitializeStaticResourcesBeforeServerInit();

#if CHIP_DEVICE_ENABLE_PORT_PARAMS
    initParams.operationalServicePort = LinuxDeviceOptions::GetInstance().securedDevicePort;
#endif


    initParams.interfaceId = LinuxDeviceOptions::GetInstance().interfaceId;

    chip::Server::GetInstance().Init(initParams);
    SetDeviceAttestationCredentialsProvider(Examples::GetExampleDACProvider());

    gFirstDynamicEndpointId = static_cast<chip::EndpointId>(
        static_cast<int>(emberAfEndpointFromIndex(static_cast<uint16_t>(emberAfFixedEndpointCount() - 1))) + 1);
    gCurrentEndpointId = gFirstDynamicEndpointId;
    emberAfEndpointEnableDisable(emberAfEndpointFromIndex(static_cast<uint16_t>(emberAfFixedEndpointCount() - 1)), false);

    
    gRooms.push_back(&room1);
    gRooms.push_back(&room2);
    gRooms.push_back(&room3);

    gActions.push_back(&action1);
    gActions.push_back(&action2);
    gActions.push_back(&action3);
    {
        pthread_t poll_thread;
        int res = pthread_create(&poll_thread, nullptr, bridge_polling_thread, nullptr);
        if (res)
        {
            printf("Error creating polling thread: %d\n", res);
            exit(1);
        }
    }

    // Run CHIP

    ApplicationInit();
    
    
    chip::DeviceLayer::PlatformMgr().RunEventLoop();
    return 0;
} 
