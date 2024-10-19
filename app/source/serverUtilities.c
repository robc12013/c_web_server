#include <stdio.h>
#include <string.h>
#include "./commonUtilities.h"
#include "./logger.h"
#include "./socketHandler.h"
#include "./httpUtilities.h"
#include "./serverUtilities.h"

char httpRequestLine[MAX_REQUEST_LINE_SIZE];
char httpStatusLine[MAX_STATUS_LINE_SIZE];
HttpHeader requestHeaders[MAX_HEADER_COUNT];
HttpHeader replyHeaders[MAX_HEADER_COUNT];
char httpRequestBody[MAX_INCOMING_MESSAGE_SIZE];
char httpReplyMessage[MAX_HTTP_REPLY_BODY_SIZE];

void serveStatus(int code) {
  char *message = "";
  switch (code) {
    case 404:
      message = "404 Not Found";
    default:
      message = "Response code message.";
      writeLog(0,"No message available for status code: %d", code);
  }
  formHttpReply(code, message);
  sendData(httpReplyMessage);
  clearReplyHeaders();
}

int serveResources(HttpResource *httpResources) {
  for (int i = 0; i < MAX_HTTP_RESOURCES_COUNT; i++) {
    if (strcmp(requestLine.target, httpResources[i].clientTarget) == 0) {
      switch (httpResources[i].type) {
        case 0:
          setReplyHeader("Content-Type", "text/plain; charset=utf-8");
          break;
        case 1:
          setReplyHeader("Content-Type", "text/html; charset=utf-8");
          break;
        case 2:
          setReplyHeader("Content-Type", "text/css; charset=utf-8");
          break;
        case 3:
          setReplyHeader("Content-Type", "text/javascript; charset=utf-8");
          break;
        default:
          setReplyHeader("Content-Type", "text/plain; charset=utf-8");
      }
      formHttpReply(200, httpResources[i].content);
      sendData(httpReplyMessage);
      clearReplyHeaders();
      return 1;
    }
  }
  serveStatus(404);
  return -1;
}
