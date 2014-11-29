#include "socket.h"

#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>

Socket::Socket(int domain, int type, int protocol) {
	if ((descriptor = socket(domain, type, protocol)) == -1){
		auto err = errno;
		throw std::runtime_error(ErrorMessage("can't create a socket", err));
	}
}

Socket::~Socket(){
	//descriptor can't be incorrect, because constructor throws on errors
	close(descriptor); 
}

void Socket::Bind(const sockaddr* address, socklen_t addressLength) const {
	if (bind(descriptor, address, addressLength) == -1) {
		auto err = errno;
		throw std::runtime_error(ErrorMessage("can't bind a name to a socket", err));
	}
}

ssize_t Socket::ReceiveMessage(msghdr& message, int flags) const {
	return recvmsg(descriptor, &message, flags);
}


std::string Socket::ErrorMessage(const std::string& msg, int error){
	return "SocketException: " + msg + " error #" + std::to_string(error) + ": " + std::strerror(error);
}

