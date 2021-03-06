#include "interface_manager.h"
#include <helpers/socket.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <cstring>
#include <unistd.h>
#include <iostream>

namespace libifman {


const unsigned short InterfaceManager::waitIntervalMs;
const unsigned short InterfaceManager::iflistReplyBuffer;

InterfaceManager::InterfaceManager() : interrupt(false){
}

InterfaceManager::~InterfaceManager(){
	interrupt = true;
	for (auto& thread : runningThreads){
		if (thread.joinable())
			thread.join();
	}
}

void InterfaceManager::ProcessMessage(const msghdr& message, ssize_t msgLen, const InnerCallbacks& callbacks) {
        auto reply = message.msg_iov->iov_base;
        for (auto header = static_cast<nlmsghdr*>(reply); NLMSG_OK(header, msgLen); header = NLMSG_NEXT(header, msgLen)){
		auto it = callbacks.find(header->nlmsg_type);
		if (it != callbacks.end())
                        it->second(header);
	}
}

void InterfaceManager::Watch(const std::atomic_bool& running, const Callbacks& callbacks) {
	std::thread t([this, &running, callbacks]{ Run(running, std::move(callbacks));});
	runningThreads.push_back(std::move(t));
}

void InterfaceManager::Run(const std::atomic_bool& running, Callbacks callbacks) {
	Socket socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	auto local = PrepareNetLinkClient(RTNLGRP_LINK);
	socket.Bind(&local);

	char buf[iflistReplyBuffer] = {0};
	iovec iov{buf, sizeof(buf)};
	msghdr message;
	memset(&message, 0, sizeof(message));
	message.msg_name = &local;
	message.msg_namelen = sizeof(local);
	message.msg_iov = &iov;
	message.msg_iovlen = 1;

        InnerCallbacks innerCallbacks;
        for (auto &pair : callbacks){
                auto &callback = pair.second;
                innerCallbacks.insert({
                                      {pair.first, [&callback](const nlmsghdr* header){
                                        auto len = header->nlmsg_len;
                                        Interface interface(static_cast<ifinfomsg*>(NLMSG_DATA(header)), len);
                                        callback(interface);
                                }}
                         });
        }

	while (running && !interrupt) {
		ssize_t receivedLength = socket.ReceiveMessage(message, MSG_DONTWAIT);
		if (receivedLength < 0) {
			int error = errno;
			if (error != EINTR && error != EAGAIN)
				std::cerr << "Error while receiving netlink message: #" << error << std::strerror(error) << "\n";
		} else if (message.msg_namelen != sizeof(local))
			std::cerr << "Incorrect message length\n";
		else
			ProcessMessage(message, receivedLength, innerCallbacks);
		std::this_thread::sleep_for(std::chrono::milliseconds(waitIntervalMs));
	}
}

struct nl_request {
        struct nlmsghdr hdr;
        struct rtgenmsg gen;
};

std::vector<Interface> InterfaceManager::GetList(const std::atomic_bool& running) {//, const std::function<void(const Interface&)>& callback) {
	Socket socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	auto local = PrepareNetLinkClient(0);
	socket.Bind(&local);

        auto kernel = PrepareNetLinkClient(0, false);
        kernel.nl_pid = 0;

        nl_request request;
        memset(&request, 0, sizeof(request));

        request.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
        request.hdr.nlmsg_type = RTM_GETLINK;
        request.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
        request.hdr.nlmsg_seq = 1;
        request.hdr.nlmsg_pid = Pid();
        request.gen.rtgen_family = AF_NETLINK;

        iovec io;
        io.iov_base = &request;
        io.iov_len = request.hdr.nlmsg_len;

        msghdr rtnl_msg;
        memset(&rtnl_msg, 0, sizeof(rtnl_msg));
        rtnl_msg.msg_iov = &io;
        rtnl_msg.msg_iovlen = 1;
        rtnl_msg.msg_name = &kernel;
        rtnl_msg.msg_namelen = sizeof(kernel);

        auto send = socket.SendMessage(rtnl_msg, MSG_DONTWAIT);
        if (send < 0) {
		int error = errno;
		std::cout << "Error while sending netlink message: #" << error << ": " << std::strerror(error) << "\n";
                return {};
        }

	char buf[iflistReplyBuffer] = {0};
	iovec iov{buf, sizeof(buf)};
	msghdr message;
	memset(&message, 0, sizeof(message));
	message.msg_name = &local;
	message.msg_namelen = sizeof(local);
	message.msg_iov = &iov;
	message.msg_iovlen = 1;

        std::vector<Interface> interfaces;

        bool done(false);
        do {
		ssize_t receivedLength = socket.ReceiveMessage(message, MSG_DONTWAIT);
		if (receivedLength < 0) {
			int error = errno;
			if (error != EINTR && error != EAGAIN)
				std::cerr << "Error while receiving netlink message: #" << error << ": " << std::strerror(error) << "\n";
                        break ;
		} else if (message.msg_namelen != sizeof(local))
			std::cerr << "Incorrect message length\n";
		else
			ProcessMessage(message, receivedLength, {
                                       {NLMSG_DONE, [&done](const nlmsghdr*){done = true;}},
                                       {RTM_NEWLINK, [&done, &interfaces](const nlmsghdr* header){
                                                auto len = header->nlmsg_len;
                                                interfaces.emplace_back(static_cast<ifinfomsg*>(NLMSG_DATA(header)), len);
                                        }}
                                       });
	} while (!done && !interrupt && running);
        return std::move(interfaces);
}

unsigned int InterfaceManager::Pid(){
        auto id = std::this_thread::get_id();
        return std::hash<std::thread::id>()(id) << 16 | getpid();
}

sockaddr_nl InterfaceManager::PrepareNetLinkClient(unsigned int groups, bool localPid){
	sockaddr_nl local;
	std::memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_groups = groups;
        if (localPid)
	        local.nl_pid = Pid();
	return local;
}

}

