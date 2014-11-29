#include "interface_manager.h"
#include <helpers/socket.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <cstring>
#include <unistd.h>
#include <iostream>

namespace libifman {

InterfaceManager::InterfaceManager(){
}

void InterfaceManager::ProcessMessage(char *buf, ssize_t &receivedLength){
	for (auto header = reinterpret_cast<nlmsghdr*>(buf); receivedLength >= static_cast<ssize_t>(sizeof(*header)); ){
		auto len = header->nlmsg_len;
		if (len < sizeof(*header) || len > receivedLength)
			std::cerr << "Incorrect message length: " << len << "\n";
		else {
			Interface interface(static_cast<ifinfomsg*>(NLMSG_DATA(header)), len);

			switch (header->nlmsg_type) {
				case RTM_DELLINK:
					std::cout << "Removed interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ") \n";
					break;
				case RTM_NEWLINK:
					std::cout << "New interface " << interface.name << " (" << interface.Type() << ", " << interface.address << ") \n";
					break;
				default:
					std::cout << "Unknown message type " << header->nlmsg_type << "\n";
			}
			receivedLength -= NLMSG_ALIGN(len);
			header = reinterpret_cast<nlmsghdr*>(reinterpret_cast<char*>(header) + NLMSG_ALIGN(len));
		}
	}
}

void InterfaceManager::Run(const std::atomic_bool& running){
	Socket socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

	char buf[8192] = {0};
	iovec iov{buf, sizeof(buf)};
	sockaddr_nl local;
	std::memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_groups = RTNLGRP_LINK;
	local.nl_pid = getpid();

	socket.Bind(&local);

	msghdr message;
	memset(&message, 0, sizeof(message));
	message.msg_name = &local;
	message.msg_namelen = sizeof(local);
	message.msg_iov = &iov;
	message.msg_iovlen = 1;

	while (running) {
		ssize_t receivedLength = socket.ReceiveMessage(message, MSG_DONTWAIT);
		if (receivedLength < 0) {
			int error = errno;
			if (error != EINTR && error != EAGAIN)
				std::cerr << "Error while receiving netlink message: #" << error << std::strerror(error) << "\n";
		} else if (message.msg_namelen != sizeof(local))
			std::cerr << "Incorrect message length\n";
		else
			ProcessMessage(buf, receivedLength);
		usleep(waitInterval);
	}
}

}

