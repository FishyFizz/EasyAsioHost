#include "AsioDriver.h"
#include "AsioChannel.h"
#include "log.h"

AsioDriver* AsioDriver::instance = nullptr;

bool AsioDriver::Init(void *hMainWnd) //Test OK
{
    dbginfo("ASIO", "ASIO Init...");
    channels = std::vector<AsioChannel>(0);
    if (!driver->init(hMainWnd))
    {
        dbgerror("ASIO", "Failed to initialize ASIO driver: %s", LastError().c_str());
        return false;
    }
    dbginfo("ASIO", "Initialized.");
    return true;
}

std::vector<ASIOSampleRate> AsioDriver::GetSupportedSampleRates(std::vector<ASIOSampleRate> samplerates_to_test) //Test OK
{
    dbginfo("ASIO", "Testing for supported sample rates...");
    std::vector<ASIOSampleRate> result;
    for (ASIOSampleRate smpr : samplerates_to_test)
    {   
        if (CanSampleRate(smpr)){
            result.push_back(smpr);
            dbgunimportant("ASIO", "%d is supported", (int)smpr);
        }
        else{
            dbgunimportant("ASIO", "%d is " RED "not" GRAY " supported", (int)smpr);
        }
    }
    return result;
}

std::vector<AsioChannelInfo> AsioDriver::GetInputChannelConfigurations() //Test OK
{
    dbginfo("ASIO", "Getting input channel configurations...");
    std::vector<AsioChannelInfo> result;

    long input_channel_count, output_channel_count;
    driver->getChannels(&input_channel_count, &output_channel_count);

    for (long i = 0; i < input_channel_count; i++)
    {
        ASIOChannelInfo tmp;
        tmp.isInput = true;
        tmp.channel = i;
        driver->getChannelInfo(&tmp);
        result.push_back(tmp);
        dbgunimportant("ASIO", "Input Channel %d (\"%s\"): channel_group=%d, is_active=%d, sample_type=%s",
             i, tmp.name, tmp.channelGroup, tmp.isActive, asio_sample_type_to_text[tmp.type].c_str());
    }

    return result;
}

std::vector<AsioChannelInfo> AsioDriver::GetOutputChannelConfigurations() //Test OK
{
    dbginfo("ASIO", "Getting output channel configurations...");
    std::vector<AsioChannelInfo> result;

    long input_channel_count, output_channel_count;
    driver->getChannels(&input_channel_count, &output_channel_count);

    for (long i = 0; i < output_channel_count; i++)
    {
        ASIOChannelInfo tmp;
        tmp.isInput = false;
        tmp.channel = i;
        driver->getChannelInfo(&tmp);
        result.push_back(tmp);
        dbgunimportant("ASIO", "Output Channel %d (\"%s\"): channel_group=%d, is_active=%d, sample_type=%s",
             i, tmp.name, tmp.channelGroup, tmp.isActive, asio_sample_type_to_text[tmp.type].c_str());
    }

    return result;
}

AsioChannelInfo AsioDriver::GetChannelInfo(bool isInput, int channel)
{
    ASIOChannelInfo tmp;
    tmp.isInput = true;
    tmp.channel = channel;
    driver->getChannelInfo(&tmp);
    return tmp;
}

AsioBufferSizeRequirements AsioDriver::GetBufferSizeRequirements() //Test OK
{
    AsioBufferSizeRequirements result;
    driver->getBufferSize(&result.min_size, &result.max_size, &result.preferred_size, &result.granularity);
    dbginfo("ASIO", "Got buffer size requirements: min=%d, max=%d, prefer=%d, granularity=%d", 
        result.min_size, result.max_size, result.preferred_size, result.granularity);
    return result;
}

AsioConfiguration AsioDriver::GetConfiguration() //Test OK
{
    AsioConfiguration result;
    result.supported_samplerates = GetSupportedSampleRates();
    result.input_channels = GetInputChannelConfigurations();
    result.output_channels = GetOutputChannelConfigurations();
    result.buffer_requirements = GetBufferSizeRequirements();
    result.clock_sources = GetClockSources();
    return result;
}

std::vector<AsioChannel> AsioDriver::CreateBuffers(std::vector<int> target_input_channels, std::vector<int> target_output_channels, long buffer_size)
{
    dbginfo("ASIO", "Try creating ASIO buffers, size=%d", buffer_size);
    std::vector<ASIOBufferInfo> buffer_infos(target_input_channels.size() + target_output_channels.size());

    int index = 0;
    for (int ch_num : target_input_channels)
    {
        buffer_infos[index++] = {true, ch_num, {0, 0}};
        dbgunimportant("ASIO", "selected input channel %d", ch_num);
    }
    for (int ch_num : target_output_channels)
    {
        buffer_infos[index++] = {false, ch_num, {0, 0}};
        dbgunimportant("ASIO", "selected output channel %d", ch_num);
    }

    //TODO: *cb leaks.
    ASIOCallbacks *cb = new ASIOCallbacks;
    *cb = ASIOCallbacks {
        _OnBufferSwitch,
        _OnSampleRateChanged,
        _OnAsioMessage,
        _OnBufferSwitchTimeInfo};
    instance = this;
    if (driver->createBuffers(&buffer_infos[0], buffer_infos.size(), buffer_size, cb) != S_OK)
    {
        dbgerror("ASIO","Failed to create buffers:%s", LastError().c_str());
        return std::vector<AsioChannel>();
    }
    
    dbginfo("ASIO", "Buffers created");
    index = 0;
    std::vector<AsioChannel> result;
    for (int ch_num : target_input_channels)
    {
        AsioChannel ch;
        ch.parent = this;
        ch.is_input = true;
        ch.channel_index = buffer_infos[index].channelNum;
        ch.buffer_size = buffer_size;
        ch.buffers[0] = buffer_infos[index].buffers[0];
        ch.buffers[1] = buffer_infos[index].buffers[1];
        ch.sample_type = GetChannelInfo(true, ch_num).sample_type;
        dbgunimportant("ASIO", "input channel %d, buffer located at %p / %p", ch_num, ch.buffers[0], ch.buffers[1]);
        result.push_back(ch);
        index++;
    }
    for (int ch_num : target_output_channels)
    {
        AsioChannel ch;
        ch.parent = this;
        ch.is_input = false;
        ch.channel_index = buffer_infos[index].channelNum;
        ch.buffer_size = buffer_size;
        ch.buffers[0] = buffer_infos[index].buffers[0];
        ch.buffers[1] = buffer_infos[index].buffers[1];
        ch.sample_type = GetChannelInfo(false, ch_num).sample_type;
        dbgunimportant("ASIO", "output channel %d, buffer located at %p / %p", ch_num, ch.buffers[0], ch.buffers[1]);
        result.push_back(ch);
        index++;
    }
    channels = result;
    return result;
}

