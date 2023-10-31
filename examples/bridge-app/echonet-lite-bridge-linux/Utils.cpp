#ifndef UTILS_CPP
#define UTILS_CPP
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <map>
#include "Device.h"
#include "EchonetEndpoint.h"
#include <iostream>
#include <memory>
#include "openecho/OpenECHO.h"
#include <mutex>

using namespace std;
// OpenECHOのnamespaceは"sonycsl_openecho"
using namespace sonycsl_openecho;
using namespace chip;
using namespace chip::app;
using namespace chip::Credentials;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::DeviceLayer;
using namespace chip::app::Clusters;
using namespace std;

bool STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE = true;
bool STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ = true;
int STATIC_CONFIG_REQUEST_GET_INTERVAL =120;
int STATIC_CONFIG_ECHONET_CLASSCODE_FILTER=-1;
int STATIC_CONFIG_DEVICE_TIMEOUT_SECONDS = 5;

std::vector<std::pair<std::string,unsigned int>> blackListEnpoints = {
    make_pair("192.168.2.157", 0x05fd02),
    make_pair("192.168.2.158", 0x05fd02),
    make_pair("192.168.2.183", 0x05fd02),
    make_pair("192.168.2.184", 0x05fd02),

    make_pair("192.168.2.159", 0x05fd02),
    make_pair("192.168.2.160", 0x05fd02),
    make_pair("192.168.2.161", 0x05fd02),
    make_pair("192.168.2.162", 0x05fd02),
    make_pair("192.168.2.163", 0x05fd02),
    make_pair("192.168.2.164", 0x05fd02),
    make_pair("192.168.2.165", 0x05fd02),
    make_pair("192.168.2.166", 0x05fd02),
    make_pair("192.168.2.167", 0x05fd02),
    make_pair("192.168.2.168", 0x05fd02),

    make_pair("192.168.2.178", 0x05fd02),
    make_pair("192.168.2.179", 0x05fd02),
    make_pair("192.168.2.180", 0x05fd02),

    make_pair("192.168.2.153", 0x05fd02),
    make_pair("192.168.2.156", 0x05fd02),
    make_pair("192.168.2.148", 0x05fd02),

};

std::vector<std::pair<std::string,unsigned int>> ihouseListWindowCoverings = {
    make_pair("192.168.2.157", 0x05fd01),
    make_pair("192.168.2.158", 0x05fd01),
    make_pair("192.168.2.183", 0x05fd01),
    make_pair("192.168.2.184", 0x05fd01),

    // make_pair("192.168.2.178", 0x05fd01),
    // make_pair("192.168.2.179", 0x05fd01),
    // make_pair("192.168.2.180", 0x05fd01),
};
std::vector<std::pair<std::string,unsigned int>> ihouseListWindows = {
    make_pair("192.168.2.159", 0x05fd01),
    make_pair("192.168.2.160", 0x05fd01),
    make_pair("192.168.2.161", 0x05fd01),
    make_pair("192.168.2.162", 0x05fd01),
    make_pair("192.168.2.163", 0x05fd01),
    make_pair("192.168.2.164", 0x05fd01),
    make_pair("192.168.2.165", 0x05fd01),
    make_pair("192.168.2.166", 0x05fd01),
    make_pair("192.168.2.167", 0x05fd01),
    make_pair("192.168.2.168", 0x05fd01),
};



