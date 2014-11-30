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
	InterfaceManager manager;
	try {
	        manager.Watch(running, {
				{RTM_DELLINK, [](const Interface& interface){
					std::cout << "Removed interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ")\n";
				}},
				{RTM_NEWLINK, [](const Interface& interface){
					std::cout << "Added interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ")\n";
				}}
			});
		std::this_thread::sleep_for(std::chrono::seconds(10));
	        manager.GetList(running, [](const Interface& interface){
				std::cout << "Interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ")\n";
			});
		return 0;
	} catch (const std::exception& e){
		std::cerr << "Caught an exception: " << e.what() << "\n";
		return 1;
	}
}

