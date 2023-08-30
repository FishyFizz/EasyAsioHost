#pragma once

#include <windows.h>

#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "iasiodrv.h"
#include "asiolist.h"

typedef void* SampleBuffer;

// Stores information used to create COM object of ASIO drivers.
class AsioDriverInitInfo
{
public:
    int id;
    CLSID clsid;
    std::string name;
    std::string dllpath;

    AsioDriverInitInfo() {}

    AsioDriverInitInfo(const asiodrvstruct &src)
    {
        id = src.drvID;
        memcpy(&clsid, &src.clsid, sizeof(CLSID));
        name = src.drvname;
        dllpath = src.dllpath;
    }
};

class AsioBufferSizeRequirements
{
public:
    long min_size, max_size, preferred_size, granularity; 
};

class AsioChannelInfo
{
public:
    long index;
    bool is_input;
    bool is_active;
    long channel_group;
    ASIOSampleType sample_type;
    std::string name;

    AsioChannelInfo(){}
    AsioChannelInfo(const ASIOChannelInfo& src)
    {
        index = src.channel;
        is_input = src.isInput;
        is_active = src.isActive;
        channel_group = src.channelGroup;
        sample_type = src.type;
        name = src.name;
    }
};

class AsioClockSourceInfo
{
public:
    long index;
    long associated_channel;
    long associated_group;
    bool is_current_source;
    std::string name;

    AsioClockSourceInfo(){};
    AsioClockSourceInfo(const ASIOClockSource& src){
        index = src.index;
        associated_channel = src.associatedChannel;
        associated_group = src.associatedGroup;
        is_current_source = src.isCurrentSource;
        name = src.name;
    }
};

class AsioConfiguration
{
public:
    AsioBufferSizeRequirements buffer_requirements;
    std::vector<AsioChannelInfo> input_channels, output_channels;
    std::vector<ASIOSampleRate> supported_samplerates;
    std::vector<AsioClockSourceInfo> clock_sources;
};

class AsioLatencyPair
{
public:
    long input_latency;
    long output_latency;
};

extern std::vector<ASIOSampleRate> common_samplerates;
extern std::map<int, std::string> asio_sample_type_to_text;
extern const char* asio_message_to_text[];