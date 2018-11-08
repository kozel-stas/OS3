#include <sstream>

#include "Connection.h"
#include "Server.h"

Connection::Connection(SOCKET &clientSocket, char *IP) : clientSocket(clientSocket), IP(IP) {}

Connection::~Connection() {

	auto &server = Server::getInstance();
	server.deleteConnection(this);
}

void Connection::addMessage(std::string message) {

	auto &server = Server::getInstance();
	server.addMessage(message);
}

void Connection::threadTimer(std::string threadId) {
	// нарушение прав на чтение wtf? ip не определен?
	while (isActive == 1) {
		std::string timerData = getInfo() + " 127.0.0.1"  + " idle\n";
		std::cout << timerData;
		addMessage(timerData);
		Sleep(1000);
	}
}

std::string Connection::idToString() {
	std::stringstream stringStream;
	stringStream << std::this_thread::get_id();
	return stringStream.str();
}

void Connection::clientProcessing() {

	std::thread timerThread(&Connection::threadTimer, this, idToString());
	timerThread.detach();
	std::string connectionMessage = getInfo() + "accept new client " + IP + "\n";
	std::cout << connectionMessage;
	addMessage(connectionMessage);

	while (isActive) {
		char clientMessage[10000];
		int bytesReceived = recv(clientSocket, clientMessage, 1000, 0);
	
		if (bytesReceived == -1 || bytesReceived == 0) {
			setIsActive(false);
			connectionMessage = getInfo() + "client " + IP + " disconnected\n";
			std::cout << connectionMessage;
			addMessage(connectionMessage);
		}
		else {
			connectionMessage = getInfo() + IP + " " + std::string(clientMessage, 0, bytesReceived) + "\n";
			std::cout << connectionMessage;
			addMessage(connectionMessage);
			send(clientSocket, clientMessage, bytesReceived + 1, 0); //write data to socket 1-socket-descriptor 2-address 3 - buffer length
		}
	}
	delete this;
}

void Connection::setIsActive(bool isActive) {
	Connection::isActive = isActive;
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