#include "pch.h"
#include "DataHenter.h"
#include "WebServer.h"

using namespace HttpWebServer::HelpClasses;

DataHenter::DataHenter(const SOCKET socket, const size_t maxContentSize) // maxContentSize i byte
{
	HenteStatus = HentData(socket, maxContentSize); // Henter og formaterer data

	if (HenteStatus == Status_OK)
	{
		if (Cookie != nullptr) FormaterCookie(); // Har cookie(er) of disse splittes i denne funksjonen
		if (QueryString != nullptr) FormaterQuerystring(); // Har querystring i header og denne splittes i denne funksjonen
		if (ContentLength > 0 && ContentType != nullptr) // Har content
		{
			// Sjekker om content er application/x-www-form-urlencoded                     
			if (ContentType[0] == 'a' && ContentType[1] == 'p' && ContentType[2] == 'p' && ContentType[3] == 'l' && ContentType[4] == 'i' && ContentType[5] == 'c' && ContentType[6] == 'a' && ContentType[7] == 't' && ContentType[8] == 'i' && ContentType[9] == 'o' && ContentType[10] == 'n' && ContentType[11] == '/' && ContentType[12] == 'x' && ContentType[13] == '-' && ContentType[14] == 'w' && ContentType[15] == 'w' && ContentType[16] == 'w' && ContentType[17] == '-' && ContentType[18] == 'f' && ContentType[19] == 'o' && ContentType[20] == 'r' && ContentType[21] == 'm' && ContentType[22] == '-' && ContentType[23] == 'u' && ContentType[24] == 'r' && ContentType[25] == 'l' && ContentType[26] == 'e' && ContentType[27] == 'n' && ContentType[28] == 'c' && ContentType[29] == 'o' && ContentType[30] == 'd' && ContentType[31] == 'e' && ContentType[32] == 'd')
			{
				if (ContentLength > 0) FormaterContentQuerystring();
			}
			// Sjekker om content er multipart/form-data
			else if (ContentType[0] == 'm' && ContentType[1] == 'u' && ContentType[2] == 'l' && ContentType[3] == 't' && ContentType[4] == 'i' && ContentType[5] == 'p' && ContentType[6] == 'a' && ContentType[7] == 'r' && ContentType[8] == 't' && ContentType[9] == '/' && ContentType[10] == 'f' && ContentType[11] == 'o' && ContentType[12] == 'r' && ContentType[13] == 'm' && ContentType[14] == '-' && ContentType[15] == 'd' && ContentType[16] == 'a' && ContentType[17] == 't' && ContentType[18] == 'a')
			{
				if (ContentLength > 0) FormaterMultiPartData();
			}
			else // Content brukes som 1 fil vedlegg og content settes til oppgitt content
			{
				MultiPartDataer.push_back(MultiPartData(Content, ContentType, ContentLength));
			}
		}
	}
	else KeepAlive = false; // Kobling skal lukkes hvis noe gikk galt
}

DataHenter::~DataHenter()
{
	if (DynamiskContent) delete[] Content;
	if (FormDataer != nullptr) delete[] FormDataer;
	if (Cookier != nullptr) delete[] Cookier;
}

bool DataHenter::GetRanges(size_t & startRange, size_t & sluttRange)
{
	if (Range == nullptr) return false;

	char *Start = Range;
	char *Slutt = nullptr;

	for (size_t i = 0; ; i++)
	{
		if (Range[i] == '-')
		{
			Range[i] = '\0';
			Slutt = Range + i + 1;
		}
		else if (Range[i] == '\0') break;
	}

	if (Slutt == nullptr) return false;

	for (size_t i = 0; ; i++)
	{
		if (Start[i] > 47 && Start[i] < 58) // Er tall
		{
			if (startRange > 0) startRange *= 10; // Ganger ved 10 hvis nytt tall
			startRange += Start[i] - 48; // Legger til ny tallverdi
		}
		else if (Start[i] == '\0') break;
	}

	bool sluttSatt = false;
	for (size_t i = 0; ; i++)
	{
		if (Slutt[i] > 47 && Slutt[i] < 58) // Er tall
		{
			sluttSatt = true;
			if (sluttRange > 0) sluttRange *= 10; // Ganger ved 10 hvis nytt tall
			sluttRange += Slutt[i] - 48; // Legger til ny tallverdi
		}
		else if (Slutt[i] == '\0') break;
		else if (Slutt[i] == '-') return false;
	}

	if (!sluttSatt) sluttRange = ~sluttRange;

	return true;
}

