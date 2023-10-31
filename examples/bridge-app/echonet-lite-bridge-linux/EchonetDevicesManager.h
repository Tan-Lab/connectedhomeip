/*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This class manages the matter and echonetLITE endpoints
 * The enpoint mappings are stored in a map of pair<string,unsigned int>
 *    mmap<pair<string,unsigned int>, EchonetEndpoint*> endpoints;
 *    where "unsigned int" value store the <classCode><instanceCode> (6 bytes) of echonetLITE endpoint
 */
 
#pragma once
#ifndef ECHONETDEVICES_MANAGER_H
#define ECHONETDEVICES_MANAGER_H
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
#include "EchonetEndpoint.h"
//#include "EchonetEndpoint_IHouseWindowCovering.h"

#include <iostream>
#include <memory>
#include "openecho/OpenECHO.h"

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


class MyControllerReceiver : public DeviceObject::Receiver {
public:
	MyControllerReceiver(){}
	virtual ~MyControllerReceiver(){}
protected:
	virtual void onGetOperationStatus(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success) {
		cout << "Controller power : ";
		for(unsigned char b : property.edt) {
			cout << hex << (int)b << " ";
		}
		cout << endl;
	}
	virtual void onGetInstallationLocation(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success) {
		cout << "Installation location : ";
		for(unsigned char b : property.edt) {
			cout << hex << (int)b << " ";
		}
		cout << endl;
	}
};


class EchonetControllerReceiver : public DeviceObject::Receiver {
public:
	EchonetControllerReceiver(){}
	virtual ~EchonetControllerReceiver(){}
protected:
	virtual bool onSetProperty(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success);
	virtual bool onGetProperty(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property, bool success);
	virtual bool onInformProperty(std::shared_ptr<EchoObject> eoj, unsigned short tid, unsigned char esv, EchoProperty& property);
};

class MyEventListener : public Echo::EventListener {
	virtual void onNewDeviceObject(std::shared_ptr<DeviceObject> device);
	virtual void onNewNode(std::shared_ptr<EchoNode> node)
	{
		//printf("onNewNode: %s\n",node.get()->getAddress().c_str());
	}
    virtual void onFoundNode(std::shared_ptr<EchoNode> node)
    {
		//printf("onFoundNode %s \n",node.get()->getAddress().c_str());
	}
    virtual void onNewEchoObject(std::shared_ptr<EchoObject> eoj)
    {
		
		//printf("onNewEchoObject\n"); 
		//printf("NNNN onNewEchoObject len=0x%04x-%02x\n", eoj->getEchoClassCode(), eoj->getInstanceCode()  );
	}
    virtual void onFoundEchoObject(std::shared_ptr<EchoObject> eoj);
    
    virtual void onNewNodeProfile(std::shared_ptr<NodeProfile> profile)
    {
		printf("onNewNodeProfile\n");
	}

    virtual void onSendFrame(EchoFrame& frame)
    {
		//printf("onSendFrame %s \n",frame.getSrcEchoAddress().c_str());
	}
    virtual void onReceiveFrame(EchoFrame& frame)
    {
        // printf("\nonReceiveFrame: %s\n",frame.getSrcEchoAddress().c_str());
		// for(int j =0; j < (int)frame.getPropertyList().size(); j++ )
		// {
		// 	vector<unsigned char> edt = frame.getPropertyList()[j].edt;
		// 	printf(" epc: %02x (%ld):" , frame.getPropertyList()[j].epc, edt.size());
			
		// 	if(edt.size()>0)
		// 	{
				

		// 		for(int k = 0; k <(int)edt.size(); k++ )
		// 		{
		// 			unsigned char* converted = &edt[k];
		// 			//unsigned int val = *((unsigned int*)converted);
		// 			printf(" %02x ", * converted);
		// 		}
		// 		printf("\n");
		// 	}
		// }
    }
};

