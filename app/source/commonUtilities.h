#ifndef COMMONUTILITIES_H
#define COMMONUTILITIES_H

int errorCheck(int failCondition, char *failMessage, char *successMessage);
void fillArrayWithNull(char *array, int size);
int loadStringToArray(char *string, char *array, int maxCharsCount);
// void copyUntil(char *sourceArray, char*destinationArray, char until, int afterOccurence);

#endif