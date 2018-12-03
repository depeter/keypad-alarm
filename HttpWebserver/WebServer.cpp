#include "pch.h"
#include "WebServer.h"
#include "DataHenter.h"
#include <ws2tcpip.h>
#include <wrl.h>
#include <robuffer.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace HttpWebServer;
using namespace HttpWebServer::HelpClasses;

WebServer::WebServer()
{
	// Starter task for lytteing etter forespøseler
	concurrency::task<void> lytteTask([this] { LyttEtterForesporsler(80, 10 * 1024 * 1024, 16); });
}

WebServer::WebServer(uint16 port, uint8 maxContentSize, uint8 maxSocketCount)
{
	// Starter thread for lytting etter forespøseler
	std::thread lytteThread([this, port, maxContentSize, maxSocketCount] { LyttEtterForesporsler(port, maxContentSize * 1024 * 1024, maxSocketCount > 0 ? maxSocketCount : 1); });
	lytteThread.detach();
}

// Nulstiller teller for logging av data flyt
void HttpWebServer::WebServer::ClearCounters()
{
	antallOppkoblet = 0;
	antallOppkoblinger = 0;
	antallBadRequest = 0;
	antallRequestMotatt = 0;
	antallBytesMottatt = 0;
	antallBytesSendt = 0;
}

// Funksjon som kjøres i egen thread og lytter etter web forespørsler
void WebServer::LyttEtterForesporsler(const unsigned short port, const size_t maxContentSize, const int maxSocketCount)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock
	if (iResult == NO_ERROR) // Initialize Winsock OK
	{
		SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a SOCKET to listent for new TCP connections
		if (ListenSocket != INVALID_SOCKET) // Create a SOCKET to listent for new TCP connections OK
		{
			sockaddr_in RecvAddr;
			RecvAddr.sin_family = AF_INET;
			RecvAddr.sin_port = htons(port);
			RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

			int iResult = bind(ListenSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr)); // Bind the socket to any address and the specified port
			if (iResult != SOCKET_ERROR)  // Bind the socket to any address and the specified port OK
			{
				iResult = listen(ListenSocket, SOMAXCONN); // Start listen to socket
				if (iResult != SOCKET_ERROR) // Start listen to socket OK
				{
					isRunning = true;
					while (true)
					{
						sockaddr_in SenderAddr;
						int SenderAddrSize = sizeof(SenderAddr);

						SOCKET clientSocket = accept(ListenSocket, (SOCKADDR*)&SenderAddr, &SenderAddrSize); // Accept a client socket
						if (clientSocket != INVALID_SOCKET)  // Accept a client socket OK
						{
							DWORD timeOut = 60000; // Setter timeout på oppkobling til 1 minutt
							setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeOut), sizeof(DWORD)); // Setter timeout på mottak
							setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeOut), sizeof(DWORD)); // Setter timeout på sending

							antallOppkoblet++;
							antallOppkoblinger++;

							// Starter ny socket oppkobling i ny thread og bruker lambda til å starte den. Når thread er startet startes lytting igjen pga, while løkke.
							std::thread foresporselThread([this, clientSocket, maxContentSize, SenderAddr]
							{
								WebForesporselMotatt(clientSocket, maxContentSize, SenderAddr);
								antallOppkoblet--;
							});
							foresporselThread.detach();

							// Skal ikke gå videre tillytting hvis maks antall socket er overskredet
							while (antallOppkoblet >= maxSocketCount)
							{
								std::this_thread::sleep_for(std::chrono::seconds(1));
							}
						}
						else if (WSAGetLastError() == WSANOTINITIALISED) break;
					}
					isRunning = false;
					closesocket(ListenSocket);
				}
			}
		}
		WSACleanup();
	}
}

