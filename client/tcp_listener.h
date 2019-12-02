#ifndef __TCP__LISTENER__H__
#define __TCP__LISTENER__H__

#define TCP_LISTEN_PORT 49125

#include <string>
#include <fstream>

//listens for a file provided by user from previous setup
class tcpListener
{
	private:
	std::string filepath;

	public:
	tcpListener();
	~tcpListener();

	void Listen();
	//assuming that the filepath exists on the computer
	std::ifstream& getPath();
};

#endif
