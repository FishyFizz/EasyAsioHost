#pragma once

#include "AsioCommon.h"
#include "AsioChannel.h"

#include <cstdio>

class AsioDriver
{
public:
    IASIO* driver;
    std::vector<AsioChannel> channels;

    bool Init(void* hMainWnd = nullptr);

    //Functions to retrieve ASIO configurations before preparing buffer and starting stream.
    bool CanSampleRate(ASIOSampleRate smpr){return driver->canSampleRate(smpr) == S_OK;}
    std::vector<ASIOSampleRate> GetSupportedSampleRates(std::vector<ASIOSampleRate> samplerates_to_test = common_samplerates);
    std::vector<AsioChannelInfo> GetInputChannelConfigurations();
    std::vector<AsioChannelInfo> GetOutputChannelConfigurations();
    AsioChannelInfo GetChannelInfo(bool isInput, int channel);
    AsioBufferSizeRequirements GetBufferSizeRequirements();
    AsioConfiguration GetConfiguration();

    //Sample Rate control
    ASIOSampleRate GetSampleRate(){ASIOSampleRate tmp; driver->getSampleRate(&tmp); return tmp;}
    void SetSampleRate(ASIOSampleRate sr){driver->setSampleRate(sr);}

    //Clock Sources control
    std::vector<AsioClockSourceInfo> GetClockSources();
    void SetClockSource(long index){driver->setClockSource(index);}

    //ASIO Driver feature tests
    bool DriverSupportsTimeInfo(){return driver->future(kAsioCanTimeInfo, nullptr);}
    bool DriverSupportsTimeCode(){return driver->future(kAsioCanTimeCode, nullptr);}
    bool DriverSupportsInputMonitor(){return driver->future(kAsioCanInputMonitor, nullptr);}
    bool DriverSupportsReportOverload(){return driver->future(kAsioCanReportOverload, nullptr);}
    bool DriverSupportsGetInternalBufferSamples(){return driver->future(kAsioGetInternalBufferSamples, nullptr);}

    //Preparing driver
    std::vector<AsioChannel> CreateBuffers(std::vector<int> target_input_channels, std::vector<int> target_output_channels, long buffer_size);
    AsioLatencyPair GetLatencies()
    {
        AsioLatencyPair result;
        driver->getLatencies(&result.input_latency, &result.output_latency);
        return result;
    }

    //Streaming control
    ASIOError Start();
    ASIOError Stop(){return driver->stop();}

    std::string LastError(){char buf[256]; driver->getErrorMessage(buf); return buf;}

    //ASIO Callback redirection.
    static AsioDriver* instance;
    static void _OnBufferSwitch(long buffer_index, ASIOBool direct_process){instance->OnBufferSwitch(buffer_index, direct_process);}
    static ASIOTime* _OnBufferSwitchTimeInfo(ASIOTime* params, long buffer_index, ASIOBool direct_process){return instance->OnBufferSwitchTimeInfo(params, buffer_index, direct_process);}
    static void _OnSampleRateChanged(ASIOSampleRate smpr){instance->OnSampleRateChanged(smpr);}
    static long _OnAsioMessage(long selector, long value, void* message, double* opt){return instance->OnAsioMessage(selector, value, message, opt);}

    //ASIO Callbacks
    void OnBufferSwitch(long buffer_index, ASIOBool direct_process);
    ASIOTime* OnBufferSwitchTimeInfo(ASIOTime* params, long buffer_index, ASIOBool direct_process);
    void OnSampleRateChanged(ASIOSampleRate smpr);
    long OnAsioMessage(long selector, long value, void* message, double* opt);

    //ASIO Message handlers
    void OnResetRequest(){}
    void OnBufferSizeChange(){}
    void OnResyncRequest(){}
    void OnLatenciesChanged(){}
    void OnOverload(){}

    //Realtime querys
    struct AsioPosition
    {
        uint64_t sample_pos;
        uint64_t timestamp;
    };

    AsioPosition GetPosition()
    {
        ASIOSamples sample_pos;
        ASIOTimeStamp timestamp;
        driver->getSamplePosition(&sample_pos, &timestamp);
        uint64_t sample_pos_64 = ((uint64_t)sample_pos.hi << 32) + (uint64_t)sample_pos.lo;
        uint64_t timestamp_64 = ((uint64_t)timestamp.hi << 32) + (uint64_t)timestamp.lo;
        return {sample_pos_64, timestamp_64};
    }

    //Cleaup
    ASIOError DisposeBuffers(){return driver->disposeBuffers();}
    ULONG Release(){return driver->Release();}
};