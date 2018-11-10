#pragma once
#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include <ws2tcpip.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Connection {
private:

	SOCKET clientSocket;
	char *IP;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> lastPrintTime;

	void addMessage(std::string message);
	void processDisconect();

	std::string idToString();

public:

	Connection(SOCKET &clientSocket, char *IP);

	virtual ~Connection();

	void clientProcessing();
	void closeSocket();

	std::string getInfo();
	std::string getCurrentTime();
};


#endif //SERVER_CONNECTION_H