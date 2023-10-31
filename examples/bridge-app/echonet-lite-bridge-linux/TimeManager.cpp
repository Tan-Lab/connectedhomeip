#ifndef TIMEMANAGER_CPP
#define TIMEMANAGER_CPP
#include "TimeManager.h"
#include <fstream>
#include<iomanip>
#include<sstream>

using namespace std;

extern string MY_TO_STRING(TimeRecordType t)
{
    switch (t)
    {
    case TimeRecordType::APP_START: return "APP_START";
    case TimeRecordType::APP_BEGIN_SEND_MULTI_CAST_5D: return "APP_BEGIN_SEND_MULTI_CAST_5D";
    case TimeRecordType::FOUND_AN_ECHONET_ENDPOINT: return "FOUND_AN_ECHONET_ENDPOINT";
    case TimeRecordType::GET_A_ENDPOINT_INFO: return "GET_A_ENDPOINT_INFO";
    case TimeRecordType::RECEIVE_ECHONET_PROPERTY_VALUE: return "RECEIVE_ECHONET_PROPERTY_VALUE";
    case TimeRecordType::START_COMMAND_READ_FROM_CHIPTOOL: return "START_COMMAND_READ_FROM_CHIPTOOL";
    case TimeRecordType::START_COMMAND_WRITE_FROM_CHIPTOOL: return "START_COMMAND_WRITE_FROM_CHIPTOOL";
    case TimeRecordType::END_COMMAND_READ_FROM_CHIPTOOL: return "END_COMMAND_READ_FROM_CHIPTOOL";
    case TimeRecordType::END_COMMAND_WRITE_FROM_CHIPTOOL: return "END_COMMAND_WRITE_FROM_CHIPTOOL";
    case TimeRecordType::RECEIVE_NOTIFY_FROM_ECHONET_DEVICE: return "RECEIVE_NOTIFY_FROM_ECHONET_DEVICE";
    case TimeRecordType::START_SEND_READ_COMMAND_TO_ECHONET_DEVICE: return "START_SEND_READ_COMMAND_TO_ECHONET_DEVICE";
    case TimeRecordType::FINISH_STARTUP: return "FINISH_STARTUP";
    case TimeRecordType::START_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE: return "START_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE";
    case TimeRecordType::END_SEND_READ_COMMAND_TO_ECHONET_DEVICE: return "END_SEND_READ_COMMAND_TO_ECHONET_DEVICE";
    case TimeRecordType::END_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE: return "END_SEND_WRITE_COMMAND_TO_ECHONET_DEVICE";
    case TimeRecordType::PROCESSED_AN_ECHONET_ENDPOINT: return "PROCESSED_AN_ECHONET_ENDPOINT";
    case TimeRecordType::STARTUP_COUNT_DEVICE: return "STARTUP_COUNT_DEVICE";


    //case TimeRecordType::APP_START: return "APP_START";
    //case TimeRecordType::APP_START: return "APP_START";
    //case TimeRecordType::APP_START: return "APP_START";
    
    default:
        break;
    }
     return "UNKNOW";
};

TimeManager* TimeManager::instance= nullptr;;
TimeManager::TimeManager()
{
    //timeStart = std::time(NULL); 
    //time(&timeStart);
    timeStart = high_resolution_clock::now();
    ofstream myfile ("out/0_output.txt", std::ios_base::app);
    myfile << "\t\t\t\t\t\t\n===========New run============\t\t\t\t\t\t\n";
    myfile.close();
}
TimeManager::~TimeManager()
{

}
void TimeManager::RecordTime(TimeRecordType timeRecordType_,unsigned short echoClassCode_, unsigned char echoInstanceCode_,unsigned char epc_,  unsigned int mattterClusterId_, unsigned int mattterAtributeId_, unsigned int value_)
{
    return; //temporary disable time manager
    TimeRecordItem *item = new TimeRecordItem{timeRecordType_,echoClassCode_,echoInstanceCode_, epc_,mattterAtributeId_,mattterClusterId_,value_};
    //item->time = std::chrono::system_clock::now();
    //item->time = std::time(NULL);  
    //time(&(item->time));
    item->time = high_resolution_clock::now();
    //item->time_from_start= double(item->time-this->timeStart);/// double(CLOCKS_PER_SEC);
     item->time_from_start = duration_cast<microseconds>(item->time - this->timeStart);
    //printf("\n\n\nCCCCCCCCCCCCCCCCCCCC %ld CCCCCCCCCCCCCCCCCCCCCCC\n\n\n",item->time_from_start);
    this->timeRecordItems.push_back(*item);
    AppendALine(item);

    //if(timeRecordType_ == TimeRecordType::FINISH_STARTUP) exit(0);

}
void TimeManager::WriteAllToFile()
{
    //ofstream myfile ("example.bin", ios::out | ios::app | ios::binary);

    ofstream myfile ("example.txt");
    if (myfile.is_open())
    {
        for(int i =0; i < (int)this->timeRecordItems.size(); i++)
        {
            TimeRecordItem *item = &this->timeRecordItems[i];
            
            myfile << item->time_from_start.count() << '\t' << MY_TO_STRING(item->timeRecordType) << '\t' << (int)(item->epc) << '\t' << item->mattterAtributeId << '\t'<<
            item->mattterClusterId;
            myfile << endl;
        }
        myfile.close();
    }

}
void TimeManager::AppendALine(TimeRecordItem *item)
{
    ofstream myfile ("out/0_output.txt", std::ios_base::app);
    sprintf(buf,"%ld\t%s\t0x%04x%02x\t0x%02x\t0x%04x\t0x%06x\t0x%06x\n",item->time_from_start.count(),MY_TO_STRING(item->timeRecordType).c_str(),item->echoClassCode, item->echoInstanceCode, item->epc ,item->mattterClusterId,item->mattterAtributeId, item->value);
    // myfile << item->time_from_start.count() << '\t' << MY_TO_STRING(item->timeRecordType) << "\t0x" << std::setfill('0')<< hex << item->echoClassCode <<   " "<<std::setfill('0')<<hex<< item->echoInstanceCode  << "\t0x" << hex << int(item->epc)<< '\t' << item->mattterAtributeId << '\t'<<
    //     item->mattterClusterId;
    //     myfile << endl;
    myfile << buf;
    myfile.close();
}
#endif
