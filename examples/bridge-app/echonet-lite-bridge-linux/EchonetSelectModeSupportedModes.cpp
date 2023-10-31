 /*
 * Author: nmtoan91
 * Date:   2023-10-02
 *
 * Description:
 * This file defines some tipical rules to convert echonetLITE options into Matter's Mode Select configs.
 */

#include <app/util/config.h>
#include <EchonetSelectModeSupportedModes.h>


using namespace std;
using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ModeSelect;
using chip::Protocols::InteractionModel::Status;

using ModeOptionStructType = Structs::ModeOptionStruct::Type;
using SemanticTag          = Structs::SemanticTagStruct::Type;
template <typename T>
using List               = app::DataModel::List<T>;
namespace {
Structs::ModeOptionStruct::Type buildModeOptionStruct(const char * label, uint8_t mode,
                                                      const List<const SemanticTag> & semanticTags)
{
    Structs::ModeOptionStruct::Type option;
    option.label        = CharSpan::fromCharString(label);
    option.mode         = mode;
    option.semanticTags = semanticTags;
    return option;
}
} // namespace

SemanticTag semanticTagsBlack[]     = { {  .value = 10 }};
SemanticTag semanticTagsCappucino[] = { { } };
SemanticTag semanticTagsEspresso[]  = { { .value = 3 } };

// TODO: Configure your options for each endpoint
const ModeOptionStructType StaticSupportedModesManager::echonetOpenCloseStopOptions[] = {
    buildModeOptionStruct("Open", 65, List<const SemanticTag>(semanticTagsBlack)),
    buildModeOptionStruct("Close", 66, List<const SemanticTag>(semanticTagsCappucino)),
    buildModeOptionStruct("Stop", 67, List<const SemanticTag>(semanticTagsEspresso))
};
const ModeOptionStructType StaticSupportedModesManager::echonetLightColorOptions[] = {
    buildModeOptionStruct("Incandescent lamp color", 65, List<const SemanticTag>(semanticTagsBlack)),
    buildModeOptionStruct("White", 66, List<const SemanticTag>(semanticTagsCappucino)),
    buildModeOptionStruct("Daylight white", 67, List<const SemanticTag>(semanticTagsEspresso)),
    buildModeOptionStruct("Daylight color", 68, List<const SemanticTag>(semanticTagsEspresso)),
    buildModeOptionStruct("Other", 64, List<const SemanticTag>(semanticTagsEspresso))
};
const ModeOptionStructType StaticSupportedModesManager::echonetPowerSavingOptions[] = {
    buildModeOptionStruct("Power-saving mode", 0x41, List<const SemanticTag>(semanticTagsBlack)),
    buildModeOptionStruct("Normal mode", 0x42, List<const SemanticTag>(semanticTagsCappucino)),
};



StaticSupportedModesManager StaticSupportedModesManager::instance = StaticSupportedModesManager();

SupportedModesManager::ModeOptionsProvider StaticSupportedModesManager::getModeOptionsProvider(EndpointId endpointId) const
{
    for (auto & endpointSpanPair : echonetSupportedOptionsByEndpoint)
    {
        if (endpointSpanPair.mEndpointId == endpointId)
        {
            return ModeOptionsProvider(endpointSpanPair.mSpan.data(), endpointSpanPair.mSpan.end());
        }
    }
    return ModeOptionsProvider(nullptr, nullptr);
}

Status StaticSupportedModesManager::getModeOptionByMode(unsigned short endpointId, unsigned char mode,
                                                        const ModeOptionStructType ** dataPtr) const
{
    printf("\n\n getModeOptionByMode %d \n\n", endpointId );
    auto modeOptionsProvider = this->getModeOptionsProvider(endpointId);
    if (modeOptionsProvider.begin() == nullptr)
    {
        return Status::UnsupportedCluster;
    }
    auto * begin = this->getModeOptionsProvider(endpointId).begin();
    auto * end   = this->getModeOptionsProvider(endpointId).end();

    for (auto * it = begin; it != end; ++it)
    {
        auto & modeOption = *it;
        if (modeOption.mode == mode)
        {
            *dataPtr = &modeOption;
            return Status::Success;
        }
    }
    ChipLogProgress(Zcl, "Cannot find the mode %u", mode);
    return Status::InvalidCommand;
}

const ModeSelect::SupportedModesManager * ModeSelect::getSupportedModesManager()
{
    return &StaticSupportedModesManager::instance;
}

void StaticSupportedModesManager::AddEchonetSupportedOptionsByEndpoint(EndpointId endpointId,EchonetOptionType echonetOptopnType)
{
    switch (echonetOptopnType)
    {
    case EchonetOptionType::OPEN_CLOSE_STOP_41_42_43:
    this->echonetSupportedOptionsByEndpoint.push_back(EndpointSpanPair(endpointId,Span<const ModeOptionStructType>(echonetOpenCloseStopOptions) ));
        break;
    case EchonetOptionType::LIGHT_COLOR_SETTING_LAMP_WHITE_DAYLIGHTWHITE_DAYLIGHCOLOR_OTHER_41_42_43_44_40:
    this->echonetSupportedOptionsByEndpoint.push_back(EndpointSpanPair(endpointId,Span<const ModeOptionStructType>(echonetLightColorOptions) ));
        break;
    case EchonetOptionType::POWER_SAVING_OPERATION_SETTING_41_42:
    this->echonetSupportedOptionsByEndpoint.push_back(EndpointSpanPair(endpointId,Span<const ModeOptionStructType>(echonetPowerSavingOptions) ));
        break;
    case EchonetOptionType::OPTION_UNKNOW:
    break;
    default:
    this->echonetSupportedOptionsByEndpoint.push_back(EndpointSpanPair(endpointId,Span<const ModeOptionStructType>(echonetOpenCloseStopOptions) ));
        break;
    }
}
StaticSupportedModesManager::StaticSupportedModesManager()
{
    //AddEchonetSupportedOptionsByEndpoint(3, EchonetOptionType::OPEN_CLOSE_STOP_41_42_43); 
}