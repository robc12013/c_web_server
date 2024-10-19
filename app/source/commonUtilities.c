#include <stdio.h>
#include <string.h>
#include "./logger.h"
#include "./commonUtilities.h"

int errorCheck(int failCondition, char *failMessage, char *successMessage) {
  if (failCondition) {
    writeLog(2, failMessage);
    return -1;
  } else {
    writeLog(0, successMessage);
    return 1;
  }
}

int loadStringToArray(char *string, char *array, int maxCharsCount) {
  int sourceStringLength = strlen(string);

  if (sourceStringLength > maxCharsCount || maxCharsCount < 1) {
    return -1;
  }

  for (int i = 0; i < sourceStringLength; i++) {
    array[i] = string[i];
  }
  array[sourceStringLength] = '\0';

  if (strlen(array) > maxCharsCount) {
    return -1;
  } else {
    return 1;
  }
}


void fillArrayWithNull(char *array, int size) {
  for (int i = 0; i < size; i++){
    array[i] = '\0';
  }
}


