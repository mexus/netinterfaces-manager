#ifndef LIB_INTERFACE_MANAGER_H
#define LIB_INTERFACE_MANAGER_H

#include "interface.h"
#include <unistd.h>
#include <atomic>
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

	static void ProcessMessage(char *buf, ssize_t &receivedLength, const Callbacks&);
};

}

#endif /* LIB_INTERFACE_MANAGER_H */