map<unsigned char, string>   Map_ZCL_DATA_TYPE_TO_NAME = {{0x00,"NO_DATA"},{0x10,"BOOLEAN"},{0x18,"BITMAP8"},{0x19,"BITMAP16"},{0x1B,"BITMAP32"},{0x1F,"BITMAP64"},{0x20,"INT8U"},{0x21,"INT16U"},{0x22,"INT24U"},{0x23,"INT32U"},{0x24,"INT40U"},{0x25,"INT48U"},{0x26,"INT56U"},{0x27,"INT64U"},{0x28,"INT8S"},{0x29,"INT16S"},{0x2A,"INT24S"},{0x2B,"INT32S"},{0x2C,"INT40S"},{0x2D,"INT48S"},{0x2E,"INT56S"},{0x2F,"INT64S"},{0x30,"ENUM8"},{0x31,"ENUM16"},{0x39,"SINGLE"},{0x3A,"DOUBLE"},{0x41,"OCTET_STRING"},{0x42,"CHAR_STRING"},{0x43,"LONG_OCTET_STRING"},{0x44,"LONG_CHAR_STRING"},{0x48,"ARRAY"},{0x4C,"STRUCT"},{0xE0,"TOD"},{0xE1,"DATE"},{0xE2,"UTC"},{0xE3,"EPOCH_US"},{0xE4,"EPOCH_S"},{0xE5,"SYSTIME_US"},{0xE6,"PERCENT"},{0xE7,"PERCENT100THS"},{0xE8,"CLUSTER_ID"},{0xE9,"ATTRIB_ID"},{0xEA,"FIELD_ID"},{0xEB,"EVENT_ID"},{0xEC,"COMMAND_ID"},{0xED,"ACTION_ID"},{0xEF,"TRANS_ID"},{0xF0,"NODE_ID"},{0xF1,"VENDOR_ID"},{0xF2,"DEVTYPE_ID"},{0xF3,"FABRIC_ID"},{0xF4,"GROUP_ID"},{0xF5,"STATUS"},{0xF6,"DATA_VER"},{0xF7,"EVENT_NO"},{0xF8,"ENDPOINT_NO"},{0xF9,"FABRIC_IDX"},{0xFA,"IPADR"},{0xFB,"IPV4ADR"},{0xFC,"IPV6ADR"},{0xFD,"IPV6PRE"},{0xFE,"HWADR"},{0xFF,"UNKNOWN"}};