int DataHenter::HentData(const SOCKET socket, const size_t maxContentSize)
{
	size_t dataMottattStørrelse = 0;

	do // Kjører helt til funksjon returnerer
	{
		int antallMotatt = recv(socket, Header + dataMottattStørrelse, sizeof(Header) - dataMottattStørrelse, 0); // Henter data fra socket

		if (antallMotatt > 0) // Har mottat data
		{
			BytesMotatt += antallMotatt; // BytesMotatt brukes for å telle totalt hvor mange bytes som er mottatt
			dataMottattStørrelse += antallMotatt;

			if (dataMottattStørrelse > 4) // Ingen vits å skjekke vis mindre enn 4 byte er motatt.
			{
				for (size_t i = 0; i < dataMottattStørrelse - 3; i++) // Går gjennom data og sjekker.
				{
					if (Header[i] == '\r' && Header[i + 1] == '\n' && Header[i + 2] == '\r' && Header[i + 3] == '\n') // Leter etter 2 linjeskift
					{
						HeaderLength = i;

						size_t contentStart = i + 4;

						int FormatHeaderStatus = FormatHeader(FeilMelding); // // Alt i header er mottat og header kan formateres.
						if (FormatHeaderStatus != Status_OK) return FormatHeaderStatus; // Formatering av header feilet, bad request skal eventuelt sendes og oppkobling skal lukkes uansett

						if (ContentLength > 0)
						{
							if (ContentLength + 1 <= sizeof(Header) - contentStart) // Content får plass i header buffer eller er allerede i header buffer. Skal ha en ledig plass på slutt for \0 terminering hvis det er tekst i content som skal formateres uten kopiering av data
							{
								while (dataMottattStørrelse < contentStart + ContentLength) // Henter resten av content
								{
									antallMotatt = recv(socket, Header + dataMottattStørrelse, (contentStart + ContentLength) - dataMottattStørrelse, 0);

									if (antallMotatt > 0)
									{
										BytesMotatt += antallMotatt; // BytesMotatt brukes for å telle totalt hvor mange bytes som er mottatt
										dataMottattStørrelse += antallMotatt;
									}
									else return Status_Bad_Closed; // Ingenting motatt, kobling lukket eller timeout. Kobling skal lukkes uten noe mer handling
								}

								Content = Header + contentStart;
								return Status_OK; // Data  med content motatt ok
							}
							else if (ContentLength > maxContentSize) // Sjekker om content størrelse er for stor
							{
								if (maxContentSize > 0) FeilMelding = "Content too big. Maximum size is " + std::to_string(maxContentSize) + " bytes.";
								else FeilMelding = "Content too big.";

								return Status_Bad_ToBig; // Content er for stort. Bad request skal sendes og kobling skal lukkes
							}
							else // Content får ikke plass i header buffer, må plasseres dynamisk
							{
								Content = new char[ContentLength + 1]; // Skal ha en ledig plass på slutt for \0 terminering hvis det er tekst i content som skal formateres uten kopiering av data
								DynamiskContent = true;

								// Kopierer data fra Header buffer til ny dynamisk Content
								memcpy(Content, Header + contentStart, dataMottattStørrelse - contentStart);
								dataMottattStørrelse = dataMottattStørrelse - contentStart;

								while (dataMottattStørrelse < ContentLength) // Henter resten av content
								{
									antallMotatt = recv(socket, Content + dataMottattStørrelse, ContentLength - dataMottattStørrelse, 0);

									if (antallMotatt > 0)
									{
										BytesMotatt += antallMotatt; // BytesMotatt brukes for å telle totalt hvor mange bytes som er mottatt
										dataMottattStørrelse += antallMotatt;
									}
									else return Status_Bad_Closed; // Ingenting motatt, kobling lukket eller timeout. Kobling skal lukkes uten noe mer handling
								}
								return Status_OK; // Data med content motatt ok
							}
						}
						return Status_OK; // Data uten content motatt ok
					}
				}
			}
		}
		else return Status_Bad_Closed; // Ingenting motatt, kobling lukket eller timeout. Kobling skal lukkes uten noe mer handling
	} while (dataMottattStørrelse < sizeof(Header)); // Henter mer data hvis hode ikke er ferdig og header buffer ikke er fullt

	FeilMelding = "Reguest header too big.";
	return Status_Bad; // Header buffer fullt uten at det er funnet 2 linjeskift. Bad request skal sendes og kobling skal lukkes
}

