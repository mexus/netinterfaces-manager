Network interfaces manager library
==================================

*libifman* is a C++ library for getting information about network
interfaces in a linux system via *netlink* socket family

Details of a building process are available at `CMakeLists.txt` files


Requirements
============

1. CMake for build library and a *watcher* example
2. Some C++11 functionality is used, so C++11 compliant compiler required.
3. Python 2.6+ for running tests

Tested with:
- CentOS 6.6: gcc 4.8.2, kernel `2.6.32-504.el6.x86_64`, cmake 2.8, python
2.6.6
- Arch Linux: gcc 4.9.2, kernel `3.17.4-1-ck`, cmake 3.0.2., python 2.7.8


Building
========

To build a library and a *watcher* example, cd into a `build` folder, then run

```
% cmake ..
% make
```


Running tests
=============

Build a project, then cd into a `test` folder. Adjust `test_device_name` and
commands for add/delete interface: `add_interface_cmd` and
`remove_interface_cmd`. For example, for CentOS 6.6 `add_interface_cmd`
should be changed to:

```python
add_interface_cmd = ["sudo", "/usr/sbin/tunctl", "-t", test_device_name]
```

Then run

```
% python2 test-monitor.py
```

If everething is OK, an ouptut will be:
```
Comparing devices lists OK
New device detected OK
Detected a 'gone' device OK
```

**Notice**: The script internally runs a `sudo` command to add/remove a net interface


Usage
=====

Build a project, then cd into a `build/watcher` folder, then run

```
% ./watcher
```

It will run until received SIGTERM or SIGINT signal, printing every 5 seconds a
list of net interfaces in a system, and an interface information when kernel
send messages about adding new or removing an existent net interface.

You can additionally supply next command line arguments:

|Argument   |Action                                                |
|-----------|------------------------------------------------------|
|--list-once|Print a list of interfaces only once                  |
|--no-watch |Don't receive kernel messages about interfaces updates|
|--no-list  |Don't print a list of interfaces                      |

- When run with `--no-watch` and `--no-list` simultaneously, program will exit
immediately
- When run with `--no-watch` and `--list-once` simultaneously, program will exit
after printing a list of network interfaces

