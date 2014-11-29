#/usr/bin/python2

import re

arps_header = "/usr/include/net/if_arp.h"
output_file = "../source/interfaces/arps_names.inc"

filter_string = "#define ARPHRD_"
arp_name = re.compile(r"(ARPHRD_[a-zA-Z0-9_]+).+/\* (.+)\s*\*/")
arp_name_short = re.compile(r"(ARPHRD_[a-zA-Z0-9_]+).+")

arps_file = open(arps_header, 'r')
arps_dict = {}
for line in arps_file:
    if line.startswith(filter_string):
        match = arp_name.search(line)
        if match:
            arps_dict[match.group(1)] = match.group(2).rstrip('. ')
        else:
            match = arp_name_short.search(line)
            if match:
                arps_dict[match.group(1)] = match.group(1)
arps_file.close()

map_file = open(output_file, 'w')
first = True
print >>map_file, "{"
for key in arps_dict:
    if first: first = False
    else: print >>map_file, ", "
    print >>map_file, "".join(["\t{", key, ", \"", arps_dict[key], "\"}"]),
print >>map_file, "\n};"
map_file.close();

