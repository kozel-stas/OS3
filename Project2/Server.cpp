#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <ws2tcpip.h>
#include <inaddr.h>
#include <direct.h>
#include <fstream>
#include <sstream>
#include "Server.h"
#pragma warning(disable : 4996)

Server::Server() {

	adjustWsaData();
	adjustListeningSocket();
	bindSocketHint();
}

Server::~Server()
{
}

void Server::adjustWsaData() {

	int wsaStartup = WSAStartup(ver, &wsData);
	if (wsaStartup != 0) {
		std::cerr << "Can't initialize winSock, Err #" << WSAGetLastError() << std::endl;
		return;
	}
}

void Server::adjustListeningSocket() {

	listeningSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET - IPv4, SOCK_STREAM - USE FOR TCP, 0 - means that we use default protocol
	if (listeningSocket == INVALID_SOCKET) {
		std::cerr << "Can't create a listening socket, Err #" << WSAGetLastError() << std::endl;
		return;
	}
}

void Server::bindSocketHint() {

	// bind the ip address and port to a socket

	sockaddr_in listeningSocketHint;
	listeningSocketHint.sin_family = AF_INET;
	listeningSocketHint.sin_port = htons(PORT);
	listeningSocketHint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listeningSocket, (sockaddr *)&listeningSocketHint, sizeof(listeningSocketHint));
}

Server& Server::getInstance() { // return reference for data type Server, which initialized in main.cpp
	static Server server;
	return server;
}
void Server::interruption_handler(int param) {
	system("cls");
	getInstance().stopServer();
	system("Pause");
	exit(0);
}

void Server::stopServer() {
	isActive = false;
	shutDownAllConnections();
	dumpLog();
}

void Server::dumpLog() {
	char current_work_dir[FILENAME_MAX];
	_getcwd(current_work_dir, sizeof(current_work_dir));
	SYSTEMTIME system_time;
	GetSystemTime(&system_time);

	std::stringstream stringStream;
	stringStream << current_work_dir  << "\\" << system_time.wDay << "_" << system_time.wMonth << "_"<< system_time.wYear << " at time " << system_time.wHour << "_" << system_time.wMinute << " log.txt";
	std::string filePath = stringStream.str();

	std::ofstream fout;
	fout.open(filePath);
	for (int i = 0; i < buffer.size(); i++) {
		std::cout << buffer[i];
		fout << buffer[i];	
	}
	fout.close();
	std::cout << filePath << std::endl;

}

void Server::start() {
	listen(listeningSocket, SOMAXCONN); //В константе SOMAXCONN хранится максимально возможное число одновременных TCP-соединений
	isActive = true;
	while (isActive) {
		sockaddr_in clientSocketHint;
		int clientSocketHintSize = sizeof(clientSocketHint);
		SOCKET clientSocket = accept(listeningSocket, (sockaddr *)&clientSocketHint, &clientSocketHintSize); //Функция accept ожидает запрос на установку TCP-соединения от удаленного хоста. В качестве аргумента ей передается дескриптор слушающего сокета.
																											// При успешной установке TCP - соединения, для него создается новый сокет.Функция accept возвращает дескриптор этого сокета.
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Client could't connect, Err #" << WSAGetLastError() << std::endl;
		}
		char *clientIP = inet_ntoa(clientSocketHint.sin_addr);
		Connection *connection = new Connection(clientSocket, clientIP);
		connections.push_back(connection); // добавление элемента в конец вектора коннектов
		std::thread connectionThread(&Connection::clientProcessing, connection);
		connectionThread.detach();
	}
}

void Server::addMessage(std::string message) {
	mutex.lock();
	buffer.push_back(message); // add message to the end of vectors array
	mutex.unlock();
}

void Server::deleteConnection(Connection *connection) {
	 mutex.lock();
	for (std::vector<Connection*>::iterator eraseIterator = connections.begin(); eraseIterator < connections.end(); eraseIterator++) {
		if (*eraseIterator == connection) {
			connections.erase(eraseIterator);
			break;
		}
	} 
	mutex.unlock();
}

void Server::shutDownAllConnections() {
	 mutex.lock();
	for (int connectionIndex = 0; connectionIndex < connections.size(); connectionIndex++) {
		connections[connectionIndex]->closeSocket();
	}
	 mutex.unlock();
}