class DefaultNodeProfile : public NodeProfile {

public:
	DefaultNodeProfile() : NodeProfile() {}
	virtual ~DefaultNodeProfile(){}
protected:
	virtual shared_ptr<vector<unsigned char> > getManufacturerCode() {
		return shared_ptr<vector<unsigned char> >();
	}
	virtual shared_ptr<vector<unsigned char> > getOperatingStatus() {
		return shared_ptr<vector<unsigned char> >();
	}
	virtual shared_ptr<vector<unsigned char> > getIdentificationNumber() {
		return shared_ptr<vector<unsigned char> >();
	}
};

class DefaultController : public DeviceObject {

public:
	DefaultController() : DeviceObject() {}
	virtual ~DefaultController(){}

	virtual unsigned short getEchoClassCode() {
		return 0x05FF;
	}
protected:
	virtual shared_ptr<vector<unsigned char> > getOperationStatus() {
		return shared_ptr<vector<unsigned char> >();
	}
	virtual bool setInstallationLocation(vector<unsigned char>& edt) {
		return false;
	}
	virtual shared_ptr<vector<unsigned char> > getInstallationLocation() {
		return shared_ptr<vector<unsigned char> >();
	}
	virtual shared_ptr<vector<unsigned char> > getFaultStatus() {
		return shared_ptr<vector<unsigned char> >();
	}
	virtual shared_ptr<vector<unsigned char> > getManufacturerCode() {
		return shared_ptr<vector<unsigned char> >();
	}
};


typedef int (*OnAEchonetEndpointAddedDelegate) (EchonetEndpoint *echonetEndpointInfo);
typedef int (*OnAEchonetEndpointRemovedDelegate) (EchonetEndpoint *echonetEndpointInfo);
class EchonetDevicesManager
{
    private:
		mutex myMutex_;
    public:
		static EchonetDevicesManager* instance;
        //std::vector<EchonetDevice*> devices;
        map<pair<string,unsigned int>, EchonetEndpoint*> endpoints;
        OnAEchonetEndpointAddedDelegate onAEchonetEndpointAddedDelegate;
		OnAEchonetEndpointRemovedDelegate onAEchonetEndpointRemovedDelegate;
    public:
		bool isNeedToBroadcastDiscoverImmediately = false;

    public:
        EchonetDevicesManager();
        int numDevices;
        void SetCallBackFunctions(OnAEchonetEndpointAddedDelegate onAEchonetEndpointAddedDelegate_,
		OnAEchonetEndpointRemovedDelegate onAEchonetEndpointRemovecDelegate_)
        {
            this->onAEchonetEndpointAddedDelegate = onAEchonetEndpointAddedDelegate_;
			this->onAEchonetEndpointRemovedDelegate = onAEchonetEndpointRemovecDelegate_;
        } 
        void FindEchonetDevices();
        void PrintDevicesSummary();
        static EchonetDevicesManager* GetInstance(){ return instance;};
        void OnFoundEchoObject(std::shared_ptr<EchoObject> eoj);
		void AddEchonetGetAttributeValue(pair<string,unsigned int>& id, EchoProperty& echoProperty,std::shared_ptr<EchoObject> eoj);
		void AddDeviceObject(std::shared_ptr<DeviceObject> deviceObject,pair<string,unsigned int>& id );
		//void AddDeviceObject(EchonetEndpoint* echonetEndpoint_,pair<string,unsigned int>& id );
		EchonetEndpoint* GetEchonetEndpointById(pair<string,unsigned int> id);
		void CheckStartupDone();
		void RemoveAnEchonetEndpoint(EchonetEndpoint* e);
		void WriteAllAdapterInfoToTextFile(string filaname="out/0_adapters.json");
		void PrintEchonetDevicesSummary();
		void ProactiveIntervalRequestDataFromEchonetDevices();
		void RemoveDeviceObject(pair<string,unsigned int>& id);

};





#endif