int DataHenter::FormatHeader(std::string &feilMelding)
{
	char *pHeader; // Midlertidig pointer for å indikere plassering

	if (HeaderLength > 13)
	{
		pHeader = Header;

		if (pHeader[0] == 'G' && pHeader[1] == 'E' && pHeader[2] == 'T' && pHeader[3] == ' ') Method = 0; // GET
		else if (pHeader[0] == 'P' && pHeader[1] == 'O' && pHeader[2] == 'S' && pHeader[3] == 'T' && pHeader[4] == ' ') Method = 1; // POST;
		else if (pHeader[0] == 'P' && pHeader[1] == 'U' && pHeader[2] == 'T' && pHeader[3] == ' ') Method = 2; // PUT;
		else if (pHeader[0] == 'D' && pHeader[1] == 'E' && pHeader[2] == 'L' && pHeader[3] == 'E' && pHeader[4] == 'T' && pHeader[5] == 'E' && pHeader[6] == ' ') Method = 3; // DELETE;
		else if (pHeader[0] == 'H' && pHeader[1] == 'E' && pHeader[2] == 'A' && pHeader[3] == 'D' && pHeader[4] == ' ') Method = 4; // HEAD;
		else
		{
			feilMelding = "Unknown method.";
			return Status_Bad;
		}
	}
	else
	{
		feilMelding = "Bad request line.";
		return Status_Bad;
	}

	// Finner URI start
	if (Method == 0 || Method == 2) URI = pHeader + 4; // GET eller PUT
	else if (Method == 1 || Method == 4) URI = pHeader + 5; // POST eller HEAD
	else URI = pHeader + 7; // DELETE

	// Sjekker at URI begynner med /, ellers er det feil
	if (URI[0] != '/')
	{
		feilMelding = "URI must start with /";
		return Status_Bad;
	}

	// Finner og setter inn \0 terminering på URI og finner muligens Querystring og setter \0 terminering på denne også
	for (size_t i = 1; ; i++)
	{
		if (URI[i] == ' ')
		{
			URI[i] = '\0';
			pHeader = URI + i + 1;
			break;
		}
		else if (URI[i] == '?' && QueryString == nullptr)
		{
			URI[i] = '\0';
			QueryString = URI + i + 1;
		}
		else if (URI[i] == '\r')
		{
			feilMelding = "Http version is missing.";
			return Status_Bad; // Linjeskift uten http versjon i request linje, bad request skal sendes og kobling skal lukkes
		}
	}

	// Sjekker Http versjon, kun HTTP/1.0 og HTTP/1.1 godtas
	if (pHeader[0] == 'H' && pHeader[1] == 'T' && pHeader[2] == 'T' && pHeader[3] == 'P' && pHeader[4] == '/' && pHeader[5] == '1' && pHeader[6] == '.' && (pHeader[7] == '1' || pHeader[7] == '0') && pHeader[8] == '\r' && pHeader[9] == '\n')
	{
		pHeader += 10;
	}
	else
	{
		feilMelding = "Http version not supported.";
		return Status_Bad; // Feil http versjon, bad request skal sendes og kobling skal lukkes
	}

	char *pHeaderSlutt = Header + HeaderLength; // Midlertidig pointer for å indikere slutt på header

	while (pHeader < pHeaderSlutt) // Har ny linje
	{
		if (pHeader[0] == 'A' || pHeader[0] == 'a')
		{
			if (pHeader + 8 < pHeaderSlutt && pHeader[1] == 'c' && pHeader[2] == 'c' && pHeader[3] == 'e' && pHeader[4] == 'p' && pHeader[5] == 't')
			{
				if (pHeader[6] == ':' && pHeader[7] == ' ')
				{
					pHeader += 8;
					Accept = pHeader;
				}
				else if (pHeader + 17 < pHeaderSlutt && pHeader[6] == '-' && pHeader[7] == 'L' && pHeader[8] == 'a' && pHeader[9] == 'n' && pHeader[10] == 'g' && pHeader[11] == 'u' && pHeader[12] == 'a' && pHeader[13] == 'g' && pHeader[14] == 'e' && pHeader[15] == ':' && pHeader[16] == ' ')
				{
					pHeader += 17;
					AcceptLanguage = pHeader;
				}
			}
			else if (pHeader + 21 < pHeaderSlutt && pHeader[1] == 'u' && pHeader[2] == 't' && pHeader[3] == 'h' && pHeader[4] == 'o' && pHeader[5] == 'r' && pHeader[6] == 'i' && pHeader[7] == 'z' && pHeader[8] == 'a' && pHeader[9] == 't' && pHeader[10] == 'i' && pHeader[11] == 'o' && pHeader[12] == 'n' && pHeader[13] == ':' && pHeader[14] == ' ' && (pHeader[15] == 'B' || pHeader[15] == 'b') && pHeader[16] == 'a' && pHeader[17] == 's' && pHeader[18] == 'i' && pHeader[19] == 'c' && pHeader[20] == ' ')
			{
				pHeader += 21;
				Authorization = pHeader;
			}
		}
		else if ((pHeader[0] == 'C' || pHeader[0] == 'c') && pHeader + 8 < pHeaderSlutt && pHeader[1] == 'o')
		{
			if (pHeader[2] == 'n')
			{
				if (pHeader[3] == 't' && pHeader[4] == 'e' && pHeader[5] == 'n' && pHeader[6] == 't' && pHeader[7] == '-')
				{
					if (pHeader + 16 <= pHeaderSlutt && (pHeader[8] == 'L' || pHeader[8] == 'l') && pHeader[9] == 'e' && pHeader[10] == 'n' && pHeader[11] == 'g' && pHeader[12] == 't' && pHeader[13] == 'h' && pHeader[14] == ':' && pHeader[15] == ' ')
					{
						pHeader += 16;

						for (size_t i = 0; ; i++)
						{
							if (pHeader[i] > 47 && pHeader[i] < 58) // Er tall
							{
								ContentLengthSatt = true;
								if (ContentLength > 0) ContentLength *= 10; // Ganger ved 10 hvis nytt tall
								ContentLength += pHeader[i] - 48; // Legger til ny tallverdi
							}
							else if (pHeader[i] == '\r') break;
							else if (pHeader[i] == ' ') continue;
							else // Ikke et tall, mellomrom eller ny linje, noe er feil
							{
								ContentLengthSatt = false; // Settes til false slik at feilmelding skal gis når for løkke avsluttes
								break;
							}
						}

						if (!ContentLengthSatt)
						{
							feilMelding = "Error reading Content-Length.";
							return Status_Bad; // Feil ved lesing av content length, bad request sendes og kobling skal lukkes
						}
					}
					else if (pHeader + 14 < pHeaderSlutt && (pHeader[8] == 'T' || pHeader[8] == 't') && pHeader[9] == 'y' && pHeader[10] == 'p' && pHeader[11] == 'e' && pHeader[12] == ':' && pHeader[13] == ' ')
					{
						pHeader += 14;
						ContentType = pHeader;
					}
				}
				else if (pHeader + 12 < pHeaderSlutt && pHeader[3] == 'n' && pHeader[4] == 'e' && pHeader[5] == 'c' && pHeader[6] == 't' && pHeader[7] == 'i' && pHeader[8] == 'o' && pHeader[9] == 'n' && pHeader[10] == ':' && pHeader[11] == ' ')
				{
					pHeader += 12;
					if (pHeader + 10 <= pHeaderSlutt && (pHeader[0] == 'k' || pHeader[0] == 'K') && pHeader[1] == 'e' && pHeader[2] == 'e' && pHeader[3] == 'p' && pHeader[4] == '-' && (pHeader[5] == 'a' || pHeader[5] == 'A') && pHeader[6] == 'l' && pHeader[7] == 'i' && pHeader[8] == 'v' && pHeader[9] == 'e') KeepAlive = true;
				}
			}
			else if (pHeader[2] == 'o' && pHeader[3] == 'k' && pHeader[4] == 'i' && pHeader[5] == 'e' && pHeader[6] == ':' && pHeader[7] == ' ')
			{
				pHeader += 8;
				Cookie = pHeader;
			}
		}
		else if (pHeader[0] == 'H' && pHeader + 6 < pHeaderSlutt && pHeader[1] == 'o' && pHeader[2] == 's' && pHeader[3] == 't' && pHeader[4] == ':' && pHeader[5] == ' ')
		{
			pHeader += 6;
			Host = pHeader;
		}
		else if ((pHeader[0] == 'R' || pHeader[0] == 'r') && pHeader + 13 < pHeaderSlutt && pHeader[1] == 'a' && pHeader[2] == 'n' && pHeader[3] == 'g' && pHeader[4] == 'e' && pHeader[5] == ':' && pHeader[6] == ' ' && (pHeader[7] == 'b' || pHeader[7] == 'B') && pHeader[8] == 'y' && pHeader[9] == 't' && pHeader[10] == 'e' && pHeader[11] == 's' && pHeader[12] == '=')
		{
			pHeader += 13;
			Range = pHeader;
		}
		else if ((pHeader[0] == 'U' || pHeader[0] == 'u') && pHeader + 12 < pHeaderSlutt && pHeader[1] == 's' && pHeader[2] == 'e' && pHeader[3] == 'r' && pHeader[4] == '-' && pHeader[5] == 'A' && pHeader[6] == 'g' && pHeader[7] == 'e' && pHeader[8] == 'n' && pHeader[9] == 't' && pHeader[10] == ':' && pHeader[11] == ' ')
		{
			pHeader += 12;
			UserAgent = pHeader;
		}

		// Finner linjeslutt, setter inn null terminering på slutt av linje og hopper til neste linje
		for (size_t i = 0; ; i++)
		{
			if (pHeader[i] == '\r')
			{
				pHeader[i] = '\0';
				pHeader += i + 2;
				break;
			}
		}
	}

	if (ContentType != nullptr && !ContentLengthSatt)
	{
		feilMelding = "Content-Length Required.";
		return Status_Bad_LengthMissing; // Har content type, men ingen content lengde
	}
	else if (ContentLength > 0 && ContentType == nullptr)
	{
		feilMelding = "Content-Type Required.";
		return Status_Bad; // Har content lengde, men ingen content type
	}

	return Status_OK;
}

