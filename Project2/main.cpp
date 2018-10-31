#include "Server.h"
#include "signal.h"

using namespace std;
BOOL WINAPI ConsoleHandler(DWORD);

int main() {
	//    server->start();
	 //   std::cout << "kek" << std::endl;
	auto& server = Server::getInstance();
	signal(SIGINT, Server::interruption_handler);

	server.start();

	return 0;
}