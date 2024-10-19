#include <stdio.h>
#include <string.h>
#include "./commonUtilities.h"
#include "./logger.h"
#include "./socketHandler.h"
#include "./httpUtilities.h"
#include "./serverUtilities.h"

int main() {

  HttpResource html = prepareHttpResource("app/httpResources/chat.html", "/chat");
  HttpResource css = prepareHttpResource("app/httpResources/chat.css", "/chat.css");
  HttpResource js = prepareHttpResource("app/httpResources/chat.js", "/chat.js");

  // httpResources[0] = html;
  // httpResources[1] = css;
  // httpResources[2] = js;

  createSocket();
  socketSetReusePort();
  socketSetKeepAlive();
  setBind();
  setListen();

  int acceptResult;
  while ((acceptResult = setAccept())) {
    if ((recvData(incomingMessage))) {
      extractRequestLine();
      writeLog(0, "requestLine.method: %s", requestLine.method);
      writeLog(0, "requestLine.target: %s", requestLine.target);
      writeLog(0, "requestLine.version: %s", requestLine.version);
    }
    
    writeLog(0, "strlen(httpResources[0].serverFilePath): %d", strlen(httpResources[0].serverFilePath));
    writeLog(0, "strlen(httpResources[1].serverFilePath): %d", strlen(httpResources[1].serverFilePath));
    writeLog(0, "strlen(httpResources[2].serverFilePath): %d", strlen(httpResources[2].serverFilePath));
    writeLog(0, "strlen(httpResources[3].serverFilePath): %d", strlen(httpResources[3].serverFilePath));
    writeLog(0, "strlen(httpResources[4].serverFilePath): %d", strlen(httpResources[4].serverFilePath));

    serveResources(httpResources);

    fillArrayWithNull(incomingMessage, MAX_INCOMING_MESSAGE_SIZE);
    closeConnection();
  }

  return 0;
}