void DataHenter::FormaterCookie()
{
	CookierSize++; // Det er i hvertfall 1 formdata.

	// Teller hvor mange flere cookidataer det er i cookiestring
	for (size_t i = 0; ; i++)
	{
		if (Cookie[i] == ';') CookierSize++;
		else if (Cookie[i] == '\0') break;
	}

	// Lager en dynamisk lagringsplass for alle formdata
	Cookier = new char*[CookierSize];

	// Setter første formdata
	Cookier[0] = Cookie;

	// Hvis flere enn 1 cookie settes disse
	if (CookierSize > 1)
	{
		for (size_t i = 0, nestePlass = 1; ; i++)
		{
			if (Cookie[i] == ';')
			{
				Cookie[i] = '\0';
				if (Cookie[i + 1] == ' ') Cookier[nestePlass++] = &Cookie[i + 2];
				else Cookier[nestePlass++] = &Cookie[i + 1];
			}
			else if (Cookie[i] == '\0') break;
		}
	}
}

void DataHenter::FormaterQuerystring()
{
	FormDataerSize++; // Det er i hvertfall 1 formdata.

	// Teller hvor mange flere formdataer det er i querystring
	for (size_t i = 0; ; i++)
	{
		if (QueryString[i] == '&') FormDataerSize++;
		else if (QueryString[i] == '\0') break;
	}

	// Lager en dynamisk lagringsplass for alle formdata
	FormDataer = new char*[FormDataerSize];

	// Setter første formdata
	FormDataer[0] = QueryString;

	// Hvis flere enn 1 formdata settes disse
	if (FormDataerSize > 1)
	{
		for (size_t i = 0, nestePlass = 1; ; i++)
		{
			if (QueryString[i] == '&')
			{
				QueryString[i] = '\0';
				FormDataer[nestePlass++] = &QueryString[i + 1];
			}
			else if (QueryString[i] == '\0') break;
		}
	}
}

