//
// Created by Fizz on 8/31/2023.
//

#include "AsioDriverManager.h"
#include "AsioDriver.h"
#include "AsioChannel.h"
#include "SampleFormatConverter.h"

#include "log.h"

#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

double constexpr PI = 3.14159265359;
double constexpr FREQ = 440;

float* global_buffer;
ASIOSampleType global_sample_type;
size_t global_sample_rate, global_buffer_size;
vector<AsioChannel> global_asio_channels;

void GlobalOnAsioBufferSwitch(long buffer_index, size_t sample_position, size_t timestamp)
{
    //Simple tone. For every channel, output a sine wave with frequency FREQ*(idx + 1), amplitude 1/(idx + 1).
    for(int i=0 ;i<global_asio_channels.size();i++)
    {
        for (size_t j = 0; j < global_buffer_size; j++)
            global_buffer[j] = sin(PI * 2 * FREQ* (float)(i+1) * (sample_position + j) / global_sample_rate) / (float)(i+1);
        SampleFormatConverter(ASIOSTFloat32LSB ,global_sample_type).ConvertBlock(global_buffer, global_asio_channels[i].buffers[buffer_index], global_buffer_size);
    }
}

void CallSequenceDemo() {
    assert(false);

    int driver_idx;
    HWND parent_hwnd;
    int channel_idx;
    int sample_rate;
    int clock_idx;
    int buffer_size;

    auto asio_driver_init_info = AsioDriverManager::Instance().driver_init_info;
    auto driver = AsioDriverManager::Instance().OpenAsioDriver(driver_idx);
    driver->Init(parent_hwnd);

    driver->driver->controlPanel();

    bool can_sample_rate = driver->CanSampleRate(sample_rate);
    driver->SetSampleRate(sample_rate);

    vector<AsioClockSourceInfo> clocks = driver->GetClockSources();
    driver->SetClockSource(clock_idx);

    AsioBufferSizeRequirements buf_req = driver->GetBufferSizeRequirements();
    // buffer_size = min_size +  n * granularity; min_size <= buffer_size <= max_size;
    buffer_size = buf_req.preferred_size;

    AsioConfiguration config = driver->GetConfiguration();
    vector<AsioChannelInfo> in_chn = config.input_channels;
    vector<AsioChannelInfo> out_chn = config.input_channels;

    vector<int> input_channel_to_activate = {1, 3, 5};
    vector<int> output_channel_to_activate = {3, 4, 7};
    //Activating all channels and sending silence to "unused" channels is suggested.

    vector<AsioChannel> asio_channels = driver->CreateBuffers(
            input_channel_to_activate,
            output_channel_to_activate,
            buffer_size);
    //asio_channels[0] <--> input_channel_to_activate[0]
    // ... ...
    //asio_channels[input_channel_to_activate.size()] <--> output_channel_to_activate[0]
    // ... ...

    driver->Start();

    //Implement your
    //    void GlobalOnAsioBufferSwitch(long buffer_index, size_t sample_position, size_t timestamp)
    //Feed audio data into asio_channels[channel_idx].buffer[buffer_idx]

    driver->Stop();
    driver->DisposeBuffers();
    AsioDriverManager::Instance().CloseAsioDriver(driver_idx);
}

int main(int args, char **argv)
{
    auto asio_driver_init_info = AsioDriverManager::Instance().driver_init_info;

    int selected_driver;
    {
        int i = 0;
        for (AsioDriverInitInfo info: AsioDriverManager::Instance().driver_init_info)
            cout << i++ << "." << info.name << endl;
        cout << GREEN << "select driver:" << NOCOLOR << endl;
        cin >> selected_driver;
        cout << "selected: " << AsioDriverManager::Instance().driver_init_info[selected_driver].name << endl;
    }

    auto driver = AsioDriverManager::Instance().OpenAsioDriver(selected_driver);
    driver->Init(GetConsoleWindow());

    driver->driver->controlPanel();
    {
        cout <<GREEN<< "Please adjust ASIO settings now. [PRESS ENTER]" <<NOCOLOR<< endl;
        string __notcare;
        getline(cin,__notcare);
        getline(cin,__notcare);
    }

    AsioConfiguration config = driver->GetConfiguration();
    global_sample_rate = driver->GetSampleRate();
    global_buffer_size = config.buffer_requirements.preferred_size;

    {
        vector<int> channels;
        for (int i = 0; i < config.output_channels.size(); i++)
            channels.push_back(i);
    }

    global_asio_channels = driver->CreateBuffers({}, {0,1,2,3,4,5,6,7}, global_buffer_size);
    global_sample_type = global_asio_channels[0].sample_type; //Assume every channel has the same sample type. This is the normal case.

    global_buffer = new float[global_buffer_size];

    driver->Start();
    //Driver calls GlobalOnAsioBufferSwitch on a separated thread. Fill AsioChannel.buffer there.
    Sleep(5000);

    driver->Stop();
    driver->DisposeBuffers();
    AsioDriverManager::Instance().CloseAsioDriver(selected_driver);
    return 0;
}