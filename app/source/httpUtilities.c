#include <stdio.h>
#include <string.h>
#include "./commonUtilities.h"
#include "./logger.h"
#include "./httpUtilities.h"


char incomingMessage[MAX_INCOMING_MESSAGE_SIZE];
RequestLine requestLine;

HttpResource httpResources[MAX_HTTP_RESOURCES_COUNT];

HttpResource prepareHttpResource(char *serverFilePath, char *clientTarget) {
  HttpResource preparedResource;

  loadStringToArray(serverFilePath, preparedResource.serverFilePath, MAX_SERVER_PATH_SIZE);
  loadStringToArray(clientTarget, preparedResource.clientTarget, MAX_HTTP_TARGET_SIZE);

  if (strstr(serverFilePath, ".html")) {
    preparedResource.type = 1;
  } else if (strstr(serverFilePath, ".css")) {
    preparedResource.type = 2;
  } else if (strstr(serverFilePath, ".js")) {
    preparedResource.type = 3;
  } else {
    preparedResource.type = 0;
  }

  loadWebFileToBuffer(serverFilePath, preparedResource.content);

  appendHttpResource(preparedResource);

  return preparedResource;
}

void appendHttpResource(HttpResource newResource) {
  for (int i = 0; i < MAX_HTTP_RESOURCES_COUNT; i++) {
    if (strlen(httpResources[i].serverFilePath) > 0) {
      continue;
    } else {
      writeLog(0,"RESOURCE APPENDED: %s", newResource.serverFilePath);
      httpResources[i] = newResource;
      return;
    }
  }
}

void extractRequestLine() {
  fillArrayWithNull(requestLine.method, MAX_REQUEST_METHOD_SIZE);
  fillArrayWithNull(requestLine.target, MAX_HTTP_TARGET_SIZE);
  fillArrayWithNull(requestLine.version, MAX_VERSION_SIZE);

  int incomingMessageIndex = 0;
  int memberIndex = 0;

  while (incomingMessage[incomingMessageIndex] != ' ') {
    requestLine.method[memberIndex++] = incomingMessage[incomingMessageIndex++];
  }
  memberIndex = 0;
  incomingMessageIndex++;
  while (incomingMessage[incomingMessageIndex] != ' ') {
    requestLine.target[memberIndex++] = incomingMessage[incomingMessageIndex++];
  }
  memberIndex = 0;
  incomingMessageIndex++;
  while (incomingMessage[incomingMessageIndex] != '\r') {
    requestLine.version[memberIndex++] = incomingMessage[incomingMessageIndex++];
  }

  return;
}

int extractRequestHeaders() {
  int index = 0;
  int readingRequestLine = 1;
  int readingKey = 0;
  int readingValue = 0;
  int keyIndex = 0;
  int valueIndex = 0;
  int headerCount = 0;

  // Clear any existing headers in the headers struct.
  for (int i = 0; i < MAX_HEADER_COUNT; i++) {
    fillArrayWithNull(requestHeaders[i].key, MAX_HEADER_KEY_SIZE);
    fillArrayWithNull(requestHeaders[i].value, MAX_HEADER_VALUE_SIZE);
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
        requestHeaders[headerCount].key[keyIndex++] = incomingMessage[index];
      } else {
        readingKey = 0;
        keyIndex = 0;
        readingValue = 1;
        index+=2;
        fillArrayWithNull(requestHeaders[headerCount].value, MAX_HEADER_VALUE_SIZE);
      }
    }
    // Read a character of the header value.
    if (readingValue) {
      if (!(lineTerminator(index))) {
        requestHeaders[headerCount].value[valueIndex++] = incomingMessage[index];
      } else {
        readingValue = 0;
        valueIndex = 0;
        readingKey = 1;
        headerCount++;
        index++;
        fillArrayWithNull(requestHeaders[headerCount].key, MAX_HEADER_KEY_SIZE);
      }
    }
    index++;
  }

  return 0;
}

int setReplyHeader(char *headerKey, char *headerValue) {
  int nextHeaderIndex = -1;
  for (int i = 0; i <= MAX_HEADER_COUNT; i++) {
    if (strlen(replyHeaders[i].key) > 0) {
      continue;
    } else {
      nextHeaderIndex = i;
      break;
    }
  }
  if (!nextHeaderIndex) {
    return -1;
  }
  strcat(replyHeaders[nextHeaderIndex].key, headerKey);
  strcat(replyHeaders[nextHeaderIndex].value, headerValue);
  return 0;
}