void DataHenter::FormaterContentQuerystring()
{
	Content[ContentLength] = '\0'; // Setter inn \0 terminering på slutten i ledig plass etter content

	if (FormDataerSize == 0) // Ingen data fra før, data kan bare legges til
	{
		FormDataerSize++; // Det er i hvertfall 1 formdata.

		// Teller hvor mange flere formdataer det er i querystring
		for (size_t i = 0; ; i++)
		{
			if (Content[i] == '&') FormDataerSize++;
			else if (Content[i] == '\0') break;
		}

		// Lager en dynamisk lagringsplass for alle formdata
		FormDataer = new char*[FormDataerSize];

		// Setter første formdata
		FormDataer[0] = Content;

		// Hvis flere enn 1 formdata settes disse
		if (FormDataerSize > 1)
		{
			for (size_t i = 0, nestePlass = 1; ; i++)
			{
				if (Content[i] == '&')
				{
					Content[i] = '\0';
					FormDataer[nestePlass++] = &Content[i + 1];
				}
				else if (Content[i] == '\0') break;
			}
		}
	}
	else // Det er data fra før, disse må legges i samme array container
	{
		size_t antallNye = 1;  // Det er i hvertfall 1 formdata.

		// Teller hvor mange flere formdataer det er i querystring
		for (size_t i = 0; ; i++)
		{
			if (Content[i] == '&') antallNye++;
			else if (Content[i] == '\0') break;
		}

		char **gamleFormData = FormDataer; // Midlertidig lagringsplass for formdata som allerede er lagt til

		FormDataer = new char*[FormDataerSize + antallNye];

		for (size_t i = 0; i < FormDataerSize; i++)
		{
			FormDataer[i] = gamleFormData[i];
		}

		delete[] gamleFormData; // Sletter gammel lager plass for formdata

		// Setter første nye formdata
		FormDataer[FormDataerSize] = Content;

		// Hvis flere enn 1 formdata settes disse
		if (antallNye > 1)
		{
			for (size_t i = 0, nestePlass = FormDataerSize + 1; ; i++)
			{
				if (Content[i] == '&')
				{
					Content[i] = '\0';
					FormDataer[nestePlass++] = &Content[i + 1];
				}
				else if (Content[i] == '\0') break;
			}
		}

		FormDataerSize += antallNye;
	}
}

