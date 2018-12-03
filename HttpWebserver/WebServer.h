#pragma once
#include <ws2tcpip.h>
#include <atomic>
#include <string>
#include "RequestEventArgs.h"

// Statusmeldinger mottak
#define Status_OK 0
#define Status_Bad 1
#define Status_Bad_ToBig 2
#define Status_Bad_LengthMissing 3
#define Status_Bad_Closed 4

namespace HttpWebServer
{
	/// <summary>Create http web server. Remember to allow server and private networks connections in capabilities on Package.appxmanifest file.</summary>
	public ref class WebServer sealed
	{
	public:
		/// <summary>Default constructor web server. Listens on port 80, has a maximum content size of 10 mb and maximum number of socket 16.</summary>
		WebServer();

		/// <summary>Constructor web server. Set listening port, maximum content size and max count socket.</summary>
		WebServer(uint16 port, uint8 maxContentSize, uint8 maxSocketCount);

		/// <summary>Number of connected now.</summary>
		property int32 ConnectedCount
		{
			int32 get()
			{
				return antallOppkoblet;
			}
		}

		/// <summary>Number of connections since the Web server started.</summary>
		property uint32 ConnectionsCount
		{
			uint32 get()
			{
				return antallOppkoblinger;
			}
		}

		/// <summary>Number of bad request with errors in the header or content.</summary>
		property uint32 BadRequestCount
		{
			uint32 get()
			{
				return antallBadRequest;
			}
		}

		/// <summary>The number of requests received.</summary>
		property uint64 NumberOfRequests
		{
			uint64 get()
			{
				return antallRequestMotatt;
			}
		}

		/// <summary>Number of bytes received from client.</summary>
		property uint64 BytesReceived
		{
			uint64 get()
			{
				return antallBytesMottatt;
			}
		}

		/// <summary>Number of bytes sent to client.</summary>
		property uint64 BytesSent
		{
			uint64 get()
			{
				return antallBytesSendt;
			}
		}

		/// <summary>To check if the web server is running.</summary>
		property bool IsRunning
		{
			bool get()
			{
				return isRunning;
			}
		}

		/// <summary>Clear counters ConnectedCount, ConnectionsCount, BadRequestCount, NumberOfRequests, BytesReceivedCount and BytesSentCount.</summary>
		void ClearCounters();

		/// <summary>Event that raises when it comes valid http request. Response settings must not be driven asynchronously with await (is the same as that nothing is set).</summary>
		event Windows::Foundation::EventHandler<RequestEventArgs^> ^HttpRequest;

	private:
		void LyttEtterForesporsler(const unsigned short port, const size_t maxContentSize, const int maxSocketCount);
		void WebForesporselMotatt(const SOCKET socket, const size_t maxContentSize, sockaddr_in SenderAddr);
		std::atomic_int antallOppkoblet = 0;
		std::atomic_uint antallOppkoblinger = 0;
		std::atomic_uint antallBadRequest = 0; // Antall feil oppkoblinger (regnes ikke hvis oppkobling plutselig lukkes)
		std::atomic_ullong antallRequestMotatt = 0;
		std::atomic_ullong antallBytesMottatt = 0;
		std::atomic_ullong antallBytesSendt = 0;
		bool isRunning = false;
		std::string FaaContentTypeBeskrivelse(FileTypes fileType);
	};
}
