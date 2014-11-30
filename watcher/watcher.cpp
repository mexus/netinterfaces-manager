#include <libifman/interface_manager.h>
#include <csignal>
#include <iostream>
#include <mutex>

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
		std::mutex coutMutex;
	        manager.Watch(running, {
				{RTM_DELLINK, [&coutMutex](const Interface& interface){
					std::lock_guard<std::mutex> lg(coutMutex);
					std::cout << "GONE " << interface.name << " " << interface.address << " " << interface.Type() << "\n";
				}},
				{RTM_NEWLINK, [&coutMutex](const Interface& interface){
					std::lock_guard<std::mutex> lg(coutMutex);
					std::cout << "NEW " << interface.name << " " << interface.address << " " << interface.Type() << "\n";
				}}
			});
		while (running){
		        manager.GetList(running, [&coutMutex](const Interface& interface){
					std::lock_guard<std::mutex> lg(coutMutex);
					std::cout << "IFACE " << interface.name << " " << interface.address << " " << interface.Type() << "\n";
				});
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
		return 0;
	} catch (const std::exception& e){
		std::cerr << "Caught an exception: " << e.what() << "\n";
		return 1;
	}
}

