#pragma once

#include "DataHenter.h"
#include "RequestMethods.h"
#include "HttpCredential.h"
#include "Cookie.h"
#include "FormData.h"
#include "FormFile.h"

namespace HttpWebServer
{
	namespace DetailClasses
	{
		/// <summary>Data for http request.</summary>
		public ref class HttpRequest sealed
		{
		public:
			/// <summary>Request method used. If actual method is HEAD will this be displayed as GET. When sending content will not be sent.</summary>
			property RequestMethods RequestMethod
			{
				RequestMethods get()
				{
					return requestMethod;
				}
			}

			/// <summary>Request uri. Always starts with /. If / is added to the end of the address in the browser gets this with as if it is part of the address. To prevent mistakes here with manual input can be trimmed away or it can be checked double up. Remember also that the address can be written with uppercase and lowercase letters.</summary>
			property Platform::String ^Uri
			{
				Platform::String^ get()
				{
					return uri;
				}
			}

			/// <summary>Type of document client accepts.</summary>
			property Platform::String ^Accept
			{
				Platform::String^ get()
				{
					return accept;
				}
			}

			/// <summary>What language client accepts.</summary>
			property Platform::String ^AcceptLanguage
			{
				Platform::String^ get()
				{
					return acceptLanguage;
				}
			}

			/// <summary>The domain name for the request.</summary>
			property Platform::String ^Host
			{
				Platform::String^ get()
				{
					return host;
				}
			}

			/// <summary>Text to detect the browser and version used.</summary>
			property Platform::String ^UserAgent
			{
				Platform::String^ get()
				{
					return userAgent;
				}
			}

			/// <summary>Authentication credentials for HTTP authentication. Can be null.</summary>
			property HttpCredential ^Authorization
			{
				HttpCredential^ get()
				{
					return authorization;
				}
			}

			/// <summary>HTTP cookies previously sent by the server to client. Can be null.</summary>
			property Platform::Array<Cookie^> ^Cookies
			{
				Platform::Array<Cookie^>^ get()
				{
					return cookies;
				}
			}

			/// <summary>Formdatas from client. Can be null.</summary>
			property Platform::Array<FormData^> ^FormDatas
			{
				Platform::Array<FormData^>^ get()
				{
					return formDatas;
				}
			}

			/// <summary>Formdata files from client.  Can be null.</summary>
			property Platform::Array<FormFile^> ^FormFiles
			{
				Platform::Array<FormFile^>^ get()
				{
					return formFiles;
				}
			}

			/// <summary>IP address of the client. Byte array with 4 bytes.</summary>
			property Platform::Array<uint8> ^IPAddress
			{
				Platform::Array<uint8>^ get()
				{
					return ipAddress;
				}
			}

			/// <summary>Is true if UserAgent contains word Mobile.</summary>
			property bool IsMobileBrowser
			{
				bool get();
			}

		internal:
			HttpRequest(HttpWebServer::HelpClasses::DataHenter &dataHenter, unsigned char *iPAdresseClient);

		private:
			RequestMethods requestMethod;
			Platform::String ^uri;
			Platform::String ^accept;
			Platform::String ^acceptLanguage;
			Platform::String ^host;
			Platform::String ^userAgent;
			HttpCredential ^authorization;
			Platform::Array<Cookie^> ^cookies;
			Platform::Array<FormData^> ^formDatas;
			Platform::Array<FormFile^> ^formFiles;
			Platform::Array<uint8> ^ipAddress;
		};
	}
}