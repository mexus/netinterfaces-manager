#include <libifman/interface_manager.h>
#include <csignal>
#include <iostream>

std::atomic_bool running(true);

void SignalHandler(int){
	running = false;
}

int main(){
	using namespace libifman;
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);
	InterfaceManager::Run(running, {
			{RTM_DELLINK, [](const Interface& interface){
				std::cout << "Removed interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ")\n";
			}},
			{RTM_NEWLINK, [](const Interface& interface){
				std::cout << "Added interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ")\n";
			}}
		});
	return 0;
}

