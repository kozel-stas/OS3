#include "Server.h"
#include "signal.h"

using namespace std;


int main() {
	//    server->start();
	 //   std::cout << "kek" << std::endl;
	auto&& server = Server::getInstance(); // for the ability to modify and discard values of the sequence within the loop. (That is, unless the container provides a read-only view, 
										  // such as std::initializer_list, in which case it will be effectively an auto const &.)
	signal(SIGINT, Server::interruption_handler);

	server.start();

	return 0;
}