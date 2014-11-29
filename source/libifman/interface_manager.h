#ifndef LIBIFMAN_INTERFACE_MANAGER_H
#define LIBIFMAN_INTERFACE_MANAGER_H

#include "interface.h"
#include <unistd.h>
#include <atomic>
#include <sys/socket.h>
#include <unordered_map>
#include <functional>

namespace libifman {

class InterfaceManager { 
public:
	typedef std::unordered_map<unsigned short, std::function<void(const Interface&)>> Callbacks;

	InterfaceManager();
	//GetList();
	static void Run(const std::atomic_bool& running, const Callbacks& = Callbacks());

private:
	static const useconds_t waitInterval = 250000;
	static const unsigned short iflistReplyBuffer = 8192;

	static void ProcessMessage(const msghdr& message, ssize_t &receivedLength, const Callbacks&);
	static sockaddr_nl PrepareNetLinkClient();
};

}

#endif /* LIBIFMAN_INTERFACE_MANAGER_H */
