#ifndef HTTPUTILITIES_H
#define HTTPUTILITIES_H

// ENUMERATIONS

enum maxSizesAndCounts {
  MAX_HEADER_COUNT = 128,
  MAX_HEADER_KEY_SIZE = 128,
  MAX_HEADER_VALUE_SIZE = 512,
  MAX_HTTP_REPLY_BODY_SIZE = 65536,
  MAX_HTTP_RESOURCE_SIZE = 32768,
  MAX_INCOMING_MESSAGE_SIZE = 65536,
  MAX_REQUEST_LINE_SIZE = 1024,
  MAX_STATUS_LINE_SIZE = 256,
  MAX_REQUEST_METHOD_SIZE = 16,
  MAX_HTTP_TARGET_SIZE = 512,
  MAX_VERSION_SIZE = 16,
  MAX_HTTP_RESOURCES_COUNT = 32,
  MAX_SERVER_PATH_SIZE = 128,
};

// VARIABLES

extern char httpStatusLine[MAX_STATUS_LINE_SIZE];
extern char httpRequestLine[MAX_REQUEST_LINE_SIZE];
extern char httpRequestBody[MAX_INCOMING_MESSAGE_SIZE];
extern char httpReplyMessage[MAX_HTTP_REPLY_BODY_SIZE];


// STRUCTURES

typedef struct StatusLine StatusLine;
struct StatusLine{
  char *protocol;
  char *code;
  char *reason;
};

typedef struct RequestLine RequestLine;
struct RequestLine{
  char method[MAX_REQUEST_METHOD_SIZE];
  char target[MAX_HTTP_TARGET_SIZE];
  char version[MAX_VERSION_SIZE];
};

typedef struct HttpHeader HttpHeader;
struct HttpHeader {
  char key[MAX_HEADER_KEY_SIZE];
  char value[MAX_HEADER_VALUE_SIZE];
};

typedef struct HttpResource HttpResource;
struct HttpResource {
  char serverFilePath[MAX_SERVER_PATH_SIZE];
  char clientTarget[MAX_HTTP_TARGET_SIZE];
  int type;
  char content[MAX_HTTP_RESOURCE_SIZE];
};

extern HttpHeader requestHeaders[MAX_HEADER_COUNT];
extern HttpHeader replyHeaders[MAX_HEADER_COUNT];
extern RequestLine requestLine;
extern HttpResource httpResources[MAX_HTTP_RESOURCES_COUNT];


// FUNCTIONS

HttpResource prepareHttpResource(char *serverFilePath, char *clientTarget);
void appendHttpResource(HttpResource newResource);
void extractRequestLine();
int extractRequestHeaders();
int setReplyHeader(char *headerKey, char *headerValue);
char *findRequestHeaderValue(char *headerkey);
int findRequestBodyStart();
void extractRequestBody(int contentLength);
void loadWebFileToBuffer(char *webFileName, char* webFileArray);
int formHttpReply(int statusCode, char *replyBody);
char *setStatusLine(int code);
int clearReplyHeaders();

#endif
