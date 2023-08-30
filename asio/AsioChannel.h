#pragma once

#include "AsioCommon.h"
#include <memory>

class AsioDriver;

class AsioChannel
{
public:
    AsioDriver* parent;
    bool is_input;
    int channel_index;
    int buffer_size;
    ASIOSampleType sample_type;
    SampleBuffer buffers[2];
};