std::vector<AsioClockSourceInfo> AsioDriver::GetClockSources()
{
    dbginfo("ASIO", "Getting clock sources...");
    ASIOClockSource clksrcs[8];
    long clksrcs_count = 8;
    driver->getClockSources(clksrcs, &clksrcs_count);

    std::vector<AsioClockSourceInfo> result(clksrcs_count);
    for(int i=0; i<clksrcs_count; i++){
        result[i] = AsioClockSourceInfo(clksrcs[i]);
        dbgunimportant("ASIO", "Clock %d (\"%s\"): %s associated_grp=%d, associated_channel=%d", 
            i, result[i].name.c_str(), result[i].is_current_source?"" GREEN "CURRENT" NOCOLOR "":"", result[i].associated_group, result[i].associated_channel);
    }
    return result;
}

extern void GlobalOnAsioBufferSwitch(long buffer_index, size_t sample_position, size_t timestamp);

void AsioDriver::OnBufferSwitch(long buffer_index, ASIOBool direct_process)
{
    ASIOSamples sample_pos;
    ASIOTimeStamp timestamp;
    driver->getSamplePosition(&sample_pos, &timestamp);
    uint64_t sample_pos_64 = ((uint64_t)sample_pos.hi << 32) + (uint64_t)sample_pos.lo;
    uint64_t timestamp_64 = ((uint64_t)timestamp.hi << 32) + (uint64_t)timestamp.lo;

    //  dbginfo("ASIO", "Buffer switch: buffer=%d, direct=%s, samplepos=%d, systemtime=%d",
    //      buffer_index, direct_process ? "yes" : "no", sample_pos_64, timestamp_64);

    GlobalOnAsioBufferSwitch(buffer_index, ((uint64_t)sample_pos.hi << 32) + (uint64_t)sample_pos.lo, ((uint64_t)timestamp.hi<<32)+ (uint64_t)timestamp.lo);
}

ASIOTime* AsioDriver::OnBufferSwitchTimeInfo(ASIOTime* params, long buffer_index, ASIOBool direct_process)
{
    ASIOSamples sample_pos = params->timeInfo.samplePosition;
    ASIOTimeStamp timestamp = params->timeInfo.systemTime;

    uint64_t sample_pos_64 = ((uint64_t)sample_pos.hi << 32) + (uint64_t)sample_pos.lo;
    uint64_t timestamp_64 = ((uint64_t)timestamp.hi << 32) + (uint64_t)timestamp.lo;

    //  dbginfo("ASIO", "Buffer switch +TimeInfo: buffer=%d, direct=%s, samplepos=%d, sr=%d, systemtime=%d, speed=%f",
    //      buffer_index, direct_process ? "yes" : "no", sample_pos_64, params->timeInfo.sampleRate, timestamp_64, params->timeInfo.speed);

    GlobalOnAsioBufferSwitch(buffer_index, sample_pos_64, timestamp_64);

    return params;
}


long AsioDriver::OnAsioMessage(long selector, long value, void* message, double* opt)
{
    dbginfo("ASIO", "ASIO Message: selector=%s, value=%d, message=%x, opt=%f", asio_message_to_text[selector], value, message, opt?(*opt):0);
    switch(selector)
    {
    case kAsioSelectorSupported:
        if(value == kAsioSelectorSupported || value == kAsioEngineVersion) return 1; //supported
        else return 0; //no other selector is supported.
        break;
    case kAsioEngineVersion:
        return 2; //ASIO v2
        break;
    case kAsioSupportsTimeInfo:
        return 1; //supported
        break;
    default: dbgwarning("ASIO", "Unimplemented ASIO Message"); return -1;
    }
}

void AsioDriver::OnSampleRateChanged(ASIOSampleRate smpr)
{
    dbgwarning("ASIO", "ASIO Samplerate changed to %d.", smpr);
    dbgwarning("ASIO", "OnSampleRateChanged unimplemented.");
}

ASIOError AsioDriver::Start(){
    dbginfo("ASIO","ASIO Start...");
    ASIOError err = driver->start();
    if(err != S_OK)
        dbgerror("ASIO","Failed to start:%s", LastError().c_str());
    return err;
}