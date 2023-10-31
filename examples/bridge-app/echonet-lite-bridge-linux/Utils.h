 /*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This class store the the struct for the mapping rule
 */
 
#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "CommissionableInit.h"
#include "Device.h"
#include "TimeManager.h"
#include <string>
#include <vector>
#include <map>
//#include "EchonetEndpoint.h"
using namespace std;
extern bool STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE;
extern bool STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ;
extern int STATIC_CONFIG_REQUEST_GET_INTERVAL;
extern int STATIC_CONFIG_ECHONET_CLASSCODE_FILTER;
extern int STATIC_CONFIG_DEVICE_TIMEOUT_SECONDS;

extern map<unsigned char, string>   Map_ZCL_DATA_TYPE_TO_NAME;
extern map<unsigned char, uint32_t>   Map_EPC_To_MatterAttribute; // econet property code
extern map<uint32_t,unsigned char>   Map_MatterAttribute_To_EPC ;

//extern map<uint32_t,uint32_t>  Map_EOJ_To_MatterEndpoint;  //econet object 
//extern map<uint32_t,uint32_t>  Map_MatterEndpoint_To_EOJ;  //econet object 

extern vector<string> locationDictionary;//[] = {"NA","Living Room","Dinning Room","Kitchen","Bathroom","Lavatory","Washroom/changing room","Passageway","Room","Stairway","Front door","Storeroom","Garden/perimeter ","Garage","Veranda/balcony","Others"};
extern string bufferDictionaryCode;

//MatterEchonetLITECombineEndpointType
enum MatterEchonetLITECombineEndpointType
{
    UNKNOW=0,
    ONOFF_LIGHT = 0x0100,
    DIMMABLE_LIGHT = 0x0101,
    COLOR_TEMPERATURE_LIGHT = 0x010D,
    TEMPERATURE_SENSOR = 0x0302,
    TEMPERATURE_SENSOR_CO2 = 0x1302,
    WINDOW_COVERING = 0x0202,
    WINDOW_COVERING_IHOUSE = 0xF202,
    MODESELECT=0x0027,
    MODESELECT_IHOUSE=0xF027,
    WINDOW = 0x1111,
    ILLUMINANCE_SENSOR = 0x0106,
    HUMIDITY_SENSOR = 0x0307,
    FLOW_SENSOR = 0x0306,
    
    OCCUPANCY_SENSOR=0x0107,
    OCCUPANCY_SENSOR_EMGERGENCY_BUTTON= 0x1107,
    HEATING_COOLING_UNIT = 0x0300,
    ONOFF_LIGHT_SWITCH = 0x0103, 
    ONOFF_LIGHT_SWITCH_IHOUSE = 0xF103, 
    FLOW_SENSOR_for_ELECTRIC_ENERGY_SENSOR = 0x0305,
    PRESSURE_SENSOR_for_STORAGE_BATTERY = 0x1305,

    
}; 




enum EchonetOptionType
{
    OPTION_UNKNOW=0,
    OPEN_CLOSE_STOP_41_42_43,
    LIGHT_COLOR_SETTING_LAMP_WHITE_DAYLIGHTWHITE_DAYLIGHCOLOR_OTHER_41_42_43_44_40,
    POWER_SAVING_OPERATION_SETTING_41_42
}; 

// enum EchonetDataTypeAndLength{
//     INT8S= 0x28,
//     INT8U=0x20,
//     INT16S=0x29,
//     INT16U=0x21,
//     INT24S=0x2A,
//     INT24U=0x22,
//     INT32S=0x2B,
//     INT32U=0x23,
//     ENUM_0X41,
// };




extern int CreateSocketConnection(string& ip, int portNo=3361);
extern bool IsAUnsignedIntNumber(string& s);
extern vector<string> split(const string &s, char delim);
extern std::vector<char> HexToBytes(const std::string& hex);
extern string HexToAsciiString(const std::string& hex);
extern unsigned int HexToUnsignedInt(const std::string& hex);
extern int Hex1ByteToSignedInt(const std::string& hex);
extern unsigned char Hex1ByteToUnSignedInt(const std::string& hex);
extern short Hex2ByteToSignedInt(const std::string& hex);
extern unsigned short Hex2ByteToUnSignedInt(const std::string& hex);
extern void TurnTheLight_Test2(string& ip, string& endpoint,bool isOn);
extern MatterEchonetLITECombineEndpointType GetMatterEndpointTypeFromEchonetEndpointCode(void* ehonetEndpoint);
//extern MatterEchonetLITECombineEndpointType GetMatterEndpointTypeFromEchonetCodes(unsigned short echoClassCode, unsigned char instanceCode);
extern int WriteDataToMatterBuffer(uint8_t * buffer, unsigned int val, uint16_t matterDataType, uint16_t matterDataLength); 

extern std::string IntToHex( unsigned int i );
extern std::string IntToHex( int i );
extern std::string IntToHex( unsigned short i );
extern std::string IntToHex( short i );
//extern std::vector<unsigned char> FastMakeVectorArray(unsigned char value);
extern int ConvertToInt(vector<unsigned char> a);
extern unsigned int ConvertToUnsignedInt(vector<unsigned char> a);
extern short ConvertToShort(vector<unsigned char> a);
extern unsigned short ConvertToUnsignedShort(vector<unsigned char> a);
extern char ConvertToChar(vector<unsigned char> a);
extern unsigned char ConvertToUnsignedChar(vector<unsigned char> a);
extern string ConvertToString(vector<unsigned char> a);
extern string ConvertEchonetValueToHexString(vector<unsigned char> a);
extern string ConvertUnsignedValueToHexString(unsigned int a, bool isAddPrefix= true);
extern string ConvertUnsignedValueToHexString(unsigned short a, bool isAddPrefix= true);
extern string ConvertUnsignedValueToHexString(unsigned char a, bool isAddPrefix= true);
extern string GetMatterEndpointTypeName(MatterEchonetLITECombineEndpointType type);
extern string GetEchonetEndpointTypeName(unsigned short echonetClassCode);
extern int ProceseParameters(int argc, char * argv[]);
#endif