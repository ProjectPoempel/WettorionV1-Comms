#include "Logger.h"

#include <cstdarg>

void Logger::Init()
{
    Serial.begin(115200);
}

void Logger::Info(const String& tag, const String& fmt, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    Serial.printf("[%s/INFO]: ", tag);
    Serial.println(buffer);

    va_end(args);
}

void Logger::Warn(const String& tag, const String& fmt, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    Serial.printf("[%s/WARN]: ", tag);
    Serial.println(buffer);

    va_end(args);
}

void Logger::Error(const String& tag, const String& fmt, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    Serial.printf("[%s/ERROR]: ", tag);
    Serial.println(buffer);

    va_end(args);
}

void Logger::Fatal(const String& tag, const String& fmt, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    Serial.printf("[%s/FATAL]: ", tag);
    Serial.println(buffer);

    va_end(args);
}

void Logger::Debug(const String& tag, const String& fmt, ...)
{
    char buffer[128];

    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    Serial.printf("[%s/DEBUG]: ", tag);
    Serial.println(buffer);

    va_end(args);
}
