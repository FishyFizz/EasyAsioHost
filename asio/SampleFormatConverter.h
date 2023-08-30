//
// Created by Fizz on 8/31/2023.
//

#ifndef NODEDAW_SAMPLEFORMATCONVERTER_H
#define NODEDAW_SAMPLEFORMATCONVERTER_H

#include "AsioCommon.h"

struct SampleTypeInfo
{
    std::string name;
    short byte_per_sample;
    bool is_lsb;
    bool is_integer;

    double (*sample_reader)(void*);
    void (*sample_writer)(double, void*);
};

double abs_int16(void* p);
double abs_int32(void* p);
double abs_float32(void* p);
double abs_float64(void* p);
void write_int16(double abs, void* p);
void write_int32(double abs, void* p);
void write_float32(double abs, void*p);
void write_float64(double abs, void*p);

extern std::map<ASIOSampleType, SampleTypeInfo> sample_type_table;

class SampleFormatConverter
{
public:
    SampleTypeInfo sample_type_from, sample_type_to;

    bool bypass = false;

    SampleFormatConverter(ASIOSampleType from, ASIOSampleType to):
            sample_type_from(sample_type_table[from]), sample_type_to(sample_type_table[to])
    {
        if(from == to)
            bypass = true;
    }

    void ConvertBlock(void* from, void* to, size_t len)
    {
        uint8_t* buffer_from = (uint8_t*)from;
        uint8_t* buffer_to = (uint8_t*) to;
        if(bypass)
        {
            memcpy(to, from, len*sample_type_from.byte_per_sample);
            return;
        }

        for(size_t from_offset=0, to_offset=0, i=0;
            i<len;
            i++, from_offset+=sample_type_from.byte_per_sample, to_offset+=sample_type_to.byte_per_sample )
        {
            sample_type_to.sample_writer(sample_type_from.sample_reader(buffer_from+from_offset), buffer_to+to_offset);
        }
    }
};

#endif //NODEDAW_SAMPLEFORMATCONVERTER_H
