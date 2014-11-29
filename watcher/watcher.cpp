#include <libifman/interface_manager.h>
#include <csignal>

std::atomic_bool running(true);

void SignalHandler(int){
	running = false;
}

int main(){
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);
	libifman::InterfaceManager::Run(running);
	return 0;
}

