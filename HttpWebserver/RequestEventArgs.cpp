#include "pch.h"
#include "RequestEventArgs.h"

using namespace HttpWebServer;

RequestEventArgs::RequestEventArgs(DetailClasses::HttpRequest ^httpRequest, DetailClasses::HttpResponse ^httpResponse, Platform::Object ^tempConnectionData, bool keepAlive)
{
	request = httpRequest;
	response = httpResponse;
	this->tempConnectionData = tempConnectionData;
	this->keepAlive = keepAlive;
}