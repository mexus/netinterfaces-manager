#!/usr/bin/python2

import subprocess
import re
import time

testExecutable = "../build/watcher/watcher"
ipExecutable = "/sbin/ip"
test_device_name = "eth_test"
sudo = "sudo"

def interfaces_ip():
    "Get interfaces list from ip"
    pattern = re.compile(r"^[0-9]+:\s+(\w+)[:@]")
    p = subprocess.Popen([ipExecutable, "link"], stdout=subprocess.PIPE)
    f = p.stdout
    interfaces = []
    for line in f:
        match = pattern.match(line.strip())
        if match:
            interfaces.append(match.group(1))
    return interfaces

def interfaces_test():
    "Get interfaces list from test"
    pattern = re.compile(r"^IFACE\s+(\w+)\s+")
    p = subprocess.Popen([testExecutable, "--no-watch", "--list-once"], stdout=subprocess.PIPE)
    f = p.stdout
    interfaces = []
    for line in f:
        match = pattern.match(line.strip())
        if match:
            interfaces.append(match.group(1))
    return interfaces

def test_interfaces_list():
    "Compare devices lists"
    list1 = interfaces_ip()
    list2 = interfaces_test()
    if set(list1) == set(list2):
        print "Comparing devices lists:"
    else:
        print "[FAILURE] Got devices: ", list2, ", but should be: ", list1

def test_add_device():
    "Test detection of a new net device"
    p = subprocess.Popen([testExecutable, "--no-list"], stdout=subprocess.PIPE)
    subprocess.call([sudo, ipExecutable, "tuntap", "add", test_device_name, "mode", "tap"])
    time.sleep(0.3)
    p.terminate()
    for line in p.stdout:
        if re.match("^NEW " + test_device_name, line.strip()):
            print "New device detected"
            return 
    print "[FAILURE] New device hasn't been detected"

def test_remove_device():
    "Test detection of a net device that has gone"
    p = subprocess.Popen([testExecutable, "--no-list"], stdout=subprocess.PIPE)
    subprocess.call([sudo, ipExecutable, "tuntap", "del", test_device_name, "mode", "tap"])
    time.sleep(0.3)
    p.terminate()
    for line in p.stdout:
        if re.match("^GONE " + test_device_name, line.strip()):
            print "Detected a 'gone' device"
            return 
    print "[FAILURE] 'Gone' device hasn't been detected"

test_interfaces_list()
test_add_device()
test_remove_device()