void DataHenter::FormaterMultiPartData()
{
	char *boundary = nullptr; // Pointer til boundary
	size_t boundaryLength; // Lengde på boundary

	// Finner "boundary="
	for (size_t i = 19; ; i++)
	{
		if (ContentType[i] == '\0') break;
		if (ContentType[i] == 'b' && ContentType[i + 1] == 'o' && ContentType[i + 2] == 'u' && ContentType[i + 3] == 'n' && ContentType[i + 4] == 'd' && ContentType[i + 5] == 'a' && ContentType[i + 6] == 'r' && ContentType[i + 7] == 'y' && ContentType[i + 8] == '=')
		{
			boundary = &ContentType[i + 9];
			break;
		}
	}
	if (boundary == nullptr) return;
	boundary -= 2;
	boundary[0] = '-';
	boundary[1] = '-'; // Søke boundary har 2 ekstra - i begynnelsen, disse legges til slik at søk blir enklere
	boundaryLength = strlen(boundary);

	char *ContentSlutt = Content + ContentLength; // Pointer til når content er slutt

	size_t delContentLengde = 0; // Midlertidig lengde variabel;
	char* pSeek = FinnNesteMultiPartData(Content, boundary, boundaryLength, ContentSlutt, delContentLengde);

	while (pSeek != nullptr)
	{
		MultiPartDataer.push_back(MultiPartData(pSeek, delContentLengde));

		pSeek = FinnNesteMultiPartData(pSeek + delContentLengde, boundary, boundaryLength, ContentSlutt, delContentLengde);
	}
}

char* DataHenter::FinnNesteMultiPartData(char *start, const char *boundary, const size_t boundaryLength, const char *contentSlutt, size_t &lengde)
{
	size_t maksSøkelengde = contentSlutt - start - boundaryLength - 2; // Finner ut hvor langt ut søk kan gå med henhold til hvor lang content lengde er og hvor lang boundary er
	lengde = 0;

	for (size_t i = 0; i < maksSøkelengde; i++)
	{
		bool lik = true;

		for (size_t i2 = 0; i2 < boundaryLength; i2++)
		{
			if (start[i + i2] != boundary[i2])
			{
				lik = false;
				break;
			}
			else lik = true;
		}

		if (lik && start[i + boundaryLength] == '\r' && start[i + boundaryLength + 1] == '\n')
		{
			char *ElementStart = start + i + boundaryLength + 2;

			for (size_t j = 0; j < maksSøkelengde; j++)
			{
				for (size_t j2 = 0; j2 < boundaryLength; j2++)
				{
					if (ElementStart[j + j2] != boundary[j2])
					{
						lik = false;
						break;
					}
					else lik = true;
				}

				if (lik)
				{
					lengde = j - 2; // Tar ikke med \r\n på slutt
					break;
				}
			}

			return ElementStart;
		}
		else if (lik)
		{
			lengde = 0;
			return nullptr;
		}
	}
	lengde = 0;
	return nullptr;
}