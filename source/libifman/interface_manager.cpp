#include "interface_manager.h"
#include <helpers/socket.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <cstring>
#include <unistd.h>
#include <iostream>

namespace libifman {

InterfaceManager::InterfaceManager() : interrupt(false){
}

InterfaceManager::~InterfaceManager(){
	interrupt = true;
	for (auto& thread : runningThreads){
		if (thread.joinable())
			thread.join();
	}
}

void InterfaceManager::ProcessMessage(const msghdr& message, ssize_t &receivedLength, const Callbacks& callbacks){
	for (auto header = static_cast<nlmsghdr*>(message.msg_iov->iov_base); receivedLength >= static_cast<ssize_t>(sizeof(*header)); ){
		auto len = header->nlmsg_len;
		if (len < sizeof(*header) || len > receivedLength)
			std::cerr << "Incorrect message length: " << len << "\n";
		else {
			Interface interface(static_cast<ifinfomsg*>(NLMSG_DATA(header)), len);

			auto it = callbacks.find(header->nlmsg_type);
			if (it != callbacks.end())
				it->second(interface);
			receivedLength -= NLMSG_ALIGN(len);
			header = reinterpret_cast<nlmsghdr*>(reinterpret_cast<char*>(header) + NLMSG_ALIGN(len));
		}
	}
}

void InterfaceManager::Watch(const std::atomic_bool& running, const Callbacks& callbacks) {
	std::thread t([this, &running, callbacks]{ Run(running, std::move(callbacks));});
	runningThreads.push_back(std::move(t));
}

void InterfaceManager::Run(const std::atomic_bool& running, Callbacks callbacks) {
	Socket socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	auto local = PrepareNetLinkClient();
	socket.Bind(&local);

	char buf[iflistReplyBuffer] = {0};
	iovec iov{buf, sizeof(buf)};
	msghdr message;
	memset(&message, 0, sizeof(message));
	message.msg_name = &local;
	message.msg_namelen = sizeof(local);
	message.msg_iov = &iov;
	message.msg_iovlen = 1;

	while (running && !interrupt) {
		ssize_t receivedLength = socket.ReceiveMessage(message, MSG_DONTWAIT);
		if (receivedLength < 0) {
			int error = errno;
			if (error != EINTR && error != EAGAIN)
				std::cerr << "Error while receiving netlink message: #" << error << std::strerror(error) << "\n";
		} else if (message.msg_namelen != sizeof(local))
			std::cerr << "Incorrect message length\n";
		else
			ProcessMessage(message, receivedLength, callbacks);
		usleep(waitInterval);
	}
}

sockaddr_nl InterfaceManager::PrepareNetLinkClient(){
	sockaddr_nl local;
	std::memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_groups = RTNLGRP_LINK;
	local.nl_pid = getpid();
	return local;
}

}