map<unsigned char, uint32_t>   Map_EPC_To_MatterAttribute = {
//{"0x8A_",BridgedDeviceBasicInformation::Attributes::VendorID::Id },
{0x8A,BridgedDeviceBasicInformation::Attributes::VendorName::Id },
{0x8C,BridgedDeviceBasicInformation::Attributes::ProductName::Id},
//{"0x82_",BridgedDeviceBasicInformation::Attributes::SoftwareVersion::Id},
{0x82,BridgedDeviceBasicInformation::Attributes::SoftwareVersionString::Id},
{0x8E,BridgedDeviceBasicInformation::Attributes::ManufacturingDate::Id},
{0x8D,BridgedDeviceBasicInformation::Attributes::PartNumber::Id},
{0x83,BridgedDeviceBasicInformation::Attributes::SerialNumber::Id},
};
map<uint32_t,unsigned char>   Map_MatterAttribute_To_EPC = {
{BridgedDeviceBasicInformation::Attributes::VendorID::Id,0x8A, },
{BridgedDeviceBasicInformation::Attributes::VendorName::Id,0x8A },
{BridgedDeviceBasicInformation::Attributes::ProductName::Id,0x8C},
{BridgedDeviceBasicInformation::Attributes::SoftwareVersion::Id,0x82},
{BridgedDeviceBasicInformation::Attributes::SoftwareVersionString::Id,0x82},
{BridgedDeviceBasicInformation::Attributes::ManufacturingDate::Id,0x8E},
{BridgedDeviceBasicInformation::Attributes::PartNumber::Id,0x8D},
{BridgedDeviceBasicInformation::Attributes::SerialNumber::Id,0x83},
};
vector<string> locationDictionary{"NA","Living Room","Dinning Room","Kitchen","Bathroom","Lavatory","Washroom/changing room","Passageway","Room","Stairway","Front door","Storeroom","Garden/perimeter ","Garage","Veranda/balcony","Others"};
string bufferDictionaryCode = "F0E0D0C0B0A09080F1E1D1C1B1A19181F2E2D2C2B2A29282F3E3D3C3B3A39383F4E4D4C4B4A49484F5E5D5C5B5A59585F6E6D6C6B6A69686F7E7D7C7B7A79787F8E8D8C8B8A89888F9E9D9C9B9A99989FAEADACABAAA9A8AFBEBDBCBBBAB9B8BFCECDCCCBCAC9C8CFDEDDDCDBDAD9D8DFEEEDECEBEAE9E8EFFEFDFCFBFAF9F8F";
extern std::vector<char> HexToBytes(const std::string& hex) {
  std::vector<char> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

extern string HexToAsciiString(const std::string& hex){
    int len = (int)hex.length();
    std::string newString;
    for(int i=0; i< len; i+=2)
    {
        std::string byte = hex.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}

extern unsigned int HexToUnsignedInt(const std::string& hex)
{
    vector<char> bytes =  HexToBytes(hex);
    if(bytes.size()==4)
        return (unsigned char)bytes[3] + (unsigned char)bytes[2]*256 + (unsigned char)bytes[1]*65536+ (unsigned char)bytes[0]*16777216;
    if(bytes.size()==1)
        return  (unsigned char)bytes[0];
    if(bytes.size()==3)
        return  (unsigned char)bytes[2] + (unsigned char)bytes[1]*256 + (unsigned char)bytes[0]*65536;//+ (unsigned int)bytes[0]*16777216;
    if(bytes.size()==2)
        return (unsigned char)bytes[1] + (unsigned char)bytes[0]*256;// + (unsigned int)bytes[0]*65536;//+ (unsigned int)bytes[0]*16777216;
    printf("\n[TOAN ERROR] Cast error with length = %d \n\n", (int)bytes.size() );
    return 1111111111;
}
extern int Hex1ByteToSignedInt(const std::string& hex)
{
    vector<char> bytes =  HexToBytes(hex);
    // if(bytes.size()==4)
    //     return (char)bytes[3] + (unsigned char)bytes[2]*256 + (unsigned char)bytes[1]*65536+ (unsigned char)bytes[0]*16777216;
    if(bytes.size()==1)
        return  (int)((char)bytes[0]);
    // if(bytes.size()==3)
    //     return  (unsigned char)bytes[2] + (unsigned char)bytes[1]*256 + (unsigned char)bytes[0]*65536;//+ (unsigned int)bytes[0]*16777216;
    // if(bytes.size()==2)
    //     return (unsigned char)bytes[1] + (unsigned char)bytes[0]*256;// + (unsigned int)bytes[0]*65536;//+ (unsigned int)bytes[0]*16777216;
    return -111111111;
}
extern short Hex2ByteToSignedInt(const std::string& hex)
{
    unsigned short x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    // output it as a signed type
    return static_cast<short>(x);
}
extern unsigned short Hex2ByteToUnSignedInt(const std::string& hex)
{
    unsigned short x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    return static_cast<unsigned short>(x);
}
extern unsigned char Hex1ByteToUnSignedInt(const std::string& hex)
{
    unsigned short x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    return static_cast<unsigned uint8_t>(x);
}



extern int CreateSocketConnection(string& ip, int portNo)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    //uint16_t portNo =3361;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  printf("\n\nERROR opening socket\n\n");
    server = gethostbyname(ip.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port =htons( (uint16_t)portNo);
    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        printf("\n\nERROR connecting\n\n");

    return sockfd;
}
bool IsAUnsignedIntNumber(string& s)
{
    if(s.size()==0 || s.size()>9 ) return false;
    for(int i =0; i < (int)s.size(); i++)
        if(s[i] < '0' || s[i] > '9') return false;
    return true;
};

extern vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
    if (!item.empty()) {
            elems.push_back(item);
            //printf("%s\n", item.c_str());
        }
    }
    return elems;
}


