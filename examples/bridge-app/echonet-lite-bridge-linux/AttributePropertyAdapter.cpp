#ifndef AttributePropertyAdapter_CPP
#define AttributePropertyAdapter_CPP
#include "AttributePropertyAdapter.h"


Json::Value AttributePropertyAdapter::ToJson()
{
    Json::Value json;
    //json["test"] = "ok";
    json["matterClusterId"] = matterClusterId;
    json["matterAttributeId"] = matterAttributeId;
    json["epc"] = echonetPropertyId;
    json["echonetDataType"] = echonetDataType;
    json["echonetDataType_String"] =  Map_ZCL_DATA_TYPE_TO_NAME[(unsigned char)echonetDataType];
    json["echonetDataLength"] = echonetDataLength;
    json["matterDataType"] = matterDataType;
    json["matterDataType_String"] = Map_ZCL_DATA_TYPE_TO_NAME[(unsigned char)matterDataType];
    json["matterDataLength"] = matterDataLength;
    json["valueMultiplierForEchonetValue"] = valueMultiplierForEchonetValue;
    Json::Value mapValueEchonet2MatterJson;
    for(map<vector<unsigned char>,vector<unsigned char>>::iterator it = mapValueEchonet2Matter.begin(); it!= mapValueEchonet2Matter.end(); it++)
    {
         mapValueEchonet2MatterJson[ConvertEchonetValueToHexString(it->first)] = ConvertEchonetValueToHexString(it->second);
    }
    json["mapValueEchonet2Matter"] = mapValueEchonet2MatterJson;


    Json::Value mapValueMatter2EchonetJson;
    for(map<vector<unsigned char>,vector<unsigned char>>::iterator it = mapValueMatter2Echonet.begin(); it!= mapValueMatter2Echonet.end(); it++)
    {
         mapValueMatter2EchonetJson[ConvertEchonetValueToHexString(it->first)] = ConvertEchonetValueToHexString(it->second);
    }
    json["mapValueMatter2Echonet"] = mapValueMatter2EchonetJson;


    return json;
}







#endif
