#ifndef SOCKETUTILITIES_H
#define SOCKETUTILITIES_H

// ENUMERATIONS

enum limits {
  MAX_MESSAGE_SIZE = 65536
};

// VARIABLES

extern char incomingMessage[MAX_MESSAGE_SIZE];


// FUNCTIONS

int createSocket();
int socketSetReusePort();
int socketSetKeepAlive();
int setBind();
int setListen();
int setAccept();
int recvData(char *incomingMessage);
int sendData(char *data);
int closeConnection();

#endif