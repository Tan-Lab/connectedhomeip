 /*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This class store the the struct for the mapping rule
 */
#pragma once
#ifndef AttributePropertyAdapter_H
#define AttributePropertyAdapter_H
#include "../Utils.h"
#include <json/json.h>

class AttributePropertyAdapter {
    public:
    chip::ClusterId matterClusterId;
    chip::AttributeId matterAttributeId;
    unsigned char echonetPropertyId;
    uint8_t * defaultValue;
    //EchonetDataTypeAndLength echonetDataType;
    uint16_t matterDataType; uint16_t echonetDataType;
    uint16_t matterDataLength; uint16_t echonetDataLength;
    float valueMultiplierForEchonetValue=0;
    map<vector<unsigned char>,vector<unsigned char>> mapValueEchonet2Matter;
    map<vector<unsigned char>,vector<unsigned char>> mapValueMatter2Echonet;

    string unit;
    AttributePropertyAdapter(){matterDataLength=1;};
    AttributePropertyAdapter( chip::ClusterId matterClusterId_,  chip::AttributeId matterAttributeId_,unsigned char echonetPropertyId_, 
      uint16_t matterDataType_ = ZAP_TYPE(INT8U),uint16_t matterDataLength_=1, uint16_t echonetDataType_ =ZAP_TYPE(INT8U), uint16_t echonetDataLength_=1  )
    {
        matterClusterId = matterClusterId_;
        matterAttributeId = matterAttributeId_;
        echonetPropertyId = echonetPropertyId_;
        matterDataType = matterDataType_;
        matterDataLength = matterDataLength_;
        echonetDataType = echonetDataType_;
        echonetDataLength = echonetDataLength_;

        

    };
    void SetDefaultValue(const char* str)
    {
        matterDataType = ZAP_TYPE(CHAR_STRING);
        int len= (int)strlen(str);
        defaultValue = (uint8_t *)malloc(len+1);
        memcpy(defaultValue, str, len);
    }
    void AddPairOfmapValue(vector<unsigned char> echonetValue, vector<unsigned char> matterValue)
    {
        mapValueEchonet2Matter.insert({echonetValue,matterValue});
        mapValueMatter2Echonet.insert({matterValue,echonetValue});
    }
    ~AttributePropertyAdapter()
    {
        free(defaultValue);
    }
    Json::Value ToJson();

};








#endif
