#pragma once

#include "Config.h"

#include <Arduino.h>

#ifdef LOG_ENABLED
#define LOG_INFO(tag, fmt, ...) Logger::Info(tag, fmt, ##__VA_ARGS__)
#define LOG_WARN(tag, fmt, ...) Logger::Warn(tag, fmt, ##__VA_ARGS__)
#define LOG_ERROR(tag, fmt, ...) Logger::Error(tag, fmt, ##__VA_ARGS__)
#define LOG_FATAL(tag, fmt, ...) Logger::Fatal(tag, fmt, ##__VA_ARGS__)
#ifdef LOG_DEBUG_ENABLED
#define LOG_DEBUG(tag, fmt, ...) Logger::Debug(tag, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(tag, fmt, ...)
#endif
#else
#define LOG_INFO(tag, fmt, ...)
#define LOG_WARN(tag, fmt, ...)
#define LOG_ERROR(tag, fmt, ...)
#define LOG_FATAL(tag, fmt, ...)
#define LOG_DEBUG(tag, fmt, ...)
#endif

class Logger
{
public:
    static void Init();
    static void Info(const String& tag, const String& fmt, ...);
    static void Warn(const String& tag, const String& fmt, ...);
    static void Error(const String& tag, const String& fmt, ...);
    static void Fatal(const String& tag, const String& fmt, ...);
    static void Debug(const String& tag, const String& fmt, ...);
};