extern void TurnTheLight_Test2(string& ip, string& endpoint,bool isOn)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    uint16_t portNo =3361;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        printf("\n\nERROR opening socket\n\n");
    server = gethostbyname("150.65.230.90"); // 150.65.230.146 150.65.230.90
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port =htons( portNo);
    char buffer[256];
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        printf("\n\nERROR connecting\n\n");

    if(isOn)
        sprintf(buffer,"%s:%s:0x80,0x30", ip.c_str(),endpoint.c_str() );
    else sprintf(buffer,"%s:%s:0x80,0x31", ip.c_str(),endpoint.c_str() );
    write(sockfd, buffer, strlen(buffer));
    close(sockfd);
}

bool IsSupportedEPC(shared_ptr<std::set<unsigned char> > m, unsigned char key)
{
    return !(m->find(key) == m->end()) ;
}



// extern MatterEchonetLITECombineEndpointType GetMatterEndpointTypeFromEchonetCodes(unsigned short echoClassCode, unsigned char instanceCode)
// {
    
// }

extern std::string IntToHex( unsigned int your_int )
{
    printf("\n\n\n\n AAAAAA 0 \n\n\n");
 std::stringstream stream;
    stream << std::hex << your_int;
    std::string result( stream.str() );
    string s = stream.str();
    if(s.size()==1) s = "0x0" + s; else s = "0x" + s;
  return s;
}
extern std::string IntToHex( int your_int )
{
    //printf("\n\n\n\n AAAAAA 1 \n\n\n");
 std::stringstream stream;
    stream << std::hex << your_int;
    std::string result( stream.str() );
    string s = stream.str();
    if(s.size()==1) s = "0x0" + s; else s = "0x" + s;
  return s;
}

extern std::string IntToHex( unsigned short your_int )
{
    //printf("\n\n\n\n AAAAAA 2 \n\n\n");
 std::stringstream stream;
    stream << std::hex << your_int;
    std::string result( stream.str() );
    string s = stream.str();
    if(s.size()==1) s = "0x0" + s; else s = "0x" + s;
  return s;
}
extern std::string IntToHex( short your_int )
{
    //printf("\n\n\n\n AAAAAA 3 \n\n\n");
 std::stringstream stream;
    stream << std::hex << your_int;
    std::string result( stream.str() );
    string s = stream.str();
    if(s.size()==1) s = "0x0" + s; else s = "0x" + s;
  return s;
}

extern int WriteDataToMatterBuffer(uint8_t * buffer, unsigned int val, uint16_t matterDataType, uint16_t matterDataLength)
{
    //uint8_t valunit8;
    switch (matterDataType)
    {
    case ZCL_INT8U_ATTRIBUTE_TYPE:
        //valunit8 = (uint8_t)val;
        memcpy(buffer, &val, matterDataLength);
        return 0;
    
    default:
        return 1;
    }
    return 0;
}


extern int ConvertToInt(vector<unsigned char> a)
{
    long ld = a.size();
    if(ld == 4) return (a[0] << 24) | (a[1] << 16) | (a[2] << 16) | a[3];
    if(ld == 3) return (a[0] << 16) | (a[1] << 8) |  a[2];
    if(ld == 2) return (a[0] << 8)  |  a[1];
    if(ld==1) return a[0];
    return 0;
}
extern unsigned int ConvertToUnsignedInt(vector<unsigned char> a)
{
    long ld = a.size();
    if(ld == 4) return (a[0] << 24) | (a[1] << 16) | (a[2] << 16) | a[3];
    if(ld == 3) return (a[0] << 16) | (a[1] << 8) |  a[2];
    if(ld == 2) return (a[0] << 8)  |  a[1];
    if(ld == 1) return a[0];
    return 0;

}
extern short ConvertToShort(vector<unsigned char> a)
{
    long ld = a.size();
    if(ld ==0) return 0;
    if(ld == 1) return a[0];
    return (a[0] << 8)  |  a[1];
    //return 0;
}
extern unsigned short ConvertToUnsignedShort(vector<unsigned char> a)
{
    long ld = a.size();
    if(ld == 2) return (a[0] << 8)  |  a[1];
    if(ld == 1) return a[0];
    return 0;
}
extern char ConvertToChar(vector<unsigned char> a)
{
    if(a.size()==0) return 0;
    return (char)a[0];
}
extern unsigned char ConvertToUnsignedChar(vector<unsigned char> a)
{
    if(a.size()==0) return 0;
    return a[0];
}
extern string ConvertToString(vector<unsigned char> a)
{
    return string(a.begin(),a.end());
}
extern string ConvertEchonetValueToHexString(vector<unsigned char> a)
{
    string s="0x";
    char t[3];
    for(int i =0; i < (int)a.size(); i++)
    {
        sprintf(t,"%02x", a[i]);
        s+= string(t);
    }
    return s; 
}
extern string ConvertUnsignedValueToHexString(unsigned int a, bool isAddPrefix)
{
    string s="0x";
    if(!isAddPrefix) s="";
    char t[9];
    sprintf(t,"%04x", a);
    s+= string(t);
    return s; 
}


