SET (TEST_EXE watcher)
add_executable(${TEST_EXE}
	watcher.cpp
)
target_link_libraries (${TEST_EXE}
	interface-manager
	pthread
)
