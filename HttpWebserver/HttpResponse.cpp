#include "pch.h"
#include "HttpResponse.h"
#include <string>

using namespace HttpWebServer;
using namespace HttpWebServer::DetailClasses;

HttpResponse::HttpResponse(bool isHeadMethod)
{
	mIsHeadMethod = isHeadMethod;
}

void HttpResponse::SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge)
{
	isSet = true;
	isData = true;
	typeData = 0;
	dataString = data;
	this->fileType = fileType;
	this->maxAge = maxAge;
}

void HttpResponse::SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge, Cookie ^cookie)
{
	isSet = true;
	isData = true;
	typeData = 0;
	dataString = data;
	this->fileType = fileType;
	this->maxAge = maxAge;

	if (cookie != nullptr) cookieTekst = cookie->SetText;
}

void HttpResponse::SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge, const Platform::Array<Cookie^> ^cookies)
{
	isSet = true;
	isData = true;
	typeData = 0;
	dataString = data;
	this->fileType = fileType;
	this->maxAge = maxAge;

	if (cookies != nullptr)
	{
		for (size_t i = 0; i < cookies->Length; i++)
		{
			cookieTekst += cookies[i]->SetText;
		}
	}
}

void HttpResponse::SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge)
{
	isSet = true;
	isData = true;
	typeData = 1;
	dataBuffer = data;
	this->fileType = fileType;
	this->maxAge = maxAge;
}

void HttpResponse::SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge, Cookie ^cookie)
{
	isSet = true;
	isData = true;
	typeData = 1;
	dataBuffer = data;
	this->fileType = fileType;
	this->maxAge = maxAge;

	if (cookie != nullptr) cookieTekst = cookie->SetText;
}

void HttpResponse::SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge, const Platform::Array<Cookie^> ^cookies)
{
	isSet = true;
	isData = true;
	typeData = 1;
	dataBuffer = data;
	this->fileType = fileType;
	this->maxAge = maxAge;

	if (cookies != nullptr)
	{
		for (size_t i = 0; i < cookies->Length; i++)
		{
			cookieTekst += cookies[i]->SetText;
		}
	}
}

void HttpResponse::SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge)
{
	isSet = true;
	isData = true;
	typeData = 2;
	this->file = file;
	this->maxAge = maxAge;
}

void HttpResponse::SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge, Cookie ^cookie)
{
	isSet = true;
	isData = true;
	typeData = 2;
	this->file = file;
	this->maxAge = maxAge;

	if (cookie != nullptr) cookieTekst = cookie->SetText;
}

void HttpResponse::SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge, const Platform::Array<Cookie^> ^cookies)
{
	isSet = true;
	isData = true;
	typeData = 2;
	this->file = file;
	this->maxAge = maxAge;

	if (cookies != nullptr)
	{
		for (size_t i = 0; i < cookies->Length; i++)
		{
			cookieTekst += cookies[i]->SetText;
		}
	}
}

void HttpResponse::SetUnauthorized(Platform::String ^name)
{
	isSet = true;
	isData = false;

	notDataTekst = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"";

	if (name->Length() < 100)
	{
		char buffer[100];
		Tekst::AnsiEncode(name->Data(), buffer);
		notDataTekst += buffer;
	}
	else
	{
		char *dBuffer = new char[name->Length() + 1];
		Tekst::AnsiEncode(name->Data(), dBuffer);
		notDataTekst += dBuffer;
		delete[] dBuffer;
	}

	notDataTekst += "\"\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Length: 0\r\n\r\n";
}

void HttpResponse::SetRedirect(Platform::String ^location, bool permanently)
{
	isSet = true;
	isData = false;

	if (permanently) notDataTekst = "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
	else notDataTekst = "HTTP/1.1 303 See Other\r\nLocation: ";

	if (location->Length() < 100)
	{
		char buffer[100];
		Tekst::AnsiEncode(location->Data(), buffer);
		notDataTekst += buffer;
	}
	else
	{
		char *dBuffer = new char[location->Length() + 1];
		Tekst::AnsiEncode(location->Data(), dBuffer);
		notDataTekst += dBuffer;
		delete[] dBuffer;
	}

	notDataTekst += "\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Length: 0\r\n\r\n";
}

void HttpResponse::SetNotFound()
{
	isSet = true;
	isData = false;

	std::string returHtml = "<html>\r\n<head>\r\n <title>404 Not Found</title>\r\n</head>\r\n<body>\r\n <h1>404 Not Found</h1>\r\n <p>No data were found.</p>\r\n</body>\r\n</html>";
	notDataTekst = "HTTP/1.1 404 Not Found\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\n\r\n";
	if (!mIsHeadMethod) notDataTekst += returHtml;
}