#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "./commonUtilities.h"
#include "./logger.h"
#include "./socketHandler.h"


int mainSocket, acceptedSocket;
struct sockaddr_in server, client;

int createSocket() {
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  server.sin_port = htons(8081);

  mainSocket = socket(AF_INET, SOCK_STREAM, 0);
  return errorCheck(mainSocket < 0, "Socket creation failed.", "Socket created.");
}


int socketSetReusePort() {
  int reusePort = 1;
  int socketOptions = setsockopt(mainSocket, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
  return errorCheck(socketOptions < 0, "Failed to set socket option REUSEPORT.", "Socket options set REUSEPORT.");
}

int socketSetKeepAlive() {
  int keepAlive = 0;
  int socketOptions = setsockopt(mainSocket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));
  return errorCheck(socketOptions < 0, "Failed to set socket option KEEPALIVE.", "Socket options set KEEPALIVE.");
}


int setBind() {
  int bindResult = bind(mainSocket, (struct sockaddr *)&server, sizeof(server));
  return errorCheck(bindResult < 0, "Bind failed.", "Bind complete.");
}


int setListen() {
  int listenResult = listen(mainSocket, 3);
  return errorCheck(listenResult == -1, "Listen failed.", "Waiting for connections...");
}


int setAccept() {
  int c = sizeof(struct sockaddr_in);

  acceptedSocket = accept(mainSocket, (struct sockaddr *)&client, (socklen_t*)&c);

  char *client_ip = inet_ntoa(client.sin_addr);
  int client_port = ntohs(client.sin_port);

  int result = errorCheck(acceptedSocket < 0, "Connection accept failed.", "Connection accept successful.");
  if (result) {
    writeLog(0, "Connection accepted from: %s:%d",client_ip,client_port);
  }
  return acceptedSocket;
}


int recvData(char *incomingMessage) {
  int recvResult = recv(acceptedSocket, incomingMessage, MAX_MESSAGE_SIZE, 0);
  errorCheck(recvResult < 0, "recvData failed.", "Incoming message received.");
  return recvResult;
}


int sendData(char *data) {
  int writeResult = send(acceptedSocket, data, strlen(data), 0);
  return errorCheck(writeResult < 0, "Error writing message to socket.", "Message successfully written to socket.");
}


int closeConnection() {
  int closeResult = close(acceptedSocket);
  return errorCheck(closeResult == -1, "Connection close failure.", "Connection close successful.");
}
