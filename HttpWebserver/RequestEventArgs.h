#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace HttpWebServer
{
	/// <summary>EventArgs for http request.</summary>
	public ref class RequestEventArgs sealed
	{
	public:
		/// <summary>Class containing all data about the http request.</summary>
		property DetailClasses::HttpRequest ^Request
		{
			DetailClasses::HttpRequest^ get()
			{
				return request;
			}
		}

		/// <summary>Class to set http response data to client.</summary>
		property DetailClasses::HttpResponse ^Response
		{
			DetailClasses::HttpResponse^ get()
			{
				return response;
			}
		}

		/// <summary>Temporary variable for storing data if a new request comes up on the same connection.</summary>
		property Platform::Object ^TempConnectionData
		{
			Platform::Object^ get()
			{
				return tempConnectionData;
			}
			void set(Platform::Object ^value)
			{
				tempConnectionData = value;
			}
		}

		/// <summary>Can be used to close the connection after the response is sent. To set a false value to true has no effect.</summary>
		property bool KeepAlive
		{
			bool get()
			{
				return keepAlive;
			}
			void set(bool value)
			{
				if (!value) keepAlive = value;
			}
		}

	internal:
		RequestEventArgs(DetailClasses::HttpRequest ^httpRequest, DetailClasses::HttpResponse ^httpResponse, Platform::Object ^tempConnectionData, bool keepAlive);
		Platform::Object ^tempConnectionData;
		bool keepAlive;

	private:
		DetailClasses::HttpRequest ^request;
		DetailClasses::HttpResponse ^response;
	};
}

