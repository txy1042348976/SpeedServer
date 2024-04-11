#ifndef __Z_DEBUG_H__
#define __Z_DEBUG_H__

#include <windows.h>
#include <stdio.h>

#define LOG_COLOR_TRACE  FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
#define LOG_COLOR_DEBUG  FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
#define LOG_COLOR_INFO  FOREGROUND_GREEN
#define LOG_COLOR_WARN  FOREGROUND_RED | FOREGROUND_GREEN
#define LOG_COLOR_ERROR  FOREGROUND_RED
#define LOG_COLOR_FATAL  FOREGROUND_RED | FOREGROUND_INTENSITY


#define _ReleasePrint_(format, ...) \
    //printf( __DATE__ " - " __TIME__ " - " __FILE__ ":%d: "format, __LINE__, ##__VA_ARGS__)
#define _DebugPrint_(format, ...) \
    printf(__FILE__ ":%d: "format, __LINE__, ##__VA_ARGS__)

#define _LevelTrace_ 0
#define _LevelDebug_ 1
#define _LevelInfo_ 2
#define _LevelWarn_ 3
#define _LevelError_ 4
#define _LevelFatal_ 5

#ifdef _DEBUG
#define _LogPrint_ _DebugPrint_
#define _LogLevel_ _LevelDebug_
#else
#define _LogPrint_ _ReleasePrint_
#define _LogLevel_ _LevelInfo_
#endif


#define _PrintWithColor_(color, format, ...) \
    { \
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo; \
        WORD saved_attributes; \
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo); \
        SetConsoleTextAttribute(hConsole, color); \
        _LogPrint_(format, ##__VA_ARGS__); \
        SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes); \
    }

#define _PrintWithLevelAndColor_(level, color, format, ...) \
    { \
        if (level >= _LogLevel_) \
        { \
            _PrintWithColor_(color, format, ##__VA_ARGS__); \
        } \
    }

#define Z_TRACE(format, ...) _PrintWithLevelAndColor_(_LevelTrace_, LOG_COLOR_TRACE, format, ##__VA_ARGS__)
#define Z_DEBUG(format, ...) _PrintWithLevelAndColor_(_LevelDebug_, LOG_COLOR_DEBUG, format, ##__VA_ARGS__)
#define Z_INFO(format, ...) _PrintWithLevelAndColor_(_LevelInfo_, LOG_COLOR_INFO, format, ##__VA_ARGS__)
#define Z_WARN(format, ...) _PrintWithLevelAndColor_(_LevelWarn_, LOG_COLOR_WARN, format, ##__VA_ARGS__)
#define Z_ERROR(format, ...) _PrintWithLevelAndColor_(_LevelError_, LOG_COLOR_ERROR, format, ##__VA_ARGS__)
#define Z_FATAL(format, ...) _PrintWithLevelAndColor_(_LevelFatal_, LOG_COLOR_FATAL, format, ##__VA_ARGS__)
#define Z_BLUE(format, ...) _PrintWithColor_(FOREGROUND_BLUE, format, ##__VA_ARGS__)
#define Z_RED(format, ...) _PrintWithColor_(FOREGROUND_RED, format, ##__VA_ARGS__)
#define Z_GREEN(format, ...) _PrintWithColor_(FOREGROUND_GREEN, format, ##__VA_ARGS__)
#define Z_YELLOW(format, ...) _PrintWithColor_(FOREGROUND_RED | FOREGROUND_GREEN, format, ##__VA_ARGS__)
#define Z_PURPLE(format, ...) _PrintWithColor_(FOREGROUND_RED | FOREGROUND_BLUE, format, ##__VA_ARGS__)
#define Z_CYAN(format, ...) _PrintWithColor_(FOREGROUND_GREEN | FOREGROUND_BLUE, format, ##__VA_ARGS__)
#define Z_WHITE(format, ...) _PrintWithColor_(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, format, ##__VA_ARGS__)
#define Z_GRAY(format, ...) _PrintWithColor_(FOREGROUND_INTENSITY, format, ##__VA_ARGS__)

#endif /* __Z_DEBUG_H__ */