extern string ConvertUnsignedValueToHexString(unsigned short a, bool isAddPrefix)
{
    string s="0x";if(!isAddPrefix) s="";
    char t[5];
    sprintf(t,"%04x", a);
    s+= string(t);
    return s; 
}
extern string ConvertUnsignedValueToHexString(unsigned char a, bool isAddPrefix)
{
    string s="0x";if(!isAddPrefix) s="";
    char t[3];
    sprintf(t,"%02x", a);
    s+= string(t);
    return s; 
}

//Beautify the log
extern string GetMatterEndpointTypeName(MatterEchonetLITECombineEndpointType type)
{
    switch (type)
    {
    case MatterEchonetLITECombineEndpointType::UNKNOW: return "UNKNOW";
    case MatterEchonetLITECombineEndpointType::ONOFF_LIGHT: return "ONOFF LIGHT";
    case MatterEchonetLITECombineEndpointType::DIMMABLE_LIGHT: return "DIMMABLE LIGHT";
    case MatterEchonetLITECombineEndpointType::COLOR_TEMPERATURE_LIGHT: return "COLOR TEMPERATURE LIGHT";
    case MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR: return "TEMPERATURE SENSOR";
    case MatterEchonetLITECombineEndpointType::WINDOW_COVERING: return "WINDOW COVERING";
    case MatterEchonetLITECombineEndpointType::MODESELECT: return "MODESELECT";
    case MatterEchonetLITECombineEndpointType::WINDOW: return "WINDOW";
    case MatterEchonetLITECombineEndpointType::ILLUMINANCE_SENSOR: return "ILLUMINANCE SENSOR";
    case MatterEchonetLITECombineEndpointType::HUMIDITY_SENSOR: return "HUMIDITY SENSOR";
    case MatterEchonetLITECombineEndpointType::FLOW_SENSOR: return "FLOW SENSOR";
    case MatterEchonetLITECombineEndpointType::FLOW_SENSOR_for_ELECTRIC_ENERGY_SENSOR: return "FLOW_SENSOR_for_ELECTRIC_ENERGY_SENSOR";
    //case MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR_EMGERGENCY_BUTTON: return "OCCUPANCY_SENSOR_EMGERGENCY_BUTTON";
    case MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR: return "OCCUPANCY SENSOR";
    //case MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR_CO2: return "TEMPERATURE_SENSOR_CO2";
    case MatterEchonetLITECombineEndpointType::HEATING_COOLING_UNIT: return "HEATING COOLING UNIT";
    case MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH: return "ONOFF LIGHT SWITCH";
    case MatterEchonetLITECombineEndpointType::PRESSURE_SENSOR_for_STORAGE_BATTERY: return "PRESSURE SENSOR for STORAGE BATTERY";
    // case MatterEchonetLITECombineEndpointType::UNKNOW: return "UNKNOW";
    // case MatterEchonetLITECombineEndpointType::UNKNOW: return "UNKNOW";
    // case MatterEchonetLITECombineEndpointType::UNKNOW: return "UNKNOW";
    // case MatterEchonetLITECombineEndpointType::UNKNOW: return "UNKNOW";
    default: 
       char buf[16];
       sprintf(buf,"%d",(int)type);
    return "CANNOT_CONVERT_MATTER_DEVICE_ERROR " + string(buf);
    }
    
}

