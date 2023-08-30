//
// Created by Fizz on 8/31/2023.
//

#include "SampleFormatConverter.h"

std::map<ASIOSampleType, SampleTypeInfo> sample_type_table = {
        {ASIOSTInt16LSB, {"Int16LSB", 2, true, true, abs_int16, write_int16}},
        {ASIOSTInt32LSB, {"Int32LSB", 4, true, true, abs_int32, write_int32}},

        {ASIOSTFloat32LSB, {"Float32LSB", 4, true, false, abs_float32, write_float32}},
        {ASIOSTFloat64LSB, {"Float64LSB", 8, true, false, abs_float64, write_float64}}
};

double abs_int16(void *p) {return ((double)(*(int16_t*)p))/double(0x7FFF);}

double abs_int32(void *p) {return ((double)(*(int32_t*)p))/double(0x7FFFFFFF);}

double abs_float32(void *p) {return (double)(*(float*)p);}

double abs_float64(void *p) {return *((double*)p);}

void write_int16(double abs, void *p) {*((int16_t*)p) = (int16_t)(abs * 0x7FFF);}

void write_int32(double abs, void *p) {*((int32_t*)p) = (int32_t)(abs * 0x7FFFFFFF);}

void write_float32(double abs, void *p) {*((float*)p) = (float)abs;}

void write_float64(double abs, void *p) {*((double*)p) = abs;}
