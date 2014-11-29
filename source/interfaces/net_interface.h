#ifndef LIB_NET_INTERFACE_H
#define LIB_NET_INTERFACE_H

#include <linux/rtnetlink.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

class NetInterace {
public:
	NetInterace(ifinfomsg*, int attributesLength);

	const int index;
	const unsigned short type;
	std::string name, address;
	
	std::string Type() const;

private:
	static const std::unordered_map<int, std::string> arpsNames;
	static std::unordered_map<unsigned short, rtattr*>
		ParseRAttributes(rtattr *firstAttribute, int attributesLength, const std::unordered_set<unsigned short>& lookupTypes);
	static std::string TypeToStr(int);
	static std::string L2Address(const unsigned char* str, unsigned int size);
};

#endif /* LIB_NET_INTERFACE_H */
