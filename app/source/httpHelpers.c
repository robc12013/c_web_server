#include <stdio.h>
#include <string.h>
#include "/app/source/logger.h"

// From logger.c.
// void writeLog(char *logMessage, ...);

#define MAX_FILE_SIZE 10000
#define MAX_HTTP_REPLY_SIZE 20000
static char httpBodyRecv[2000];
static char requestMethod[2000];
static char httpReply[MAX_HTTP_REPLY_SIZE];
struct httpHeader {
  char key[1000];
  char value[1000];
};


void fillArrayWithNull(char array[], int size) {
  for (int i = 0; i < size; i++){
    array[i] = '\0';
  }
}

char *extractRequestMethod(char *incomingMessage) {
  fillArrayWithNull(requestMethod, 2000);
  for (int i = 0; i <= 2000; i++) {
    if (incomingMessage[i] != '\r' && incomingMessage[i+1] != '\n') {
      requestMethod[i] = incomingMessage[i];
    } else {
      return requestMethod;
    }
  }
  return "";
}

int extractRequestHeaders(char *incomingMessage, struct httpHeader *headers) {
  int index = 0;
  int readingRequestLine = 1;
  int readingKey = 0;
  int readingValue = 0;
  int keyIndex = 0;
  int valueIndex = 0;
  int headerCount = 0;

  // Clear any existing headers in the headers struct.
  for (int i = 0; i < 100; i++) {
    fillArrayWithNull(headers[i].key, 1000);
    fillArrayWithNull(headers[i].value, 1000);
  }
  
  // RFC9112 states a recipient MAY recognize a single LF as a line terminator.
  int lineTerminator(int index) {
    return (
      (incomingMessage[index] == '\r' && 
      incomingMessage[index+1] == '\n') ||
      incomingMessage[index] == '\n'
    );
  }

  int emptyLine(int index) {
    return (
      incomingMessage[index] == '\r' && 
      incomingMessage[index+1] == '\n'
    );
  }

  int endOfHeaders(int index) {
    return (
      lineTerminator(index) && 
      (emptyLine(index+2) || emptyLine(index+1))
    );
  }

  // RFC9112 requires that empty line(s) received prior to the request-line should be ignored.
  while (emptyLine(index)) {
    index += 2;
    if (index > 10) {
      return 1;
    }
  }
  
  while (!(endOfHeaders(index))) {
    // Skip over the HTTP request line.
    if (readingRequestLine) {
      if (lineTerminator(index)) {
        readingRequestLine = 0;
        readingKey = 1;
        index+=2;
      } else { 
        index++;
        continue;
      }
    }
    // Read a character of the header key.
    if (readingKey) {
      if (incomingMessage[index] != ':') {
        headers[headerCount].key[keyIndex++] = incomingMessage[index];
      } else {
        readingKey = 0;
        keyIndex = 0;
        readingValue = 1;
        index+=2;
        fillArrayWithNull(headers[headerCount].value, 1000);
      }
    }
    // Read a character of the header value.
    if (readingValue) {
      if (!(lineTerminator(index))) {
        headers[headerCount].value[valueIndex++] = incomingMessage[index];
      } else {
        readingValue = 0;
        valueIndex = 0;
        readingKey = 1;
        headerCount++;
        index++;
        fillArrayWithNull(headers[headerCount].key, 500);
      }
    }
    index++;
  }

  return 0;
}


char *findRequestHeaderValue(struct httpHeader *headers, char *headerkey) {
  for (int i = 0; i < 100; i++) {
    if (strcmp(headers[i].key, headerkey) == 0) {
      return headers[i].value;
    }
  }
  return "Header not found.";
}

int findHttpBodyStart(char *incomingMessage) {
  int bodyStart;
  for (int i = 0; i <= 1996; i++) {
    if (incomingMessage[i] == '\r' && incomingMessage[i+1] == '\n' && incomingMessage[i+2] == '\r' && incomingMessage[i+3] == '\n') {
      bodyStart = i+4;
      return bodyStart;
    }
  }
  return -1;
}

char *extractHttpBody(char *incomingMessage, int bodyStart){
  fillArrayWithNull(httpBodyRecv, 2000);
  for (int i = 0; i <= 2000; i++) {
    if (incomingMessage[bodyStart+i] != '\0') {
      httpBodyRecv[i] = incomingMessage[bodyStart+i];
    } else {
      return httpBodyRecv;
    }
  }
  return "";
}

void loadWebFileToArray(char *webFileName, char* webFileArray) {
  fillArrayWithNull(webFileArray, MAX_FILE_SIZE);
  FILE *webFileStream;
  int counter = 0;

  webFileStream = fopen(webFileName, "r");

  if (webFileStream == NULL) {
    writeLog("Could not open file: %s", webFileName);
  } else {
    while (getc(webFileStream) != EOF) {
      if (feof(webFileStream)) {
        break;
      }
      counter++;
    }
    rewind(webFileStream);
  }
    
  int readFile = fread(webFileArray, 1, counter, webFileStream);
  fclose(webFileStream);

  if (readFile <= 0) {
    writeLog("Could not read %s to array.", webFileName);
  } else {
    webFileArray[counter] = '\0';
    writeLog("file %s successfully loaded.", webFileName);
  }
}

char *formHttpReply(char *httpStartLine, char *httpHeaders, char *httpBody) {
  fillArrayWithNull(httpReply, MAX_HTTP_REPLY_SIZE);

  int headerStart = 0;
  int replyBodyStart;
  int endOfReply = strlen(httpBody) + strlen(httpStartLine) + strlen(httpHeaders);

  for (int i = 0; i < strlen(httpStartLine); i++) {
    httpReply[i] = httpStartLine[i];
    headerStart+=1;
  }
  replyBodyStart = headerStart;

  for (int i = 0; i < strlen(httpHeaders); i++) {
    httpReply[i + headerStart] = httpHeaders[i];
    replyBodyStart+=1;
  }

  for (int i = 0; i < endOfReply; i++) {
    if (httpBody[i] != '\0') {
      httpReply[replyBodyStart+i] = httpBody[i];
    } else {
      break;
    }
  }

  return httpReply;
}
