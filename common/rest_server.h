#ifndef __REST__SERVER__H__
#define __REST__SERVER__H__

#define DEFAULT_HTTP_PORT 80

#include <string>
#include <map>

/* Endpoint handler example prototype:
 * std::string endpoint_handler(const std::string request_type, const std::string accept_types,
 *                              const std::string data_type, const std::string data
 *                              const std::map<std::string,std::string> queries);
 */ 

class RestServer
{
public:
	RestServer();
	~RestServer();

	void start(const unsigned short port = DEFAULT_HTTP_PORT);
	void kill();

	void subscribe(const std::string endpoint, std::string (*handler)(const std::string, const std::string, const std::string, const std::string, const std::map<std::string,std::string>));

private:

	std::map<std::string, std::string(*)(const std::string, const std::string, const std::string, const std::string, const std::map<std::string,std::string>)> handlers;

	pthread_t thread_http;
	bool server_active;
	unsigned short http_port;
};

#endif
