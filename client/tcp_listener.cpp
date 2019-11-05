#include "tcp_listener.h"

class tcpListener
{
	private:
	std::string filepath;

	public:
	tcpListener();
	~tcpListener();

	void Listen()
	{
		
		try 
		{
			boost::asio::io_service io_service;
			tcp:: acceptor acceptor(io_service,tcp::endpoint(tcp::v4(),49125));
		
			while(filepath.empty())
			{
				tcp::socket socket(io_service);
				acceptor.accept(socket) ;
				boost :: system::error_code ignored_error;
				boost::asio::read(socket, boost::asio::buffer(filepath), ignored_error);
			}

		}
		catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
			return;
		}
		
	}

	//assuming that the filepath exists on the computer
	std::ifstream* getPath()
	{
		std::ifstream* ifile;
		ifile->open(filepath, std::ios::binary);
				
		if(!*ifile)
		{
			std::cout << ("File " + filepath + " does not exist");
			return NULL;
		}
		else
		{
			return ifile;
		}
	}
