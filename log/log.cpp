//
// Created by Fizz on 8/31/2023.
//

#include <iostream>
#include <cstdarg>
#include <windows.h>
#include "log.h"

#define LOGUTIL_IMPL(suffix, color)\
void dbg##suffix(const char* module_name, const char* format, ...)\
{\
    va_list args;\
    va_start(args, format);\
    console_log(color, module_name, format, args);\
}\
void dbg##suffix(const wchar_t* module_name, const wchar_t* format, ...)\
{\
    va_list args;\
    va_start(args, format);\
    console_log(L"" color, module_name, format, args);\
}

LOGUTIL_IMPL(ok, GREEN)
LOGUTIL_IMPL(error, RED)
LOGUTIL_IMPL(unimportant, GRAY)
LOGUTIL_IMPL(info, BLUE)
LOGUTIL_IMPL(warning, YELLOW)
LOGUTIL_IMPL(out, NOCOLOR)

void console_log(const char* color_code, const char* module_name, const char* format, va_list args)
{
    SYSTEMTIME t;
    GetSystemTime(&t);

    printf("%s[%02d:%02d:%02d:%03d] - %s : ", color_code, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, module_name);

    vprintf(format, args);

    printf(NOCOLOR"\n");
}

void console_log(const wchar_t* color_code, const wchar_t* module_name, const wchar_t* format, va_list args)
{
    SYSTEMTIME t;
    GetSystemTime(&t);
    wprintf(L"%s[%02d:%02d:%02d:%03d] - %s : ", color_code, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, module_name);
    vwprintf(format, args);
    wprintf(L"" NOCOLOR "\n");
}