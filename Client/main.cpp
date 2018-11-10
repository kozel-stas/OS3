
#pragma comment(lib, "Ws2_32.lib")
#pragma(push)
#pragma warning ( disable : 4789 )
#include <iostream>
#include <winsock2.h> 
#include <ws2tcpip.h>
#include <time.h>
#include "Windows.h"
#include <inaddr.h>
#include <string>

#pragma warning(disable:4996) 

using namespace std;

const char *ar[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s",
					"t", "u", "v", "w", "x", "y", "z" };

int getRandomSleepTime() {
	int sleepTime = rand() % 10 + 2;
	return sleepTime * 1000; // to mili???
}

string generateRandomString() {
	string userInput;
	srand(time(NULL));
	int n = rand() % 100;
	for (int i = 0; i < n; i++) userInput += ar[rand() % (sizeof ar / sizeof(char *))];
	userInput += "\0";
	return userInput;
}


int startClient(const char *ip) {
	int port = 5223;

		//Initialize WinSock
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wsResult = WSAStartup(version, &wsData);
	if (wsResult != 0) {
		cerr << "Can't start WinSock, Err #" << wsResult << endl;
		return -1;
	}

	//Create socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		return -1;
	}

	//Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET; // we use AF_INET for ip
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = inet_addr(ip);

	char buffer[10000];
	string userInput = generateRandomString();


	//Connect to server
	int connectResult = connect(clientSocket, (sockaddr *)&hint, sizeof(hint));
	if (connectResult == SOCKET_ERROR) {
		cout << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	int random = getRandomSleepTime();
	cout << random << endl;
	Sleep(random);
	cout << userInput << endl;

	int sendResult = send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);

	if (sendResult != SOCKET_ERROR) {
		//Wait for response
		ZeroMemory(buffer, 10000);
	
		int byteReceived = recv(clientSocket, buffer, 5000, 0);
		if (byteReceived > 0) {
			//Echo response to console
			cerr << "SERVER > " << string(buffer, 0, byteReceived) << endl;
		}
	}

	random = getRandomSleepTime();
	Sleep(random);

	closesocket(clientSocket);
	WSACleanup();
	return 0;
}


int main(int argc, char* argv[]) {
	startClient(argv[1]);
	system("Pause");
}