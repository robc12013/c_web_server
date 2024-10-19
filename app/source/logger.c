#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "./logger.h"

void writeLog(int level, char *logFString, ...) {
  char *logLevel;

  switch(level) {
    case 0:
      logLevel = "INFO";
      break;
    case 1:
      logLevel = "WARN";
      break;
    case 2:
      logLevel = "ERROR";
      break;
    default:
      logLevel = "UNKNOWN";
  }
  
  time_t epochTime = time(NULL);
  struct tm tm = *localtime(&epochTime);

  va_list arg_ptr;
  va_start(arg_ptr, logFString);

  char logMessage[MAX_LOG_MESSAGE_SIZE];
  vsprintf(logMessage, logFString, arg_ptr);

  FILE *logFile = fopen("/app/logs/app.log", "a");
  fprintf(logFile, "%s ",logLevel);
  fprintf(logFile, "[ UTC %d-%d-%d %02d:%02d:%02d ]  ", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  fprintf(logFile, logMessage);
  fprintf(logFile, "\n");
  fclose(logFile);
}
