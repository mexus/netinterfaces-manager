#ifndef LIBIFMAN_INTERFACE_MANAGER_H
#define LIBIFMAN_INTERFACE_MANAGER_H

#include "interface.h"
#include <unistd.h>
#include <atomic>
#include <sys/socket.h>
#include <unordered_map>
#include <functional>
#include <thread>
#include <memory>
#include <vector>

namespace libifman {

class InterfaceManager { 
public:
	typedef std::unordered_map<unsigned short, std::function<void(const Interface&)>> Callbacks;

	InterfaceManager();
	~InterfaceManager();
	//GetList();
	void Watch(const std::atomic_bool& running, const Callbacks& = Callbacks());

private:
	static const useconds_t waitInterval = 250000;
	static const unsigned short iflistReplyBuffer = 8192;
	std::atomic_bool interrupt;

	static void ProcessMessage(const msghdr& message, ssize_t &receivedLength, const Callbacks&);
	static sockaddr_nl PrepareNetLinkClient();
	void Run(const std::atomic_bool& running, Callbacks = Callbacks());

	std::vector<std::thread> runningThreads;
};

}

#endif /* LIBIFMAN_INTERFACE_MANAGER_H */