int clearReplyHeaders(){
   for (int i = 0; i <= MAX_HEADER_COUNT; i++) {
    fillArrayWithNull(replyHeaders[i].key, MAX_HEADER_KEY_SIZE);
    fillArrayWithNull(replyHeaders[i].value, MAX_HEADER_VALUE_SIZE);
  }
  return 0;
}


char *findRequestHeaderValue(char *headerKey) {
  for (int i = 0; i < MAX_HEADER_COUNT; i++) {
    writeLog(0, "Checking Header: %s", requestHeaders[i].key);
    if (strcmp(requestHeaders[i].key, headerKey) == 0) {
      writeLog(0, "Header value found: %s",requestHeaders[i].value);
      return requestHeaders[i].value;
    }
  }
  writeLog(2, "Header not found for given key: %s", headerKey);
  return "Header not found.";
}

int findRequestBodyStart() {
  int requestBodyStart = 0;
  int maxHeadersSize = MAX_HEADER_COUNT * (MAX_HEADER_KEY_SIZE + MAX_HEADER_VALUE_SIZE);
  for (int i = 0; i <= maxHeadersSize; i++) {
    if (incomingMessage[i] == '\r' && incomingMessage[i+1] == '\n' && incomingMessage[i+2] == '\r' && incomingMessage[i+3] == '\n') {
      requestBodyStart = i+4;
      return requestBodyStart;
    }
  }
  return -1;
}

void extractRequestBody(int contentLength) {
  int requestBodyStart;
  if ((requestBodyStart = findRequestBodyStart())) {
    fillArrayWithNull(httpRequestBody, MAX_INCOMING_MESSAGE_SIZE);
    for (int i = 0; i <= contentLength; i++) {
      if (incomingMessage[requestBodyStart+i] != '\0') {
        httpRequestBody[i] = incomingMessage[requestBodyStart+i];
      } else {
        return;
      }
    }
  } else {
    writeLog(2, "Could not find start of http body.");
  }
}

void loadWebFileToBuffer(char *webFileName, char* webFileArray) {
  fillArrayWithNull(webFileArray, MAX_HTTP_RESOURCE_SIZE);
  FILE *webFileStream;
  int counter = 0;

  webFileStream = fopen(webFileName, "r");

  if (webFileStream == NULL) {
    writeLog(2, "Could not open file: %s", webFileName);
    return;
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
    writeLog(2, "Could not read %s to array.", webFileName);
    return;
  } else {
    webFileArray[counter] = '\0';
    writeLog(0, "File %s successfully loaded.", webFileName);
  }
}

char *setStatusLine(int code) {
  StatusLine statusLine;
  fillArrayWithNull(httpStatusLine, MAX_STATUS_LINE_SIZE);
  statusLine.protocol = "HTTP/1.1 ";

  switch(code) {
    case 101:
      statusLine.code = "101 ";
      statusLine.reason = "Switching Protocols\r\n";
      break;
    case 200:
      statusLine.code = "200 ";
      statusLine.reason = "OK\r\n";
      break;
    case 404:
      statusLine.code = "404 ";
      statusLine.reason = "Not Found\r\n";
      break;
    case 500:
      statusLine.code = "500 ";
      statusLine.reason = "Internal Server Error\r\n";
      break;
    default:
      statusLine.code = "500 ";
      statusLine.reason = "Internal Server Error\r\n";
  }
  strcat(httpStatusLine, statusLine.protocol);
  strcat(httpStatusLine, statusLine.code);
  strcat(httpStatusLine, statusLine.reason);
  return httpStatusLine;
}

int formHttpReply(int statusCode, char *replyBody) {
  fillArrayWithNull(httpReplyMessage, MAX_HTTP_REPLY_BODY_SIZE);

  char formattedReplyHeaders[MAX_HEADER_COUNT * (MAX_HEADER_KEY_SIZE + MAX_HEADER_VALUE_SIZE)];
  fillArrayWithNull(formattedReplyHeaders, (MAX_HEADER_COUNT * (MAX_HEADER_KEY_SIZE + MAX_HEADER_VALUE_SIZE)));

  setStatusLine(statusCode);
  strcat(httpReplyMessage, httpStatusLine);

  for (int i = 0; i <= MAX_HEADER_COUNT; i++) {
    if (strlen(replyHeaders[i].key) > 0) {
      strcat(formattedReplyHeaders, replyHeaders[i].key);
      strcat(formattedReplyHeaders, ": ");
      strcat(formattedReplyHeaders, replyHeaders[i].value);
      strcat(formattedReplyHeaders, "\r\n");
    } else {
      strcat(formattedReplyHeaders, "\r\n");
      break;
    }
  }

  strcat(httpReplyMessage, formattedReplyHeaders);
  strcat(httpReplyMessage, replyBody);

  return 1;
}
