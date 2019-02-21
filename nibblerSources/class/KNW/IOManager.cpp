#include "IOManager.hpp"

IOManager::IOManager(size_t thread_size)
		:
		io(new boost::asio::io_service),
		work_(*io){

	for (size_t index = 0; index < thread_size; ++index) {
		thread_group_io.create_thread( boost::bind( &IOManager::IORunner, this) );
	}
}

void IOManager::IORunner() {
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Start" << std::endl;
	io->run();
	std::cout << "[" << boost::this_thread::get_id() << "] Thread Finish" << std::endl;
}

boost::asio::io_service &IOManager::getIo() {
	return *io;
}

boost::thread_group &IOManager::getThreadGroup() {
	return thread_group;
}

IOManager::~IOManager() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	io->stop();
	thread_group.join_all();
}