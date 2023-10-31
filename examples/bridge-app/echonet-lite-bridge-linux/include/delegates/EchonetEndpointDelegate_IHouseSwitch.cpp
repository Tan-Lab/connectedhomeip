#ifndef EchonetEndpointDelegate_IHouseSwitch_CPP
#define EchonetEndpointDelegate_IHouseSwitch_CPP
#include "EchonetEndpointDelegate_IHouseSwitch.h"
#include "../../EchonetEndpoint.h"

EmberAfStatus EchonetEndpointDelegate_IHouseSwitch::WriteProperty(chip::AttributeId attributeId, ClusterId clusterId, const EmberAfAttributeMetadata * attributeMetadata,uint8_t * buffer)
{
    printf("\n\n\n\n\n\n\n\n\n\n WriteProperty_iHouseSwitch clusterId=0x%08x, attributeId=0x%08x \n\n\n\n\n\n\n",clusterId, attributeId );
    EchonetEndpoint*ep = (EchonetEndpoint*)myEchonetEndpointpointer;
    unsigned char epc = 0x80;
    std::vector<unsigned char> value = {0x30};
    
    EmberAfStatus errorCode = EMBER_ZCL_STATUS_SUCCESS;
    
    shared_ptr<DeviceObject> device_;
    if (buffer[0] == 0) //close = 0x42
    {
        device_ = ep->GetDeviceObject(0x05fd,2 );
    }
    else if (buffer[0] == 01) // open = 0x41
    {
        device_ = ep->GetDeviceObject(0x05fd,1 );
    }
    else 
    {
        printf("\n\n\n [ERROR][IHOUSE] Cannot find the case for epc=0x%02x val=0x%02x \n\n\n",  epc, value[0]);
        return EMBER_ZCL_STATUS_DEPRECATED83;
    }
    if(device_ == nullptr)
    {
        printf("\n\n\n\n\n[ERROR] Cannot find echo device with address %s  \n\n\nn\n\n", ep->address.c_str());
    }
    
    EchoFrame eFrame = device_.get()->set(true).reqSetProperty(epc,value).send();
    printf("____ EchonetEndpoint::SetGET epc=0x%02x  value=%s tid=%d\n",epc, ConvertEchonetValueToHexString(value).c_str(), eFrame.getTID());
    //test only

    ep->currentWaitingTID = eFrame.getTID();
    TimeManager::GetInstance()->RecordTime(TimeRecordType::START_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE, ep->echoClassCode,ep->instanceCode,epc ,ep->eoj_pair.second>>8,ep->eoj_pair.second%256, ConvertToUnsignedInt(value) );

    if (STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE== false)
    {
        for(int i =0; i < 500; i++)
        {
            if(ep->currentWaitingTID  == 0xFFFF) 
            {
                TimeManager::GetInstance()->RecordTime(TimeRecordType::END_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE, ep->echoClassCode,ep->instanceCode,epc ,ep->eoj_pair.second>>8,ep->eoj_pair.second%256, ConvertToUnsignedInt(value) );
                break;
            }
            usleep(10 * 1000); //sleep 10 miliseconds
        }
        //if(ep->currentWaitingTID != 0xFFFF) errorCode = EMBER_ZCL_STATUS_DEPRECATED83; //for test only
    }
    
    ep->GET_properties[epc].echonetValue = value;
    return errorCode;
    

    return EMBER_ZCL_STATUS_SUCCESS;
}
int EchonetEndpointDelegate_IHouseSwitch::ReadProperty(chip::ClusterId clusterId,chip::AttributeId attributeId, uint8_t * buffer, uint16_t maxReadLength)
{
    EchonetEndpoint*ep = (EchonetEndpoint*)myEchonetEndpointpointer;
    printf("\n\n\n\n\n\n\n\n\n\n ReadProperty_iHouseSwitch clusterId=0x%08x, attributeId=0x%08x \n\n\n\n\n\n\n",clusterId, attributeId );

    ep->currentWaitingPropertyId = 0x80;
    shared_ptr<DeviceObject> device_ = ep->GetDeviceObject(0x05fd, 01);
    if(device_ == nullptr) return 1;

    device_->get().reqGetProperty(ep->currentWaitingPropertyId).send();
    if (STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ== false)
    {
        for(int i =0; i < 500; i++)
        {
            if(i%50==0)
            printf("[ReadProperty ihouse] Waiting for response 0x%02x (sleeping 50ms)\n",ep->currentWaitingPropertyId);
            if(ep->currentWaitingPropertyId  == 0xFF) 
            {
                break;
            }
            usleep(10 * 1000); 
        }
    }
    
    vector<unsigned char> echonetValue = ep->GET_properties[0x80].echonetValue;

    if(echonetValue.size()>0)
    {
    if(echonetValue[0] == 0x30)//open
        buffer[0] = 1;
    else if(echonetValue[0] == 0x31) //close
        buffer[0] = 0;
    else buffer[0] = 0;
    } else buffer[0] = 0;

    return 0;
}
EmberAfStatus EchonetEndpointDelegate_IHouseSwitch::SetGET(unsigned char epc, vector<unsigned char> value)
{
    return (EmberAfStatus)0;
}
#endif