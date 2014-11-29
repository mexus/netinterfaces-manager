#ifndef LIB_INTERFACE_MANAGER_H
#define LIB_INTERFACE_MANAGER_H

#include "interface.h"
#include <unistd.h>
#include <atomic>

namespace libifman {

class InterfaceManager { 
public:
	InterfaceManager();
	//GetList();
	static void Run(const std::atomic_bool& running);

private:
	static const useconds_t waitInterval = 250000;

	static void ProcessMessage(char *buf, ssize_t &receivedLength);
};

}

#endif /* LIB_INTERFACE_MANAGER_H */
