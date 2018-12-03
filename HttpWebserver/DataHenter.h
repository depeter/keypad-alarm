#pragma once

#include <ws2tcpip.h>
#include <string>
#include <vector>
#include "MultiPartData.h"

namespace HttpWebServer
{
	namespace HelpClasses
	{
		class DataHenter
		{
		public:
			DataHenter(const SOCKET socket, const size_t maxContentSize);
			~DataHenter();

			bool GetRanges(size_t &startRange, size_t &sluttRange); // Returnerer false hvis ranges ikke er satt ellers returnerer true

			int Method; // 0 = GET, 1 = POST, 2 = PUT, 3 = DELETE og 4 = HEAD
			char *URI;  // URI uten querystring
			char *Accept = nullptr; // "Accept: "
			char *AcceptLanguage = nullptr; // "Accept-Language: "
			char *Host = nullptr; // "Host: "
			char *UserAgent = nullptr; //"User-Agent: "
			char *Authorization = nullptr; // "Authorization: Basic "

			char **Cookier = nullptr; // For lagring av cookies
			size_t CookierSize = 0; // Antall som er lagret i Cookier

			char **FormDataer = nullptr; // For lagring av URL kodet formdata, hver data må splittes på '=' og tekst må URL dekodes
			size_t FormDataerSize = 0; // Antall data som er lagret i FormDataer

			std::vector<MultiPartData> MultiPartDataer;

			bool KeepAlive = false; // "Connection: " true hvis keep-alive ellers false
			int HenteStatus; // Status for henting, verdier er definert i WebServer.h
			std::string FeilMelding; // Retur melding som skal sendes ved feil. Har kun innhold hvis HenteStatus er R_BAD
			int BytesMotatt = 0; // BytesMotatt brukes for å telle totalt hvor mange bytes totalt som er mottatt

		private:
			char Header[8192]; // Buffer for lagring av header og content. Hvis det ikke er plass til content i denne buffer opprettes det dynamisk minne til content, ellers settes content pointer til en plass i denne buffer
			size_t HeaderLength = 0;
			char *QueryString = nullptr;
			char *Range = nullptr; // "Range: bytes="
			char *ContentType = nullptr; // "Content-Type: "
			bool ContentLengthSatt = false; // Er true hvis content lengde er satt.
			size_t ContentLength = 0; // "Content-Length: "
			char *Content = nullptr;
			char *Cookie = nullptr; // "Cookie: "

			int HentData(const SOCKET socket, const size_t maxContentSize); // maxContentSize i byte 
			int FormatHeader(std::string &feilMelding);
			void FormaterCookie();
			void FormaterQuerystring();
			void FormaterContentQuerystring();
			void FormaterMultiPartData();
			char* FinnNesteMultiPartData(char *start, const char *boundary, const size_t boundaryLength, const char * contentSlutt, size_t &lengde);

			bool DynamiskContent = false;  // Brukes av destructor for å vite at content er allokert dynamisk og må slettes når klasse ødelegges
		};
	}
}