// Funksjon som kjøres i egen tråd når en webforespørsel er mottatt
void WebServer::WebForesporselMotatt(const SOCKET socket, const size_t maxContentSize, sockaddr_in SenderAddr)
{
	unsigned char IPAdresseClient[4]; // IP adressen til klient
	IPAdresseClient[0] = SenderAddr.sin_addr.S_un.S_un_b.s_b1;
	IPAdresseClient[1] = SenderAddr.sin_addr.S_un.S_un_b.s_b2;
	IPAdresseClient[2] = SenderAddr.sin_addr.S_un.S_un_b.s_b3;
	IPAdresseClient[3] = SenderAddr.sin_addr.S_un.S_un_b.s_b4;

	bool keepAlive = false;

	// Buffring av data hvis det er en headkommando som blir kjørt. Dette for at noen browsere sjekker data med HEAD først og deretter bruker GET. Her er det unødvendig å lage data på nytt.
	DetailClasses::HttpRequest ^headHttpRequest = nullptr;
	DetailClasses::HttpResponse ^headHttpResponse = nullptr;
	Platform::Object ^tempConnectionData = nullptr;
	Windows::Foundation::DateTime headDateTime;

	do
	{
		DataHenter dataHenter(socket, maxContentSize); // Klasse som henter data og formaterer innholdet i dataene.
		antallBytesMottatt += static_cast<unsigned long long>(dataHenter.BytesMotatt);
		keepAlive = dataHenter.KeepAlive;

		if (dataHenter.HenteStatus == Status_OK) // Forespørsel motatt OK
		{
			antallRequestMotatt++;

			DetailClasses::HttpRequest ^httpRequest = ref new DetailClasses::HttpRequest(dataHenter, IPAdresseClient);
			DetailClasses::HttpResponse ^httpResponse;

			if (dataHenter.Method == 0 && headHttpRequest != nullptr && httpRequest->Uri == headHttpRequest->Uri && headDateTime.UniversalTime > Windows::Globalization::Calendar().GetDateTime().UniversalTime - ((headHttpResponse->maxAge + 1) * 10000000LL)) // Sjekker om data kan hentes fra headbuffer
			{
				bool erLik{ true };

				if (httpRequest->Host != headHttpRequest->Host) erLik = false;

				if (erLik)
				{
					if (httpRequest->Authorization != nullptr || headHttpRequest->Authorization != nullptr)
					{
						if (httpRequest->Authorization != nullptr && headHttpRequest->Authorization != nullptr)
						{
							if (httpRequest->Authorization->Username != headHttpRequest->Authorization->Username || httpRequest->Authorization->Password != headHttpRequest->Authorization->Password) erLik = false;
						}
						else erLik = false;
					}
				}

				if (erLik)
				{
					if (httpRequest->Cookies != nullptr || headHttpRequest->Cookies != nullptr)
					{
						if (httpRequest->Cookies != nullptr && headHttpRequest->Cookies != nullptr && httpRequest->Cookies->Length == headHttpRequest->Cookies->Length)
						{
							for (unsigned int i = 0; i < httpRequest->Cookies->Length; i++)
							{
								if (httpRequest->Cookies[i]->Name != headHttpRequest->Cookies[i]->Name || httpRequest->Cookies[i]->Value != headHttpRequest->Cookies[i]->Value)
								{
									erLik = false;
									break;
								}
							}
						}
						else erLik = false;
					}
				}
				
				if (erLik)
				{
					if (httpRequest->FormDatas != nullptr || headHttpRequest->FormDatas != nullptr)
					{
						if (httpRequest->FormDatas != nullptr && headHttpRequest->FormDatas != nullptr && httpRequest->FormDatas->Length == headHttpRequest->FormDatas->Length)
						{
							for (unsigned int i = 0; i < httpRequest->FormDatas->Length; i++)
							{
								if (httpRequest->FormDatas[i]->Name != headHttpRequest->FormDatas[i]->Name || httpRequest->FormDatas[i]->Value != headHttpRequest->FormDatas[i]->Value)
								{
									erLik = false;
									break;
								}
							}
						}
						else erLik = false;
					}
				}

				if (erLik)
				{
					httpRequest = headHttpRequest;
					httpResponse = headHttpResponse;
					headHttpRequest = nullptr;
					headHttpResponse = nullptr;
				}
				else
				{
					headHttpRequest = nullptr;
					headHttpResponse = nullptr;
					httpResponse = ref new DetailClasses::HttpResponse(dataHenter.Method == 4); // Oppretter respons klasse og setter isHead metode til true hvis metode er HEAD

					RequestEventArgs ^requestEventArgs = ref new RequestEventArgs(httpRequest, httpResponse, tempConnectionData, keepAlive);
					HttpRequest(this, requestEventArgs); // Sender HttpRequest event
					tempConnectionData = requestEventArgs->tempConnectionData;
					keepAlive = requestEventArgs->keepAlive;
				}
			}
			else // Kan ikke hentes fra Head buffer
			{
				headHttpRequest = nullptr;
				headHttpResponse = nullptr;
				httpResponse = ref new DetailClasses::HttpResponse(dataHenter.Method == 4); // Oppretter respons klasse og setter isHead metode til true hvis metode er HEAD

				RequestEventArgs ^requestEventArgs = ref new RequestEventArgs(httpRequest, httpResponse, tempConnectionData, keepAlive);
				HttpRequest(this, requestEventArgs); // Sender HttpRequest event
				tempConnectionData = requestEventArgs->tempConnectionData;
				keepAlive = requestEventArgs->keepAlive;

				if (dataHenter.Method == 4 && httpResponse->isSet && httpResponse->isData) // Buffrer head kommando til senere bruk hvis neste er en get med samme forespørsel
				{
					headHttpRequest = httpRequest;
					headHttpResponse = httpResponse;
					headDateTime = Windows::Globalization::Calendar().GetDateTime();
				}
			}

			if (httpResponse->isSet) // Event kommet i retur med data satt eller data er hentet fra Head buffer
			{
				if (httpResponse->isData) // Sjekker at det er data som er satt (Videresending, ikke funnet eller at authorising trengs)
				{
					size_t startRange = 0;
					size_t sluttRange = 0;
					bool harRanges = dataHenter.GetRanges(startRange, sluttRange);

					if (httpResponse->typeData == 0) // String type
					{
						size_t tekstLengde = Tekst::Utf8EncodeLength(httpResponse->dataString->Data());

						if (harRanges && startRange < tekstLengde && startRange <= sluttRange) // Oppgitt område skal sendes
						{
							if (sluttRange >= tekstLengde) sluttRange = tekstLengde - 1;

							std::string returHead = "HTTP/1.1 206 Partial Content\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(sluttRange - startRange + 1) + "\r\nContent-Range: bytes " + std::to_string(startRange) + "-" + std::to_string(sluttRange) + "/" + std::to_string(tekstLengde) + "\r\nContent-Type: " + FaaContentTypeBeskrivelse(httpResponse->fileType) + "\r\n" + httpResponse->cookieTekst + "\r\n";

							if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
							{
								char *returTekst = new char[tekstLengde + 1];
								Tekst::Utf8Encode(httpResponse->dataString->Data(), returTekst);

								int antallSendt = send(socket, returHead.c_str(), returHead.length(), 0);
								antallSendt += send(socket, returTekst + startRange, sluttRange - startRange + 1, 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
							else
							{
								int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
						}
						else
						{
							std::string returHead = "HTTP/1.1 200 OK\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(tekstLengde) + "\r\nContent-Type: " + FaaContentTypeBeskrivelse(httpResponse->fileType) + "\r\n" + httpResponse->cookieTekst + "\r\n";

							if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
							{
								char *returTekst = new char[returHead.length() + tekstLengde + 1];
								strcpy_s(returTekst, returHead.length() + tekstLengde + 1, returHead.c_str());

								Tekst::Utf8Encode(httpResponse->dataString->Data(), returTekst + returHead.length());
								
								int antallSendt = send(socket, returTekst, returHead.length() + tekstLengde, 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
							else
							{
								int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
						}
					}
					else if (httpResponse->typeData == 1) // Buffer type
					{
						if (harRanges && startRange < httpResponse->dataBuffer->Length && startRange <= sluttRange) // Oppgitt område skal sendes
						{
							if (sluttRange >= httpResponse->dataBuffer->Length) sluttRange = httpResponse->dataBuffer->Length - 1;

							std::string returHead = "HTTP/1.1 206 Partial Content\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(sluttRange - startRange + 1) + "\r\nContent-Range: bytes " + std::to_string(startRange) + "-" + std::to_string(sluttRange) + "/" + std::to_string(httpResponse->dataBuffer->Length) + "\r\nContent-Type: " + FaaContentTypeBeskrivelse(httpResponse->fileType) + "\r\n" + httpResponse->cookieTekst + "\r\n";

							if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
							{
								// Query the IBufferByteAccess interface.
								Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
								reinterpret_cast<IInspectable*>(httpResponse->dataBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

								// Retrieve the buffer data.
								byte *pBuffer = nullptr;
								bufferByteAccess->Buffer(&pBuffer);

								// Sender data
								int antallSendt = send(socket, returHead.c_str(), returHead.length(), 0);
								antallSendt += send(socket, reinterpret_cast<char*>(pBuffer) + startRange, sluttRange - startRange + 1, 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
							else
							{
								int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
						}
						else
						{
							std::string returHead = "HTTP/1.1 200 OK\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(httpResponse->dataBuffer->Length) + "\r\nContent-Type: " + FaaContentTypeBeskrivelse(httpResponse->fileType) + "\r\n" + httpResponse->cookieTekst + "\r\n";

							if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
							{
								// Query the IBufferByteAccess interface.
								Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
								reinterpret_cast<IInspectable*>(httpResponse->dataBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

								// Retrieve the buffer data.
								byte *pBuffer = nullptr;
								bufferByteAccess->Buffer(&pBuffer);

								// Sender data
								int antallSendt = send(socket, returHead.c_str(), returHead.length(), 0);
								antallSendt += send(socket, reinterpret_cast<char*>(pBuffer), httpResponse->dataBuffer->Length, 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
							else
							{
								int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
								if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
								else keepAlive = false;
							}
						}
					}
					else // File type
					{
						const size_t bufferSize = 8192;
						Platform::String ^navn = httpResponse->file->Name;

						try
						{
							Windows::Storage::Streams::IRandomAccessStreamWithContentType ^fil = concurrency::create_task(httpResponse->file->OpenReadAsync()).get(); // Åpne fil
							
							// Finner content type
							std::string contentType;
							if (fil->ContentType->Length() == 0) // Feil i IOT core slik at noen får contenttype som er tom. Det rettes her. Denne if del kan muligens slettes i senere versjoner av webserver for er der på grunn av en Microsoft bug som kun gjelder for IOT core
							{
								std::wstring filNavn = httpResponse->file->Name->Data();
								size_t filNavnLengde = filNavn.length();

								// Filtyper som ikke virket på v 10.0.10240.16384 er: html, js, jpg, png, svg, webm, htm, xml, json, zip, xps (alt annet blir octet stream)
								if (filNavnLengde > 5 && (filNavn[filNavnLengde - 1] == L'l' || filNavn[filNavnLengde - 1] == L'L') && (filNavn[filNavnLengde - 2] == L'm' || filNavn[filNavnLengde - 2] == L'M') && (filNavn[filNavnLengde - 3] == L't' || filNavn[filNavnLengde - 3] == L'T') && (filNavn[filNavnLengde - 4] == L'h' || filNavn[filNavnLengde - 4] == L'H') && filNavn[filNavnLengde - 5] == L'.') contentType = "text/html";
								else if (filNavnLengde > 3 && (filNavn[filNavnLengde - 1] == L's' || filNavn[filNavnLengde - 1] == L'S') && (filNavn[filNavnLengde - 2] == L'j' || filNavn[filNavnLengde - 2] == L'J') && filNavn[filNavnLengde - 3] == L'.') contentType = "application/javascript";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'g' || filNavn[filNavnLengde - 1] == L'G') && (filNavn[filNavnLengde - 2] == L'p' || filNavn[filNavnLengde - 2] == L'P') && (filNavn[filNavnLengde - 3] == L'j' || filNavn[filNavnLengde - 3] == L'J') && filNavn[filNavnLengde - 4] == L'.') contentType = "image/jpeg";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'g' || filNavn[filNavnLengde - 1] == L'G') && (filNavn[filNavnLengde - 2] == L'n' || filNavn[filNavnLengde - 2] == L'N') && (filNavn[filNavnLengde - 3] == L'p' || filNavn[filNavnLengde - 3] == L'P') && filNavn[filNavnLengde - 4] == L'.') contentType = "image/png";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'g' || filNavn[filNavnLengde - 1] == L'G') && (filNavn[filNavnLengde - 2] == L'v' || filNavn[filNavnLengde - 2] == L'V') && (filNavn[filNavnLengde - 3] == L's' || filNavn[filNavnLengde - 3] == L'S') && filNavn[filNavnLengde - 4] == L'.') contentType = "image/svg+xml";
								else if (filNavnLengde > 5 && (filNavn[filNavnLengde - 1] == L'm' || filNavn[filNavnLengde - 1] == L'M') && (filNavn[filNavnLengde - 2] == L'b' || filNavn[filNavnLengde - 2] == L'B') && (filNavn[filNavnLengde - 3] == L'e' || filNavn[filNavnLengde - 3] == L'E') && (filNavn[filNavnLengde - 4] == L'w' || filNavn[filNavnLengde - 4] == L'W') && filNavn[filNavnLengde - 5] == L'.') contentType = "video/webm";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'm' || filNavn[filNavnLengde - 1] == L'M') && (filNavn[filNavnLengde - 2] == L't' || filNavn[filNavnLengde - 2] == L'T') && (filNavn[filNavnLengde - 3] == L'h' || filNavn[filNavnLengde - 3] == L'H') && filNavn[filNavnLengde - 4] == L'.') contentType = "text/html";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'l' || filNavn[filNavnLengde - 1] == L'L') && (filNavn[filNavnLengde - 2] == L'm' || filNavn[filNavnLengde - 2] == L'M') && (filNavn[filNavnLengde - 3] == L'x' || filNavn[filNavnLengde - 3] == L'X') && filNavn[filNavnLengde - 4] == L'.') contentType = "text/xml";					
								else if (filNavnLengde > 5 && (filNavn[filNavnLengde - 1] == L'n' || filNavn[filNavnLengde - 1] == L'N') && (filNavn[filNavnLengde - 2] == L'o' || filNavn[filNavnLengde - 2] == L'O') && (filNavn[filNavnLengde - 3] == L's' || filNavn[filNavnLengde - 3] == L'S') && (filNavn[filNavnLengde - 4] == L'j' || filNavn[filNavnLengde - 4] == L'J') && filNavn[filNavnLengde - 5] == L'.') contentType = "application/json";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L'p' || filNavn[filNavnLengde - 1] == L'P') && (filNavn[filNavnLengde - 2] == L'i' || filNavn[filNavnLengde - 2] == L'I') && (filNavn[filNavnLengde - 3] == L'z' || filNavn[filNavnLengde - 3] == L'Z') && filNavn[filNavnLengde - 4] == L'.') contentType = "application/zip";
								else if (filNavnLengde > 4 && (filNavn[filNavnLengde - 1] == L's' || filNavn[filNavnLengde - 1] == L'S') && (filNavn[filNavnLengde - 2] == L'p' || filNavn[filNavnLengde - 2] == L'P') && (filNavn[filNavnLengde - 3] == L'x' || filNavn[filNavnLengde - 3] == L'X') && filNavn[filNavnLengde - 4] == L'.') contentType = "application/vnd.ms-xpsdocument";	
								else contentType = "application/octet-stream";
							}
							else if (fil->ContentType->Length() < 99)
							{
								char buffer[100];
								Tekst::AnsiEncode(fil->ContentType->Data(), buffer);
								contentType = buffer;
							}
							else
							{
								char *dBuffer = new char[fil->ContentType->Length() + 1];
								Tekst::AnsiEncode(fil->ContentType->Data(), dBuffer);
								contentType = dBuffer;
								delete[] dBuffer;
							}

							if ((contentType[0] == 't' &&  contentType[1] == 'e' && contentType[2] == 'x' && contentType[3] == 't' && contentType[4] == '/') || contentType == "application/javascript" || contentType == "image/svg+xml" || contentType == "application/json") // Fil er tekst og må sikres at data sendes i utf-8 format
							{
								contentType += "; charset=utf-8";

								Windows::Storage::Streams::IBuffer ^buffer = ref new Windows::Storage::Streams::Buffer(static_cast<unsigned int>(fil->Size));
								concurrency::create_task(fil->ReadAsync(buffer, static_cast<unsigned int>(fil->Size), Windows::Storage::Streams::InputStreamOptions::ReadAhead)).wait(); // Leser fil inn i buffer

								// Query the IBufferByteAccess interface.
								Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
								reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

								// Retrieve the buffer data.
								byte *pBuffer = nullptr;
								bufferByteAccess->Buffer(&pBuffer);

								// Ordner tekst som skal sendes
								char* returTekst;
								bool  dynamiskTekst = false;
								size_t tekstLengde;

								if (pBuffer[0] == 239 && pBuffer[1] == 187 && pBuffer[2] == 191) // Er i utf-8 format, fjerner kun bom markering
								{
									returTekst = reinterpret_cast<char*>(pBuffer) + 3;
									tekstLengde = buffer->Length - 3;
								}
								else // Må konverteres til utf-8
								{
									tekstLengde = Tekst::Utf8EncodeLength(reinterpret_cast<char*>(pBuffer), buffer->Length);
									returTekst = new char[tekstLengde];
									dynamiskTekst = true;
									Tekst::Utf8Encode(reinterpret_cast<char*>(pBuffer), returTekst, buffer->Length);
								}

								if (harRanges && startRange < tekstLengde && startRange <= sluttRange) // Oppgitt område skal sendes
								{
									if (sluttRange >= tekstLengde) sluttRange = tekstLengde - 1;

									std::string returHead = "HTTP/1.1 206 Partial Content\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(sluttRange - startRange + 1) + "\r\nContent-Range: bytes " + std::to_string(startRange) + "-" + std::to_string(sluttRange) + "/" + std::to_string(tekstLengde) + "\r\nContent-Type: " + contentType + "\r\n" + httpResponse->cookieTekst + "\r\n";

									if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.length(), 0);
										antallSendt += send(socket, returTekst + startRange, sluttRange - startRange + 1, 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
									else
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
								}
								else
								{
									std::string returHead = "HTTP/1.1 200 OK\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(tekstLengde) + "\r\nContent-Type: " + contentType + "\r\n" + httpResponse->cookieTekst + "\r\n";

									if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.length(), 0);
										antallSendt += send(socket, returTekst, tekstLengde, 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
									else
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
								}

								if (dynamiskTekst) delete[] returTekst;
							}
							else // Fil er ikke tekst og kan sendes slik den er
							{
								if (harRanges && startRange < fil->Size && startRange <= sluttRange) // Oppgitt område skal sendes
								{
									if (sluttRange >= static_cast<size_t>(fil->Size)) sluttRange = static_cast<size_t>(fil->Size) - 1;

									std::string returHead = "HTTP/1.1 206 Partial Content\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(sluttRange - startRange + 1) + "\r\nContent-Range: bytes " + std::to_string(startRange) + "-" + std::to_string(sluttRange) + "/" + std::to_string(fil->Size) + "\r\nContent-Type: " + contentType + "\r\n" + httpResponse->cookieTekst + "\r\n";

									if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
									{
										if (sluttRange - startRange + 1 <= bufferSize)
										{
											Windows::Storage::Streams::IBuffer ^buffer = ref new Windows::Storage::Streams::Buffer(sluttRange - startRange + 1);
											if (startRange > 0) fil->Seek(startRange); // Søker til startpunkt hvis det ikke er ved start
											concurrency::create_task(fil->ReadAsync(buffer, sluttRange - startRange + 1, Windows::Storage::Streams::InputStreamOptions::Partial)).wait(); // Leser fil til buffer

											// Query the IBufferByteAccess interface.
											Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
											reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

											// Retrieve the buffer data.
											byte *pBuffer = nullptr;
											bufferByteAccess->Buffer(&pBuffer);

											int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
											antallSendt += send(socket, reinterpret_cast<char*>(pBuffer), sluttRange - startRange + 1, 0);
											if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
											else keepAlive = false;
										}
										else
										{
											Windows::Storage::Streams::IBuffer ^buffer = ref new Windows::Storage::Streams::Buffer(bufferSize);

											// Query the IBufferByteAccess interface.
											Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
											reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

											// Retrieve the buffer data.
											byte *pBuffer = nullptr;
											bufferByteAccess->Buffer(&pBuffer);

											size_t antallIgjen = sluttRange - startRange + 1;
											if (startRange > 0) fil->Seek(startRange); // Søker til startpunkt hvis det ikke er ved start

											int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0); // Sender header

											if (antallSendt > 0)
											{
												antallBytesSendt += static_cast<unsigned long long>(antallSendt);

												do
												{
													if (antallIgjen > bufferSize)
													{
														concurrency::create_task(fil->ReadAsync(buffer, bufferSize, Windows::Storage::Streams::InputStreamOptions::Partial)).wait();
														antallIgjen -= bufferSize;
													}
													else
													{
														concurrency::create_task(fil->ReadAsync(buffer, antallIgjen, Windows::Storage::Streams::InputStreamOptions::Partial)).wait();
														antallIgjen = 0;
													}

													antallSendt = send(socket, reinterpret_cast<char*>(pBuffer), buffer->Length, 0);
													if (antallSendt < static_cast<int>(buffer->Length))
													{
														keepAlive = false;
														if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
														break;
													}
													else antallBytesSendt += static_cast<unsigned long long>(antallSendt);
												} while (antallIgjen > 0);
											}
											else keepAlive = false;
										}
									}
									else
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
								}
								else
								{
									std::string returHead = "HTTP/1.1 200 OK\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=" + std::to_string(httpResponse->maxAge) + "\r\nContent-Length: " + std::to_string(fil->Size) + "\r\nContent-Type: " + contentType + "\r\n" + httpResponse->cookieTekst + "\r\n";

									if (dataHenter.Method < 4) // Er GET,POST,PUT eller DELETE og content skal sendes
									{
										if (fil->Size <= bufferSize)
										{
											Windows::Storage::Streams::IBuffer ^buffer = ref new Windows::Storage::Streams::Buffer(static_cast<unsigned int>(fil->Size));
											concurrency::create_task(fil->ReadAsync(buffer, static_cast<unsigned int>(fil->Size), Windows::Storage::Streams::InputStreamOptions::ReadAhead)).wait();

											// Query the IBufferByteAccess interface.
											Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
											reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

											// Retrieve the buffer data.
											byte *pBuffer = nullptr;
											bufferByteAccess->Buffer(&pBuffer);

											int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
											antallSendt += send(socket, reinterpret_cast<char*>(pBuffer), buffer->Length, 0);
											if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
											else keepAlive = false;
										}
										else
										{
											Windows::Storage::Streams::IBuffer ^buffer = ref new Windows::Storage::Streams::Buffer(bufferSize);

											// Query the IBufferByteAccess interface.
											Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
											reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

											// Retrieve the buffer data.
											byte *pBuffer = nullptr;
											bufferByteAccess->Buffer(&pBuffer);

											size_t antallIgjen = static_cast<size_t>(fil->Size);

											int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0); // Sender header

											if (antallSendt > 0)
											{
												antallBytesSendt += static_cast<unsigned long long>(antallSendt);

												do
												{
													if (antallIgjen > bufferSize)
													{
														concurrency::create_task(fil->ReadAsync(buffer, bufferSize, Windows::Storage::Streams::InputStreamOptions::Partial)).wait();
														antallIgjen -= bufferSize;
													}
													else
													{
														concurrency::create_task(fil->ReadAsync(buffer, antallIgjen, Windows::Storage::Streams::InputStreamOptions::Partial)).wait();
														antallIgjen = 0;
													}

													antallSendt = send(socket, reinterpret_cast<char*>(pBuffer), buffer->Length, 0);
													if (antallSendt < static_cast<int>(buffer->Length))
													{
														keepAlive = false;
														if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
														break;
													}
													else antallBytesSendt += static_cast<unsigned long long>(antallSendt);
												} while (antallIgjen > 0);
											}
											else keepAlive = false;
										}
									}
									else
									{
										int antallSendt = send(socket, returHead.c_str(), returHead.size(), 0);
										if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
										else keepAlive = false;
									}
								}
							}
						}
						catch (...)
						{
							keepAlive = false;
							std::string returHtml = "<html>\r\n<head>\r\n <title>500 Internal Server Error</title>\r\n</head>\r\n<body>\r\n <h1>500 Internal Server Error</h1>\r\n <p>Error reading file data.</p>\r\n</body>\r\n</html>";
							std::string returMelding = "HTTP/1.1 500 Internal Server Error\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\nConnection: close\r\n\r\n";
							if (dataHenter.Method != 4) returMelding += returHtml;

							int antallSendt = send(socket, returMelding.c_str(), returMelding.size(), 0);
							if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
						}
					}
				}
				else // SetNotFound, SetRedirect eller SetUnauthorized er satt
				{
					int antallSendt = send(socket, httpResponse->notDataTekst.c_str(), httpResponse->notDataTekst.size(), 0);
					if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
				}
			}
			else // Event kommet i retur uten at noe er satt og er intern server feil
			{
				keepAlive = false;
				std::string returHtml = "<html>\r\n<head>\r\n <title>500 Internal Server Error</title>\r\n</head>\r\n<body>\r\n <h1>500 Internal Server Error</h1>\r\n <p>No data was set from server.</p>\r\n</body>\r\n</html>";
				std::string returMelding = "HTTP/1.1 500 Internal Server Error\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\nConnection: close\r\n\r\n";
				if (dataHenter.Method != 4) returMelding += returHtml;

				int antallSendt = send(socket, returMelding.c_str(), returMelding.size(), 0);
				if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
			}
		}
		else if (dataHenter.HenteStatus == Status_Bad_ToBig) // Content for stort til å bli behandlet. Skal bare sende at dette er forbudt og kobling lukkes.
		{
			std::string returHtml = "<html>\r\n<head>\r\n <title>403 Forbidden</title>\r\n</head>\r\n<body>\r\n <h1>400 Bad Request</h1>\r\n <p>" + dataHenter.FeilMelding + "</p>\r\n</body>\r\n</html>";
			std::string returMelding = "HTTP/1.1 403 Forbidden\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\nConnection: close\r\n\r\n" + returHtml;
			int antallSendt = send(socket, returMelding.c_str(), returMelding.size(), 0);
			if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
			antallBadRequest++;
		}
		else if (dataHenter.HenteStatus == Status_Bad_LengthMissing) // Content lengde er påkrevd i denne server. Sender feilmelding om dette. Chuncked er er ikke støttet hvis det er det som sendes. Kobling skal lukkes.
		{
			std::string returHtml = "<html>\r\n<head>\r\n <title>411 Length Required</title>\r\n</head>\r\n<body>\r\n <h1>411 Length Required</h1>\r\n <p>Content length required for this server.</p>\r\n</body>\r\n</html>";
			std::string returMelding = "HTTP/1.1 411 Length Required\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\nConnection: close\r\n\r\n" + returHtml;
			int antallSendt = send(socket, returMelding.c_str(), returMelding.size(), 0);
			if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
			antallBadRequest++;
		}
		else if (dataHenter.HenteStatus == Status_Bad) // Noe har gått galt, feilmelding skal sendes
		{
			std::string returHtml = "<html>\r\n<head>\r\n <title>400 Bad Request</title>\r\n</head>\r\n<body>\r\n <h1>400 Bad Request</h1>\r\n <p>" + dataHenter.FeilMelding + "</p>\r\n</body>\r\n</html>";
			std::string returMelding = "HTTP/1.1 400 Bad Request\r\nDate: " + Tid::GetHttpDateTime() + "\r\nServer: GMJ/2.0\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: " + std::to_string(returHtml.size()) + "\r\nConnection: close\r\n\r\n" + returHtml;
			int antallSendt = send(socket, returMelding.c_str(), returMelding.size(), 0);
			if (antallSendt > 0) antallBytesSendt += static_cast<unsigned long long>(antallSendt);
			antallBadRequest++;
		}
	} while (keepAlive);

	closesocket(socket); // Lukker oppkobling hvis den fremdeles er åpen
}

std::string WebServer::FaaContentTypeBeskrivelse(FileTypes fileType)
{
	const char* const filTyper[] =
	{
		"text/html; charset=utf-8",
		"text/css; charset=utf-8",
		"text/xml; charset=utf-8",
		"text/plain; charset=utf-8",
		"application/javascript; charset=utf-8",
		"application/json; charset=utf-8",
		"application/soap+xml; charset=utf-8",
		"image/svg+xml; charset=utf-8",
		"image/jpeg",
		"image/png",
		"image/gif",
		"image/x-icon",
		"video/mp4",
		"video/webm",
		"audio/mpeg",
		"audio/wav",
		"application/pdf",
		"application/zip",
		"application/vnd.ms-xpsdocument",
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document",
		"application/octet-stream"
	};

	return filTyper[static_cast<int>(fileType)];
}