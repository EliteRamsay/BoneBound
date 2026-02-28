#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

static FILE* logFile = NULL;

static void InitLogger()
{
    if (!logFile) {
        logFile = fopen("debug_log.txt", "w");
        if (logFile) {
            time_t now = time(NULL);
            fprintf(logFile, "=== BONEBOUND DEBUG LOG ===\n");
            fprintf(logFile, "Started: %s", ctime(&now));
            fflush(logFile);
        }
    }
}

static void CloseLogger()
{
    if (logFile) {
        time_t now = time(NULL);
        fprintf(logFile, "\nClosed: %s", ctime(&now));
        fclose(logFile);
        logFile = NULL;
    }
}

#define LOG(...) \
    do { \
        InitLogger(); \
        if (logFile) { \
            fprintf(logFile, __VA_ARGS__); \
            fflush(logFile); \
        } \
    } while(0)

#define LOG_PRINTF(...) LOG(__VA_ARGS__)

#endif