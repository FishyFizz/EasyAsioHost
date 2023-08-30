//
// Created by Fizz on 8/31/2023.
//

#ifndef EASYASIOHOST_LOG_H
#define EASYASIOHOST_LOG_H

#include <cstdarg>

#define BLACK       "\u001b[30m"
#define RED         "\u001b[31m"
#define GREEN       "\u001b[32m"
#define YELLOW      "\u001b[33m"
#define BLUE        "\u001b[34m"
#define MAGENTA     "\u001b[35m"
#define CYAN        "\u001b[36m"
#define WHITE       "\u001b[37m"
#define NOCOLOR     "\u001b[0m"
#define GRAY        "\u001b[30;1m"

#define LOGUTIL_DECL(suffix)\
void dbg##suffix(const char* module_name, const char* format, ...);\
void dbg##suffix(const wchar_t* module_name, const wchar_t* format, ...);

LOGUTIL_DECL(ok)
LOGUTIL_DECL(error)
LOGUTIL_DECL(unimportant)
LOGUTIL_DECL(info)
LOGUTIL_DECL(warning)
LOGUTIL_DECL(out)

void console_log(const char* color_code, const char* module_name, const char* format, va_list args);
void console_log(const wchar_t* color_code, const wchar_t* module_name, const wchar_t* format, va_list args);

#endif //EASYASIOHOST_LOG_H
