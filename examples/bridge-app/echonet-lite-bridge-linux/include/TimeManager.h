 /*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This class supports log time event into file
 */
#pragma once
#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include<map>
#include<vector>
#include <iostream>
#include <chrono>
#include <ctime>  
using namespace std::chrono;
using namespace std;


//#define MY_TO_STRING(type) "##type##"



enum TimeRecordType{
    APP_START,
    APP_BEGIN_SEND_MULTI_CAST_5D,
    FOUND_AN_ECHONET_ENDPOINT,
    GET_A_ENDPOINT_INFO,
    RECEIVE_ECHONET_PROPERTY_VALUE,
    
    START_COMMAND_READ_FROM_CHIPTOOL,
    START_COMMAND_WRITE_FROM_CHIPTOOL,
    END_COMMAND_READ_FROM_CHIPTOOL,
    END_COMMAND_WRITE_FROM_CHIPTOOL,
    
    START_SEND_READ_COMMAND_TO_ECHONET_DEVICE,
    START_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE,

    END_SEND_READ_COMMAND_TO_ECHONET_DEVICE,
    END_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE,


    RECEIVE_NOTIFY_FROM_ECHONET_DEVICE,
    FINISH_STARTUP,
    PROCESSED_AN_ECHONET_ENDPOINT,

    STARTUP_COUNT_DEVICE
};
extern string MY_TO_STRING(TimeRecordType t);

struct TimeRecordItem{
    TimeRecordType timeRecordType;
    unsigned short echoClassCode;
    unsigned short echoInstanceCode;
    unsigned char epc;
    unsigned int mattterAtributeId;
    unsigned int mattterClusterId;
    unsigned int value;
    
    std::chrono::_V2::system_clock::time_point time;
    microseconds time_from_start;
    void SetEchoClassAndInstance(unsigned short echoClassCode_, unsigned short echoInstanceCode_){echoClassCode = echoClassCode_;echoInstanceCode = echoInstanceCode_;};
    
};
class TimeManager
{
    char buf[256];
    static TimeManager* instance;
    std::chrono::_V2::system_clock::time_point timeStart;
    public:
        //ofstream myfile;
        vector<TimeRecordItem> timeRecordItems;
    public:
        TimeManager();
        ~TimeManager();
        static TimeManager* GetInstance(){ 
			if(instance==nullptr){instance = new TimeManager();}
			return instance;};        \
        void RecordTime(TimeRecordType timeRecordType_,unsigned short echoClassCode_=0, unsigned char echoInstanceCode_=0, unsigned char epc_=0, unsigned int mattterClusterId_=0 , unsigned int mattterAtributeId_=0, unsigned int value_=0);
        void WriteAllToFile();
        void AppendALine(TimeRecordItem *item);
};



#endif