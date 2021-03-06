#include "interface.h"
#include <sstream>
#include <iomanip>
#include <net/if_arp.h>

namespace libifman {

Interface::Interface(const ifinfomsg* ifi, int attributesLength) : type(ifi->ifi_type)
{
	auto attributes = ParseRAttributes(IFLA_RTA(ifi), attributesLength, {IFLA_IFNAME, IFLA_ADDRESS});
	auto it = attributes.find(IFLA_IFNAME);
	if (it != attributes.end())
		name.assign(static_cast<const char*>(RTA_DATA(it->second)), RTA_PAYLOAD(it->second) - 1);
	it = attributes.find(IFLA_ADDRESS);
	if (it != attributes.end())
		address = L2Address(static_cast<const unsigned char*>(RTA_DATA(it->second)), RTA_PAYLOAD(it->second));
}

std::string Interface::Type() const {
	return TypeToStr(type);
}

std::string Interface::L2Address(const unsigned char* str, unsigned int size) {
	std::stringstream res;
	for (unsigned int i = 0; i != size; ++i){
		if (i != 0) res << ":";
		res << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(str[i]);
	}
	return res.str();
}

const std::unordered_map<int, std::string> Interface::arpsNames
#include "arps_names.inc"

std::string Interface::TypeToStr(int t){
	auto it = arpsNames.find(t);
	if (it == arpsNames.end())
		return std::to_string(t);
	else
		return it->second;
}

std::unordered_map<unsigned short, const rtattr*>
Interface::ParseRAttributes(const rtattr *attribute, int attributesLength, const std::unordered_set<unsigned short>& lookupTypes) {
	std::unordered_map<unsigned short, const rtattr*> res;
	while (RTA_OK(attribute, attributesLength)) {
		auto type = attribute->rta_type;
		if (lookupTypes.find(type) != lookupTypes.end())
			res[type] = attribute;
		attribute = RTA_NEXT(attribute, attributesLength);
	}
	return std::move(res);
}

const std::string& Interface::GetName() const {
	return name;
}

const std::string& Interface::GetAddress() const {
	return address;
}

}

