#include "catch.hpp"
#include "rest_server.h"
#include <unistd.h>

static std::string example_handler(const std::string request_type, const std::string accept_types, const std::string data_type, const std::string data, const std::map<std::string,std::string> queries)
{
	std::string response = (std::string)"Request Type: " + request_type + "\r\nAccept Types: " + accept_types + "\r\nData Type: " + data_type + "\r\nData: " + data + "\r\nQueries: ";

	for(auto x : queries)
		response += "\r\n\t" + x.first + '=' + x.second;

	std::string http_header1 = "HTTP/1.1 200 OK\r\nContent-Type: text/txt\r\nContent-Length: ";
	std::string http_header2 = "\r\nConnection: close\r\n\r\n";

	char len[65] = {0};
	sprintf(len, "%d", (unsigned int)response.size());
	
	return http_header1 + len + http_header2 + response;
}

TEST_CASE("RestServer - Endpoint Subscribe with cURL requests")
{
	RestServer rs;

	rs.subscribe("endpoint1", example_handler);

	std::string cmd = (std::string)"curl --silent '127.0.0.1/ssrftp/v1/endpoint1?q1=test1&q2=test2' --request GET --data '{\"username\":\"xyz\",\"password\":\"abc\"}'";

	FILE* run_cmd = popen(cmd.c_str(), "r");
	REQUIRE(run_cmd);
	usleep(1000000);

	char response[8193] = {0};

	size_t readsize = fread(response, sizeof(char), 8193, run_cmd);

	std::string correct_response = "Request Type: GET\r\nAccept Types: */*\r\nData Type: application/x-www-form-urlencoded\r\nData: {\"username\":\"xyz\",\"password\":\"abc\"}\r\nQueries: \r\n\tq1=test1\r\n\tq2=test2";

	CHECK(readsize == correct_response.size());
	CHECK(correct_response == response);

	pclose(run_cmd);
}

TEST_CASE("RestServer - Invalid Endpoint with cURL requests")
{
	RestServer rs;

	std::string cmd = (std::string)"curl --silent '127.0.0.1/ssrftp/v1/endpoint1?q1=test1&q2=test2' --request GET --data '{\"username\":\"xyz\",\"password\":\"abc\"}'";

	FILE* run_cmd = popen(cmd.c_str(), "r");
	REQUIRE(run_cmd);
	usleep(1000000);

	char response[8193] = {0};

	size_t readsize = fread(response, sizeof(char), 8193, run_cmd);

	std::string correct_response = "{\r\n\t\"code\": 400,\r\n\t\"message\": \"Invalid REST endpoint.\"\r\n}";

	CHECK(readsize == correct_response.size());
	CHECK(correct_response == response);

	pclose(run_cmd);
}
