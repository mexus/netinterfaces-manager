#ifndef LIB_INTERFACE_MANAGER_H
#define LIB_INTERFACE_MANAGER_H

#include "net_interface.h"

#include <unistd.h>

class InterfaceManager { 
public:
	InterfaceManager();
	//GetList();
	static void Run();

private:
	static const useconds_t waitInterval = 250000;

	static void ProcessMessage(char *buf, ssize_t &receivedLength);
};

#endif /* LIB_INTERFACE_MANAGER_H */
