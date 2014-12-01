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
	void GetList(const std::atomic_bool& running, const std::function<void(const Interface&)>&);
	void Watch(const std::atomic_bool& running, const Callbacks& = Callbacks());

private:
	typedef std::unordered_map<unsigned short, std::function<void(const nlmsghdr*)>> InnerCallbacks;
	static const unsigned short waitIntervalMs = 250;
	static const unsigned short iflistReplyBuffer = 8192;
	std::atomic_bool interrupt;

	static void ProcessMessage(const msghdr& message, ssize_t receivedLength, const InnerCallbacks&);
	static sockaddr_nl PrepareNetLinkClient(unsigned int groups, bool localPid = true);
	void Run(const std::atomic_bool& running, Callbacks = Callbacks());

        static unsigned int Pid();

	std::vector<std::thread> runningThreads;
};

}

#endif /* LIBIFMAN_INTERFACE_MANAGER_H */
