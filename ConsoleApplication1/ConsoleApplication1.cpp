#include "pch.h"
#include <iostream>

using namespace utility;				// common utilities like string conversion
using namespace web;					// common features like URIs.
using namespace web::http;				// common HTTP functionality
using namespace web::http::client;		// HTTP client features
using namespace concurrency::streams;	// async streams

int main()
{

	// make shared pointer for filestream - template type of <ostream>
	auto fileStream = std::make_shared<ostream>();

	// open stream to output file // lambda[=] capture by value 
	pplx::task<void> requestTask = fstream::open_ostream(U("results.json")).then([&](ostream outFile)
	{
		// file to write to
		*fileStream = outFile;

		// create http_client to send request
		http_client client(U("http://www.mapquestapi.com/"));

		// build request URI and start request
		uri_builder builder(U("traffic/v2/incidents"));

		builder.append_query(U("key"), U("EMmnAiwykQhrjAkG2pGhFM2vrxYznUXP"));
		builder.append_query(U("boundingBox"), U("27.80,-97.39,27.58,-97.21")); // selena statue to bob hall pier coords
		return client.request(methods::GET, builder.to_string());
	})
	// Handle response headers arriving
	.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());

		// write response body into file
		return response.body().read_to_end(fileStream->streambuf());
	})

	// close the file stream
	.then([=](size_t)
	{
		return fileStream->close();
	});

	// wait for all I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception: %s\n", e.what());
	}

	return 0;

}
