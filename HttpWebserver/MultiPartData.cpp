#include "pch.h"
#include "MultiPartData.h"

using namespace HttpWebServer::HelpClasses;

// Data del fra multipart
MultiPartData::MultiPartData(char *content, int lengde)
{
	for (int i = 0; i < lengde - 3; i++)
	{
		if (content[i] == 'n' && content[i + 1] == 'a' && content[i + 2] == 'm' && content[i + 3] == 'e' && i < lengde - 6 && content[i + 4] == '=' && content[i + 5] == '"') // Leter etter name="
		{
			i += 6;
			Name = content + i;

			for (; ; i++)
			{
				if (i >= lengde || content[i] == '\0' || content[i] == '\r' || content[i] == '\n') // Sikre seg mot feil i innholdet
				{
					Name = nullptr;
					return;
				}
				else if (content[i] == '"') // Slutt av name funnet
				{
					content[i] = '\0';
					break;
				}
			}
		}
		else if (content[i] == 'f' && content[i + 1] == 'i' && content[i + 2] == 'l' && content[i + 3] == 'e' && i < lengde - 10 && content[i + 4] == 'n' && content[i + 5] == 'a' && content[i + 6] == 'm' && content[i + 7] == 'e' && content[i + 8] == '=' && content[i + 9] == '"') // Leter etter filename="
		{
			i += 10;
			Filename = content + i;
			IsFile = true;

			for (; ; i++)
			{
				if (i >= lengde || content[i] == '\0' || content[i] == '\r' || content[i] == '\n')  // Sikre seg mot feil i innholdet
				{
					Filename = nullptr;
					return;
				}
				else if (content[i] == '"') // Slutt av filename funnet
				{
					content[i] = '\0';
					break;
				}
			}
		}
		else if ((content[i] == 'C' || content[i] == 'c') && content[i + 1] == 'o' && content[i + 2] == 'n' && content[i + 3] == 't' && i < lengde - 14 && content[i + 4] == 'e' && content[i + 5] == 'n' && content[i + 6] == 't' && content[i + 7] == '-' && (content[i + 8] == 'T' || content[i + 8] == 't') && content[i + 9] == 'y' && content[i + 10] == 'p' && content[i + 11] == 'e' && content[i + 12] == ':' && content[i + 13] == ' ') // Leter etter Content-Type: 
		{
			i += 14;
			ContentType = content + i;
			IsFile = true;

			for (; ; i++)
			{
				if (i >= lengde || content[i] == '\0' || content[i] == '\n')  // Sikre seg mot feil i innholdet
				{
					ContentType = nullptr;
					return;
				}
				else if (content[i] == '\r' && i < lengde - 3 && content[i + 1] == '\n' && content[i + 2] == '\r' && content[i + 3] == '\n') // Slutt av content type funnet og etter dette begynner content
				{
					content[i] = '\0';
					goto ContentFunnet; // Bruker goto fordi søket der blir feil når det settes inn \0 terminering
				}
				else if (content[i] == '\r' && i < lengde - 1 && content[i + 1] == '\n') // Slutt av content type funnet
				{
					content[i] = '\0';
					break;
				}
			}
		}
		else if (content[i] == '\r' && content[i + 1] == '\n' && content[i + 2] == '\r' && content[i + 3] == '\n') // 2 linjeskift nå kommer content.
		{
		ContentFunnet:
			if (lengde - i - 4 > 0)
			{
				Content = content + i + 4;
				ContentLength = lengde - i - 4;
			}
			return;
		}
	}
}

// Brukes når fil legges til direkte fra content
MultiPartData::MultiPartData(char *content, char *contentType, int contentLength)
{
	IsFile = true;
	Content = content;
	ContentType = contentType;
	ContentLength = contentLength;
}