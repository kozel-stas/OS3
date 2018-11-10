#include <sstream>

#include "Connection.h"
#include "Server.h"

Connection::Connection(SOCKET &clientSocket, char *IP) : clientSocket(clientSocket), IP(IP) {}

Connection::~Connection() {
	Server::getInstance().deleteConnection(this);
}

void Connection::addMessage(std::string message) {
	Server::getInstance().addMessage(message);
}

void Connection::processDisconect() {
	std::string connectionMessage = getInfo() + "client " + IP + " disconnected\n";
	std::cout << connectionMessage;
	addMessage(connectionMessage);
}

std::string Connection::idToString() {
	std::stringstream stringStream;
	stringStream << std::this_thread::get_id();
	return stringStream.str();
}

void Connection::clientProcessing() {
	lastPrintTime = std::chrono::system_clock::now();

	fd_set read_s; // Множество
	timeval time_out; // Таймаут

	std::string connectionMessage = getInfo() + "accept new client " + IP + "\n";
	std::cout << connectionMessage;
	addMessage(connectionMessage);
	
	while (true) {
		FD_ZERO(&read_s); // Обнуляем мнодество
		FD_SET(clientSocket, &read_s); // Заносим в него наш сокет 
		time_out.tv_sec = 0; time_out.tv_usec = 0; //Таймаут 0.0 секунды.

		int select_result =  select(0, &read_s, NULL, NULL, &time_out);
		if (select_result == SOCKET_ERROR) {
			processDisconect();
			break;
		}

		if (select_result != 0) {
			char clientMessage[10000];
			int bytesReceived = recv(clientSocket, clientMessage, 10000, 0);
			if (bytesReceived == -1) {
				processDisconect();
				break;
			}
			connectionMessage = getInfo() + IP + " " + std::string(clientMessage, 0, bytesReceived) + "\n";
			std::cout << connectionMessage;
			addMessage(connectionMessage);
			send(clientSocket, clientMessage, bytesReceived + 1, 0); //write data to socket 1-socket-descriptor 2-address 3 - buffer length
		}

		auto end = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(end - lastPrintTime).count() > 1000) {
			lastPrintTime = end;
			std::string timerData = getInfo() + IP + " idle\n";
			std::cout << timerData;
			addMessage(timerData);
		}
	}

	delete this;
}

void Connection::closeSocket() {
	closesocket(clientSocket);
}

std::string Connection::getInfo() {
	std::string preamble = getCurrentTime() + "[" + idToString() + "]: ";
	return preamble;
}

std::string Connection::getCurrentTime() {
	SYSTEMTIME system_time;
	std::stringstream stream;
	GetSystemTime(&system_time);
	stream << "[" << system_time.wHour << ":" << system_time.wMinute << ":" << system_time.wSecond << "]";
	return stream.str();
}