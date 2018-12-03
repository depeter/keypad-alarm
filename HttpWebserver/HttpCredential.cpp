#include "pch.h"
#include "HttpCredential.h"

using namespace HttpWebServer::DetailClasses;

HttpCredential::HttpCredential(char *credentialText)
{
	int innTekstLengde = static_cast<int>(strlen(credentialText));

	if (innTekstLengde < 300)
	{
		char buffer[300];
		int dekodetLengde = Base64Decode((unsigned char*)buffer, (unsigned char*)credentialText, innTekstLengde);
		if (dekodetLengde > 0)
		{
			char* midUsername = buffer;
			char* midPassword = nullptr;

			for (size_t i = 0; ; i++)
			{
				if (buffer[i] == '\0') break;
				else if (buffer[i] == ':')
				{
					buffer[i] = '\0';
					midPassword = buffer + i + 1;
					break;
				}
			}

			wchar_t wbuffer[230];

			Tekst::AnsiDecode(midUsername, wbuffer);
			username = ref new Platform::String(wbuffer);

			if (midPassword != nullptr)
			{
				Tekst::AnsiDecode(midPassword, wbuffer);
				password = ref new Platform::String(wbuffer);
			}
		}
	}
	else
	{
		char *buffer = new char[innTekstLengde + 1];
		int dekodetLengde = Base64Decode((unsigned char*)buffer, (unsigned char*)credentialText, innTekstLengde);

		if (dekodetLengde > 0)
		{
			char* midUsername = buffer;
			char* midPassword = nullptr;

			for (size_t i = 0; ; i++)
			{
				if (buffer[i] == '\0') break;
				else if (buffer[i] == ':')
				{
					buffer[i] = '\0';
					midPassword = buffer + i + 1;
					break;
				}
			}

			wchar_t *wbuffer = new wchar_t[strlen(midUsername) + 1];
			Tekst::AnsiDecode(midUsername, wbuffer);
			username = ref new Platform::String(wbuffer);
			delete[] wbuffer;

			if (midPassword != nullptr)
			{
				wbuffer = new wchar_t[strlen(midPassword) + 1];
				Tekst::AnsiDecode(midPassword, wbuffer);
				password = ref new Platform::String(wbuffer);
				delete[] wbuffer;
			}
			delete[] buffer;
		}
		else delete[] buffer;
	}
}

int HttpCredential::Base64Decode(unsigned char *utTekst, const unsigned char *innTekst, int innLengde)
{
	const char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	*utTekst = 0;
	if (*innTekst == 0) return 0;
	if (innLengde % 4 != 0)  return 0;

	unsigned char *pUtTekst = utTekst;
	do
	{
		const char *pTable = strchr(table, innTekst[0]);
		char a = pTable ? pTable - table : 0;
		pTable = strchr(table, innTekst[1]);
		char b = pTable ? pTable - table : 0;
		pTable = strchr(table, innTekst[2]);
		char c = pTable ? pTable - table : 0;
		pTable = strchr(table, innTekst[3]);
		char d = pTable ? pTable - table : 0;
		*pUtTekst++ = (a << 2) | (b >> 4);
		*pUtTekst++ = (b << 4) | (c >> 2);
		*pUtTekst++ = (c << 6) | d;
		if (!(innTekst[1] && strchr(table, innTekst[1]) != nullptr))
		{
			pUtTekst -= 2;
			break;
		}
		else if (!(innTekst[2] && strchr(table, innTekst[2]) != nullptr))
		{
			pUtTekst -= 2;
			break;
		}
		else if (!(innTekst[3] && strchr(table, innTekst[3]) != nullptr))
		{
			pUtTekst--;
			break;
		}
		innTekst += 4;
		while (*innTekst && (*innTekst == 13 || *innTekst == 10)) innTekst++;
	} while (innLengde -= 4);

	*pUtTekst = 0;
	return pUtTekst - utTekst;
}