//Beautify the log
extern string GetEchonetEndpointTypeName(unsigned short echonetClassCode)
{
    switch (echonetClassCode)
    {
    case 0x0290: case 0x0291: return "General light";
    case 0x0011: return "Temperature sensor";
    case 0x0012: return "Humidity sensor";
    case 0x0025: return "Flow sensor";
    case 0x001D: return "Illuminance sensor";
    case 0x001B: return "CO2 sensor";
    case 0x0003: return "Emergency button";
    case 0x0007: return "Human detection sensor";
    case 0x0262: return "Curtain";
    case 0x0265: return "Window";
    case 0x0130: return "Home air conditioner";
    case 0x05fd: return "Switch";
    case 0x0022: return " Electric Sensor (Pressure sensor) ";
    case 0x027d: return " Storage battery (Pressure sensor)";
    default: 
       char buf[16];
       sprintf(buf,"%d",echonetClassCode);
    return "CANNOT_CONVERT_ECHONET_DEVICE_ERROR " + string(buf);

    }
}

extern MatterEchonetLITECombineEndpointType GetMatterEndpointTypeFromEchonetEndpointCode(void* echonetEndpoint_)
{
    EchonetEndpoint*ep =(EchonetEndpoint*)echonetEndpoint_;
    unsigned short echoClassCode= ep->echoClassCode;
    unsigned char echoInstanceCode = ep->instanceCode;
    shared_ptr<DeviceObject> openDevice = ep->GetDeviceObject();
    MatterEchonetLITECombineEndpointType type = MatterEchonetLITECombineEndpointType::UNKNOW;

    std::string address = ep->address;


    if(STATIC_CONFIG_ECHONET_CLASSCODE_FILTER >0)
    {
        if(echoClassCode!=STATIC_CONFIG_ECHONET_CLASSCODE_FILTER)
        {
            TimeManager::GetInstance()->RecordTime(TimeRecordType::PROCESSED_AN_ECHONET_ENDPOINT, echoClassCode, ep->instanceCode,0,0,0, (unsigned int) MatterEchonetLITECombineEndpointType::UNKNOW  );
            return MatterEchonetLITECombineEndpointType::UNKNOW;

        }
    }


    //Check if the current ip is blacklisted
    for(std::vector<std::pair<std::string,unsigned int>> ::iterator it = blackListEnpoints.begin();
    it!= blackListEnpoints.end(); it++ )
    {
        if(address.compare(it->first) == 0 
            && (((unsigned int)echoClassCode<<8) + echoInstanceCode ) == it->second
        ) 
        return MatterEchonetLITECombineEndpointType::UNKNOW;
    }

    //Check special endpoints in iHouse
    for(std::vector<std::pair<std::string,unsigned int>> ::iterator it = ihouseListWindowCoverings.begin();
    it!= ihouseListWindowCoverings.end(); it++ )
    {
        if(address.compare(it->first) == 0 
            && (((unsigned int)echoClassCode<<8) + echoInstanceCode ) == it->second
        ) 
        return MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH_IHOUSE;
        
    }

    //Check special endpoints in iHouse
    for(std::vector<std::pair<std::string,unsigned int>> ::iterator it = ihouseListWindows.begin();
    it!= ihouseListWindows.end(); it++ )
    {
        if(address.compare(it->first) == 0 
            && (((unsigned int)echoClassCode<<8) + echoInstanceCode ) == it->second
        ) 
        return MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH_IHOUSE;
    }


    
    // Convert EchonetLITE device id to Matter device ID
    

    switch (echoClassCode)
    {
    case 0x0290: case 0x0291:
        if(IsSupportedEPC(openDevice->getSetProperties(), 0xB0))
            {
                if(IsSupportedEPC(openDevice->getSetProperties(), 0xB1))
                    type= MatterEchonetLITECombineEndpointType::COLOR_TEMPERATURE_LIGHT;
                else
                    type= MatterEchonetLITECombineEndpointType::DIMMABLE_LIGHT;
            }
            else 
            {
                type= MatterEchonetLITECombineEndpointType::ONOFF_LIGHT;
            }
        break;
    case 0x0011: type= MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR; break;
     case 0x000D:  type= MatterEchonetLITECombineEndpointType::ILLUMINANCE_SENSOR; break;

    ////////case 0x001B: return MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR_CO2; // ====> TEMPERATURE_SENSOR
    case 0x001B: type= MatterEchonetLITECombineEndpointType::TEMPERATURE_SENSOR_CO2; break;// ====> TEMPERATURE_SENSOR
    ////////case 0x0003: return MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR_EMGERGENCY_BUTTON;
     case 0x0003: type= MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR_EMGERGENCY_BUTTON; break;//EMGERGENCY_BUTTON
     case 0x0007: type= MatterEchonetLITECombineEndpointType::OCCUPANCY_SENSOR;break;
     case 0x0012: type= MatterEchonetLITECombineEndpointType::HUMIDITY_SENSOR;break;
    case 0x0025: type= MatterEchonetLITECombineEndpointType::FLOW_SENSOR ;break;
    case 0x0022: type= MatterEchonetLITECombineEndpointType::FLOW_SENSOR_for_ELECTRIC_ENERGY_SENSOR ;break;
    
    case 0x0262: 
        type= MatterEchonetLITECombineEndpointType::WINDOW_COVERING; //01
        break;
    break;
    case 0x0265:
        type= MatterEchonetLITECombineEndpointType::WINDOW; //03
        break;
    break; 
    case 0x0130: type= MatterEchonetLITECombineEndpointType::HEATING_COOLING_UNIT;
        break;
    case 0x05fd: type= MatterEchonetLITECombineEndpointType::ONOFF_LIGHT_SWITCH;
       break;

    // case 0x027d: type= MatterEchonetLITECombineEndpointType::PRESSURE_SENSOR_for_STORAGE_BATTERY ;break;

    default:
        break;
    }

    TimeManager::GetInstance()->RecordTime(TimeRecordType::PROCESSED_AN_ECHONET_ENDPOINT, echoClassCode, ep->instanceCode,0,0,0, (unsigned int) type  );
    return type;
}
extern int ProceseParameters(int argc, char * argv[])
{
    int processedCount =0;
    for(int i =1; i < argc; i++)
    {
        bool isOK = false;
        if(strcmp(argv[i],"-asyncread")==0)
        {
            isOK = true;
            processedCount++;
            STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ = !STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_READ;
        }
        else if(strcmp(argv[i],"-asyncwrite")==0)
        {
            isOK = true;
            processedCount++;
            STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE = !STATIC_CONFIG_IS_ASNCHRONOUS_COMISSION_WRITE;
        }
        else if(strcmp(argv[i],"-get_interval")==0)
        {
            isOK = true;
            processedCount+=2;
            i++;
            STATIC_CONFIG_REQUEST_GET_INTERVAL = atoi(argv[i]);
        }
        else if(strcmp(argv[i],"-classcode_filter")==0)
        {
            isOK = true;
            processedCount+=2;
            i++;
            STATIC_CONFIG_ECHONET_CLASSCODE_FILTER = atoi(argv[i]);
        }
        if(!isOK)
        {
            printf("[WARNING] Cannot understand the parameter: %s \n", argv[i]);
        }
        //extern int STATIC_CONFIG_REQUEST_GET_INTERVAL;
    }
    return processedCount;
}

#endif