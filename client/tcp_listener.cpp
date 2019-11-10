#include "tcp_listener.h"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <unistd.h>

using boost::asio::ip::tcp;

// Constructor
tcpListener::tcpListener()
{

}

// Destructor
tcpListener::~tcpListener()
{

}

void tcpListener::Listen()
{

	char file_cstr[300] = {0};

	try 
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service,tcp::endpoint(tcp::v4(),TCP_LISTEN_PORT));

		while(filepath.empty())
		{
			tcp::socket socket(io_service);
			acceptor.accept(socket);
			boost::system::error_code ignored_error;
			boost::asio::read(socket, boost::asio::buffer(file_cstr, 300), ignored_error);
		}

	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}

}

//assuming that the filepath exists on the computer

std::ifstream& tcpListener::getPath()
{
	std::ifstream ifile;

	ifile.open(this->filepath.c_str(), std::ios::binary);

	if(!ifile)
	{
		std::cout << ("File " + this->filepath + " does not exist");
		return ifile;
	}
	else
	{
		return ifile;
	}
}
