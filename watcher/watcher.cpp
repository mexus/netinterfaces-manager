#include <libifman/interface_manager.h>
#include <csignal>
#include <iostream>
#include <mutex>
#include <cstring>

std::atomic_bool running(true);
std::mutex coutMutex;

void SignalHandler(int){
	running = false;
}

void InterfaceNew(const libifman::Interface& interface){
	std::lock_guard<std::mutex> lg(coutMutex);
	std::cout << "NEW " << interface.name << " " << interface.address << " " << interface.Type() << "\n";
}

void InterfaceGone(const libifman::Interface& interface){
	std::lock_guard<std::mutex> lg(coutMutex);
	std::cout << "GONE " << interface.name << "\n";
}

void InterfacePrint(const libifman::Interface& interface){
	std::lock_guard<std::mutex> lg(coutMutex);
	std::cout << "IFACE " << interface.name << " " << interface.address << " " << interface.Type() << "\n";
}

bool cmp(const char* lhs, const char* rhs){
	return strcmp(lhs, rhs) == 0;
}

int main(int argc, char** argv){
	using namespace libifman;
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);
	InterfaceManager manager;

	bool runWatch(true), runList(true), runListOnce(false);

	for (int i = 1; i < argc; ++i){
		const char* arg = argv[i];
		if (cmp(arg, "--list-once"))
			runListOnce = true;
		else if (cmp(arg, "--no-watch"))
			runWatch = false;
		else if (cmp(arg, "--no-list"))
			runList = false;
		else {
			std::cout << "Unrecognized option " << arg << "\n";
			return 1;
		}
	}
	if (!runWatch && !runList){
		std::cout << "Nothing to do\n";
		return 0;
	}

	try {
		if (runWatch)
		        manager.Watch(running, {{RTM_DELLINK, InterfaceGone}, {RTM_NEWLINK, InterfaceNew}});
		while (running){
			if (runList){
			        manager.GetList(running, InterfacePrint);
				if (runListOnce && !runWatch)
					break;
				else if (runListOnce)
					runList = false;
			}
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
		return 0;
	} catch (const std::exception& e){
		std::cerr << "Caught an exception: " << e.what() << "\n";
		return 1;
	}
}

