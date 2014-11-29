#ifndef HELPERS_SOCKET_H
#define HELPERS_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

class Socket {
public:
	Socket(int domain, int type, int protocol);
	~Socket();
	void Bind(const sockaddr* address, socklen_t addressLength) const;
	ssize_t ReceiveMessage(msghdr& message, int flags) const;

	template<class T>
	void Bind(const T* address) const {
		Bind(reinterpret_cast<const sockaddr*>(address), sizeof(T));
	}
private:
	int descriptor;

	static std::string ErrorMessage(const std::string& message, int error);
};

#endif /* HELPERS_SOCKET_H */

