#include "AsioCommon.h"

std::vector<ASIOSampleRate> common_samplerates = {44100, 48000, 96000, 192000};

std::map<int, std::string> asio_sample_type_to_text = {
    {0, "Int16MSB"},
    {1, "Int24MSB"},
    {2, "Int32MSB"},
    {3, "Float32MSB"},
    {4, "Float64MSB"},

    {8, "Int32MSB16"},
    {9, "Int32MSB18"},
    {10, "Int32MSB20"},
    {11, "Int32MSB24"},

    {16, "Int16LSB"},
    {17, "Int24LSB"},
    {18, "Int32LSB"},
    {19, "Float32LSB"},
    {20, "Float64LSB"},

    {24, "Int32LSB16"},
    {25, "Int32LSB18"},
    {26, "Int32LSB20"},
    {27, "Int32LSB24"},

    {32, "DSDInt8LSB1"},
    {33, "DSDInt8MSB1"},
    {40, "DSDInt8NER8"},
};

const char* asio_message_to_text[] = {
    "",
    "kAsioSelectorSupported",
    "kAsioEngineVersion",
    "kAsioResetRequest",
    "kAsioBufferSizeChange",
    "kAsioResyncRequest",
    "kAsioLatenciesChanged",
    "kAsioSupportsTimeInfo",
    "kAsioSupportsTimeCode",
    "kAsioMMCCommand",
    "kAsioSupportsInputMonitor",
    "kAsioSupportsInputGain",
    "kAsioSupportsInputMeter",
    "kAsioSupportsOutputGain",
    "kAsioSupportsOutputMeter",
    "kAsioOverload"
};