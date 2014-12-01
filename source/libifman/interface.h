#ifndef LIBIFMAN_INTERFACE_H
#define LIBIFMAN_INTERFACE_H

#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace libifman {

class Interface {
public:
	Interface(const ifinfomsg*, int attributesLength);

	const int index;
	const unsigned short type;
	std::string name, address;
	
	std::string Type() const;

private:
	static const std::unordered_map<int, std::string> arpsNames;
	static std::unordered_map<unsigned short, const rtattr*>
		ParseRAttributes(const rtattr *firstAttribute, int attributesLength, const std::unordered_set<unsigned short>& lookupTypes);
	static std::string TypeToStr(int);
	static std::string L2Address(const unsigned char* str, unsigned int size);
};

}

#endif /* LIBIFMAN_INTERFACE_H */
