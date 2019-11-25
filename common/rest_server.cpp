#include "rest_server.h"
#include <pthread.h>
#include <stdexcept>
#include <unistd.h>
#include <boost/asio.hpp>
#include <iostream>

static void* shared_memory[3];

typedef std::string string;

static string http_respond(string request, std::map<string, string(*)(const string, const string, const string, const string, const std::map<string,string>)>* handlers)
{
	string endpoint;
	size_t i = request.find_first_of(' ') + 1;
	while(i < request.size() && request.at(i) != ' ')
	{
		endpoint.push_back(request.at(i));
		i++;
	}
	
	if(endpoint.compare(0, 11, "/ssrftp/v1/") == 0)
		endpoint.erase(0, 11);
	else
		endpoint = "";

	std::map<string,string> queries;

	size_t endpoint_len = endpoint.find_first_of('?');
	i = endpoint_len + 1;
	while(i < endpoint.size())
	{
		string key;
		string value;

		while(i < endpoint.size() && endpoint.at(i) != '=')
		{
			key.push_back(endpoint.at(i));
			i++;
		}
		i++;
		while(i < endpoint.size() && endpoint.at(i) != '&')
		{
			value.push_back(endpoint.at(i));
			i++;
		}
		i++;

		queries[key] = value;
	}

	endpoint.erase(endpoint_len, endpoint.size() - endpoint_len);
	
	if((*handlers)[endpoint] != nullptr)
	{
		string request_type;
		i = 0;
		while(i < request.size() && request.at(i) != ' ')
		{
			request_type.push_back(request.at(i));
			i++;
		}

		string accept_types;
		i = request.find("Accept: ") + 8;
		while(i < request.size() && request.at(i) != '\r' && request.at(i) != '\n')
		{
			accept_types.push_back(request.at(i));
			i++;
		}

		string data_type;
		i = request.find("Content-Type: ") + 14;
		while(i < request.size() && request.at(i) != '\r' && request.at(i) != '\n')
		{
			data_type.push_back(request.at(i));
			i++;
		}

		string data = request;
		string content_length;
		i = request.find("Content-Length: ") + 16;
		while(i < request.size() && request.at(i) != '\r' && request.at(i) != '\n')
		{
			content_length.push_back(request.at(i));
			i++;
		}
		size_t data_size = atoi(content_length.c_str());
		data.erase(0, data.size() - data_size);

		return (*handlers)[endpoint](request_type, accept_types, data_type, data, queries);
	}

	return "HTTP/1.1 400 Bad Request\r\n\r\n{\r\n\t\"code\": 400,\r\n\t\"message\": \"Invalid REST endpoint.\"\r\n}";
}

static void* server_main_loop(void* args)
{
	bool* is_active = ((bool**)shared_memory)[0];
	std::map<string, string(*)(const string, const string, const string, const string, const std::map<string,string>)>* handlers = ((std::map<string, string(*)(const string, const string, const string, const string, const std::map<string,string>)>**)shared_memory)[1];
	unsigned short* port = ((unsigned short**)shared_memory)[2];

	boost::asio::io_service ios;
	boost::asio::ip::tcp::acceptor acc(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), *port));

	while(*is_active)
	{
		boost::asio::ip::tcp::socket skt(ios);
		acc.accept(skt);

		string request;
		char buf[8192];
		size_t readlen;

		readlen = skt.read_some(boost::asio::buffer(buf, 8192));

		for(size_t i = 0; i < readlen; i++)
			request.push_back(buf[i]);

		string response = http_respond(request, handlers);

		skt.write_some(boost::asio::buffer(response.c_str(), response.size()));

		skt.close();
	}

	pthread_exit(nullptr);
}

RestServer::RestServer()
{
	this->server_active = false;

	this->start();
}

RestServer::~RestServer()
{
	if(this->server_active)
		this->kill();
}

void RestServer::start(const unsigned short port)
{
	if(this->server_active)
		return;

	this->server_active = true;
	this->http_port = port;

	shared_memory[0] = (void*)&this->server_active;
	shared_memory[1] = (void*)&this->handlers;
	shared_memory[2] = (void*)&this->http_port;

	if(pthread_create(&this->thread_http, nullptr, &server_main_loop, nullptr))
	{
		this->server_active = false;
		throw std::runtime_error("Cannot create REST server thread");
	}
}

void RestServer::kill()
{
	if(this->server_active)
	{
		this->server_active = false;
		pthread_join(thread_http, nullptr);
	}
}

void RestServer::subscribe(const string endpoint, string (*handler)(const string, const string, const string, const string, const std::map<string,string>))
{
	this->handlers[endpoint] = handler;
}
