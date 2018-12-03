#include "pch.h"

namespace Tid
{
	// Gir dato string for tid nå i http format. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	std::string GetHttpDateTime()
	{
		SYSTEMTIME tid;
		GetSystemTime(&tid);

		char buffer[30];
		GetHttpDateTime(tid, buffer);

		return buffer;
	}

	// Skriver dato nå til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(char* utTid)
	{
		SYSTEMTIME tid;
		GetSystemTime(&tid);

		GetHttpDateTime(tid, utTid);
	}

	// Gir dato string for innlagt tid i http format. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	std::string GetHttpDateTime(Windows::Foundation::DateTime tid)
	{
		ULARGE_INTEGER uitid;
		uitid.QuadPart = tid.UniversalTime;

		FILETIME ftid;
		ftid.dwLowDateTime = uitid.LowPart;
		ftid.dwHighDateTime = uitid.HighPart;

		SYSTEMTIME stid;
		FileTimeToSystemTime(&ftid, &stid);

		char buffer[30];
		GetHttpDateTime(stid, buffer);

		return buffer;
	}

	// Skriver dato for innlagt tid til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(Windows::Foundation::DateTime tid, char* utTid)
	{
		ULARGE_INTEGER uitid;
		uitid.QuadPart = tid.UniversalTime;

		FILETIME ftid;
		ftid.dwLowDateTime = uitid.LowPart;
		ftid.dwHighDateTime = uitid.HighPart;

		SYSTEMTIME stid;
		FileTimeToSystemTime(&ftid, &stid);

		GetHttpDateTime(stid, utTid);
	}

	// Skriver dato for innlagt tid til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(SYSTEMTIME tid, char* utTid)
	{
		const char* tallRekke = "0123456789"; // Array for uttak av tall tegn fra tall

		const char* ukeRekke1 = "SMTWTFS"; // Sun, Mon, Tue, Wed, Thu, Fri, Sat
		const char* ukeRekke2 = "uouehra"; // Sun, Mon, Tue, Wed, Thu, Fri, Sat
		const char* ukeRekke3 = "nneduit"; // Sun, Mon, Tue, Wed, Thu, Fri, Sat

		const char* maaedRekke1 = "JFMAMJJASOND"; // Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
		const char* maaedRekke2 = "aeapauuuecoe"; // Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
		const char* maaedRekke3 = "nbrrynlgptvc"; // Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec

		utTid[0] = ukeRekke1[tid.wDayOfWeek];
		utTid[1] = ukeRekke2[tid.wDayOfWeek];
		utTid[2] = ukeRekke3[tid.wDayOfWeek];
		utTid[3] = ',';
		utTid[4] = ' ';
		utTid[5] = tallRekke[tid.wDay / 10];
		utTid[6] = tallRekke[tid.wDay % 10];
		utTid[7] = ' ';
		utTid[8] = maaedRekke1[tid.wMonth - 1];
		utTid[9] = maaedRekke2[tid.wMonth - 1];
		utTid[10] = maaedRekke3[tid.wMonth - 1];
		utTid[11] = ' ';
		utTid[12] = tallRekke[tid.wYear / 1000];
		utTid[13] = tallRekke[(tid.wYear / 100) % 10];
		utTid[14] = tallRekke[(tid.wYear / 10) % 10];
		utTid[15] = tallRekke[tid.wYear % 10];
		utTid[16] = ' ';
		utTid[17] = tallRekke[tid.wHour / 10];
		utTid[18] = tallRekke[tid.wHour % 10];
		utTid[19] = ':';
		utTid[20] = tallRekke[tid.wMinute / 10];
		utTid[21] = tallRekke[tid.wMinute % 10];
		utTid[22] = ':';
		utTid[23] = tallRekke[tid.wSecond / 10];
		utTid[24] = tallRekke[tid.wSecond % 10];
		utTid[25] = ' ';
		utTid[26] = 'G';
		utTid[27] = 'M';
		utTid[28] = 'T';
		utTid[29] = '\0';
	}
}

namespace Tekst
{
	// Finner unicode lengde på en utf-8 string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8DecodeLength(const char *innTekst)
	{
		size_t lengde = 0;

		for (size_t i = 0; ;)
		{
			if (innTekst[i] == '\0') break;
			else if (innTekst[i] > 0) lengde++, i++; // 1 byte brukes for tegn
			else if ((static_cast<unsigned char>(innTekst[i]) & 224) == 192 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128) lengde++, i += 2; // 2 byte bukes for tegn
			else if ((static_cast<unsigned char>(innTekst[i]) & 240) == 224 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128) lengde++, i += 3; // 3 byte brukes for tegn
			else if ((static_cast<unsigned char>(innTekst[i]) & 248) == 240 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 3]) & 192) == 128) lengde++, i += 4; // 4 byte brukes for tegn
			else i++;
		}

		return lengde;
	}

	// Dekoder en utf-8 string til unicode
	void Utf8Decode(const char *innTekst, wchar_t *utTekst)
	{
		for (size_t i = 0, j = 0; ; )
		{
			if (innTekst[i] == '\0')
			{
				utTekst[j] = innTekst[i];
				break;
			}
			else if (innTekst[i] > 0) utTekst[j++] = innTekst[i++]; // 1 byte brukes for tegn
			else if ((static_cast<unsigned char>(innTekst[i]) & 224) == 192 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128)
			{
				utTekst[j++] = ((innTekst[i] & 31) << 6) | (innTekst[i + 1] & 63); // 2 byte bukes for tegn
				i += 2;
			}
			else if ((static_cast<unsigned char>(innTekst[i]) & 240) == 224 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128)
			{
				utTekst[j++] = ((innTekst[i] & 15) << 12) | ((innTekst[i + 1] & 63) << 6) | (innTekst[i + 2] & 63); // 3 byte brukes for tegn
				i += 3;
			}
			else if ((static_cast<unsigned char>(innTekst[i]) & 248) == 240 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 3]) & 192) == 128) // 4 byte brukes for tegn
			{
				utTekst[j++] = L'�'; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '�'
				i += 4;
			}
			else i++; // Ukjent tegn, droppes
		}
	}

	// Dekoder en utf-8 string til unicode, innTekst kan mangle \0 terminering på grunn av at det er gitt lengde.
	void Utf8Decode(const char *innTekst, wchar_t *utTekst, size_t lengde)
	{
		for (size_t i = 0, j = 0; ; )
		{
			if (innTekst[i] == '\0')
			{
				utTekst[j] = innTekst[i];
				break;
			}
			else if (i >= lengde)
			{
				utTekst[j] = L'\0';
				break;
			}
			else if (innTekst[i] > 0) utTekst[j++] = innTekst[i++]; // 1 byte brukes for tegn
			else if ((static_cast<unsigned char>(innTekst[i]) & 224) == 192 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128)
			{
				utTekst[j++] = ((innTekst[i] & 31) << 6) | (innTekst[i + 1] & 63); // 2 byte bukes for tegn
				i += 2;
			}
			else if ((static_cast<unsigned char>(innTekst[i]) & 240) == 224 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128)
			{
				utTekst[j++] = ((innTekst[i] & 15) << 12) | ((innTekst[i + 1] & 63) << 6) | (innTekst[i + 2] & 63); // 3 byte brukes for tegn
				i += 3;
			}
			else if ((static_cast<unsigned char>(innTekst[i]) & 248) == 240 && (static_cast<unsigned char>(innTekst[i + 1]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 2]) & 192) == 128 && (static_cast<unsigned char>(innTekst[i + 3]) & 192) == 128) // 4 byte brukes for tegn
			{
				utTekst[j++] = L'�'; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '�'
				i += 4;
			}
			else i++;
		}
	}

	// Finner utf-8 lengde på en unicode string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8EncodeLength(const wchar_t *innTekst)
	{
		size_t lengde = 0;

		for (size_t i = 0; ; i++)
		{
			if (innTekst[i] == L'\0') break;
			else if (innTekst[i] < 128) lengde++; // 1 byte brukes for tegn
			else if (innTekst[i] < 2048) lengde += 2; // 2 byte bukes for tegn
			else lengde += 3; // 3 byte bukes for tegn
		}

		return lengde;
	}

	// Finner utf-8 lengde på en ANSI string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8EncodeLength(const char *innTekst, size_t lengde)
	{
		size_t nyLengde = 0;

		for (size_t i = 0; i < lengde; i++)
		{
			if (innTekst[i] >= 0) nyLengde++; // 1 byte brukes for tegn
			else if (innTekst[i] >= -96) nyLengde += 2; // 2 byte bukes for tegn

			else if (innTekst[i] == '€') nyLengde += 3;
			else if (innTekst[i] == '‚') nyLengde += 3;
			else if (innTekst[i] == 'ƒ') nyLengde += 2;
			else if (innTekst[i] == '„') nyLengde += 3;
			else if (innTekst[i] == '…') nyLengde += 3;
			else if (innTekst[i] == '†') nyLengde += 3;
			else if (innTekst[i] == '‡') nyLengde += 3;
			else if (innTekst[i] == 'ˆ') nyLengde += 2;
			else if (innTekst[i] == '‰') nyLengde += 3;
			else if (innTekst[i] == 'Š') nyLengde += 2;
			else if (innTekst[i] == '‹') nyLengde += 3;
			else if (innTekst[i] == 'Œ') nyLengde += 2;
			else if (innTekst[i] == 'Ž') nyLengde += 2;
			else if (innTekst[i] == '‘') nyLengde += 3;
			else if (innTekst[i] == '’') nyLengde += 3;
			else if (innTekst[i] == '“') nyLengde += 3;
			else if (innTekst[i] == '”') nyLengde += 3;
			else if (innTekst[i] == '•') nyLengde += 3;
			else if (innTekst[i] == '–') nyLengde += 3;
			else if (innTekst[i] == '—') nyLengde += 3;
			else if (innTekst[i] == '˜') nyLengde += 2;
			else if (innTekst[i] == '™') nyLengde += 3;
			else if (innTekst[i] == 'š') nyLengde += 2;
			else if (innTekst[i] == '›') nyLengde += 3;
			else if (innTekst[i] == 'œ') nyLengde += 2;
			else if (innTekst[i] == 'ž') nyLengde += 2;
			else if (innTekst[i] == 'Ÿ') nyLengde += 2;
			else nyLengde += 3;
		}

		return nyLengde;
	}

	// Koder unicode til utf-8 string
	void Utf8Encode(const wchar_t *innTekst, char *utTekst)
	{
		for (size_t i = 0, j = 0; ;)
		{
			if (innTekst[i] == L'\0')
			{
				utTekst[j] = static_cast<char>(innTekst[i]);
				break;
			}
			else if (innTekst[i] < 128) utTekst[j++] = static_cast<char>(innTekst[i++]); // 1 byte brukes for tegn
			else if (innTekst[i] < 2048) // 2 byte brukes for tegn
			{
				utTekst[j++] = (innTekst[i] >> 6) | 192;
				utTekst[j++] = (innTekst[i++] & 191) | 128;
			}
			else // 3 byte brukes for tegn
			{
				utTekst[j++] = (innTekst[i] >> 12) | 224;
				utTekst[j++] = ((innTekst[i] >> 6) & 191) | 128;
				utTekst[j++] = (innTekst[i++] & 191) | 128;
			}
		}
	}

	// Koder unicode til utf-8 string
	void Utf8Encode(const char *innTekst, char *utTekst, size_t lengde)
	{
		for (size_t i = 0, j = 0; i < lengde; i++)
		{
			if (innTekst[i] >= 0) utTekst[j++] = innTekst[i]; // 1 byte brukes for tegn
			else if (innTekst[i] >= -96) // 2 byte brukes for tegn
			{
				utTekst[j++] = (static_cast<unsigned char>(innTekst[i]) >> 6) | 192;
				utTekst[j++] = (static_cast<unsigned char>(innTekst[i]) & 191) | 128;
			}
			else if (innTekst[i] == '€') { utTekst[j++] = -30; utTekst[j++] = -126; utTekst[j++] = -84; }
			else if (innTekst[i] == '‚') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -102; }
			else if (innTekst[i] == 'ƒ') { utTekst[j++] = -58; utTekst[j++] = -110; }
			else if (innTekst[i] == '„') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -98; }
			else if (innTekst[i] == '…') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -90; }
			else if (innTekst[i] == '†') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -96; }
			else if (innTekst[i] == '‡') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -95; }
			else if (innTekst[i] == 'ˆ') { utTekst[j++] = -53; utTekst[j++] = -122; }
			else if (innTekst[i] == '‰') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -80; }
			else if (innTekst[i] == 'Š') { utTekst[j++] = -59; utTekst[j++] = -96; }
			else if (innTekst[i] == '‹') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -71; }
			else if (innTekst[i] == 'Œ') { utTekst[j++] = -59; utTekst[j++] = -110; }
			else if (innTekst[i] == 'Ž') { utTekst[j++] = -59; utTekst[j++] = -67; }
			else if (innTekst[i] == '‘') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -104; }
			else if (innTekst[i] == '’') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -103; }
			else if (innTekst[i] == '“') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -100; }
			else if (innTekst[i] == '”') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -99; }
			else if (innTekst[i] == '•') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -94; }
			else if (innTekst[i] == '–') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -109; }
			else if (innTekst[i] == '—') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -108; }
			else if (innTekst[i] == '˜') { utTekst[j++] = -53; utTekst[j++] = -100; }
			else if (innTekst[i] == '™') { utTekst[j++] = -30; utTekst[j++] = -124; utTekst[j++] = -94; }
			else if (innTekst[i] == 'š') { utTekst[j++] = -59; utTekst[j++] = -95; }
			else if (innTekst[i] == '›') { utTekst[j++] = -30; utTekst[j++] = -128; utTekst[j++] = -70; }
			else if (innTekst[i] == 'œ') { utTekst[j++] = -59; utTekst[j++] = -109; }
			else if (innTekst[i] == 'ž') { utTekst[j++] = -59; utTekst[j++] = -66; }
			else if (innTekst[i] == 'Ÿ') { utTekst[j++] = -59; utTekst[j++] = -72; }
			else { utTekst[j++] = -17; utTekst[j++] = -65; utTekst[j++] = -67; } // Ellers settes tegnet "�"
		}
	}

	// Funksjon som finner integer verdi fra en prosent kodet hex verdi
	int ValueFromHex(const char *innTekst, int nummer)
	{
		innTekst += 3 * nummer;
		int verdi = 0;

		if (innTekst[0] != '%' || innTekst[1] == '\0' || innTekst[2] == '\0') return 0;

		if (innTekst[1] >= 48 && innTekst[1] <= 57) verdi = (innTekst[1] - 48) * 16;
		else if (innTekst[1] >= 65 && innTekst[1] <= 70) verdi = (innTekst[1] - 55) * 16;
		else if (innTekst[1] >= 97 && innTekst[1] <= 102) verdi = (innTekst[1] - 87) * 16;
		else return 0;

		if (innTekst[2] >= 48 && innTekst[2] <= 57) verdi += innTekst[2] - 48;
		else if (innTekst[2] >= 65 && innTekst[2] <= 70) verdi += innTekst[2] - 55;
		else if (innTekst[2] >= 97 && innTekst[2] <= 102) verdi += innTekst[2] - 87;
		else return 0;

		return verdi;
	}

	// Funksjon som finner unicode lengde på en URL kodet string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t UrlDecodeLength(const char *innTekst)
	{
		size_t lengde = 0;

		for (size_t i = 0; ; )
		{
			if (innTekst[i] == '\0') break;  // Slutt på tekst
			else if (innTekst[i] == '%') // Er muligens unicode tegn og kan bestå av flere 1 byte
			{
				int midVerdi = ValueFromHex(&innTekst[i], 0);

				if (midVerdi > 0)
				{
					if (midVerdi < 128)
					{
						lengde++; // 1 byte brukes for tegn
						i += 3;
					}
					else if ((midVerdi & 224) == 192 && (ValueFromHex(&innTekst[i], 1) & 192) == 128)
					{
						lengde++; // 2 byte bukes for tegn
						i += 6;
					}
					else if ((midVerdi & 240) == 224 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128)
					{
						lengde++; // 3 byte brukes for tegn
						i += 9;
					}
					else if ((midVerdi & 248) == 240 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128 && (ValueFromHex(&innTekst[i], 3) & 192) == 128) // 4 byte brukes for tegn
					{
						lengde++; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '?'
						i += 12;
					}
					else if ((midVerdi & 252) == 248 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128 && (ValueFromHex(&innTekst[i], 3) & 192) == 128 && (ValueFromHex(&innTekst[i], 4) & 192) == 128) // 5 byte brukes for tegn
					{
						lengde++; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '?'
						i += 15;
					}
					else if ((midVerdi & 254) == 252 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128 && (ValueFromHex(&innTekst[i], 3) & 192) == 128 && (ValueFromHex(&innTekst[i], 4) & 192) == 128 && (ValueFromHex(&innTekst[i], 5) & 192) == 128) // 6 byte brukes for tegn
					{
						lengde++; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '?'
						i += 18;
					}
					else
					{
						lengde++;
						i += 3;
					}
				}
				else
				{
					lengde++;
					i++;
				}
			}
			else if (innTekst[i] > 0) lengde++, i++; // 1 byte brukes for tegn
			else // URL er kodet feil og tolkes derfor som ANSI kodet
			{
				if (innTekst[i] >= -96) lengde++; // 1 byte brukes for tegn
				else if (innTekst[i] == '€') lengde++;
				else if (innTekst[i] == '‚') lengde++;
				else if (innTekst[i] == 'ƒ') lengde++;
				else if (innTekst[i] == '„') lengde++;
				else if (innTekst[i] == '…') lengde++;
				else if (innTekst[i] == '†') lengde++;
				else if (innTekst[i] == '‡') lengde++;
				else if (innTekst[i] == 'ˆ') lengde++;
				else if (innTekst[i] == '‰') lengde++;
				else if (innTekst[i] == 'Š') lengde++;
				else if (innTekst[i] == '‹') lengde++;
				else if (innTekst[i] == 'Œ') lengde++;
				else if (innTekst[i] == 'Ž') lengde++;
				else if (innTekst[i] == '‘') lengde++;
				else if (innTekst[i] == '’') lengde++;
				else if (innTekst[i] == '“') lengde++;
				else if (innTekst[i] == '”') lengde++;
				else if (innTekst[i] == '•') lengde++;
				else if (innTekst[i] == '–') lengde++;
				else if (innTekst[i] == '—') lengde++;
				else if (innTekst[i] == '˜') lengde++;
				else if (innTekst[i] == '™') lengde++;
				else if (innTekst[i] == 'š') lengde++;
				else if (innTekst[i] == '›') lengde++;
				else if (innTekst[i] == 'œ') lengde++;
				else if (innTekst[i] == 'ž') lengde++;
				else if (innTekst[i] == 'Ÿ') lengde++;

				i++;
			}
		}

		return lengde;
	}

	// Funksjon som dekoder en URL kodet string til unicode
	void UrlDecode(const char *innTekst, wchar_t *utTekst)
	{
		for (size_t i = 0, j = 0; ;)
		{
			if (innTekst[i] == '\0') // Slutt på tekst
			{
				utTekst[j] = innTekst[i];
				break;
			}
			else if (innTekst[i] == '+') // Er mellomrom og består av 1 byte
			{
				utTekst[j++] = L' ';
				i++;
			}
			else if (innTekst[i] == '%') // Er muligens unicode tegn og kan bestå av flere 1 byte
			{
				int midVerdi = ValueFromHex(&innTekst[i], 0);

				if (midVerdi > 0)
				{
					if (midVerdi < 128)
					{
						utTekst[j++] = midVerdi; // 1 byte brukes for tegn
						i += 3;
					}
					else if ((midVerdi & 224) == 192 && (ValueFromHex(&innTekst[i], 1) & 192) == 128)
					{
						utTekst[j++] = ((midVerdi & 31) << 6) | (ValueFromHex(&innTekst[i], 1) & 63); // 2 byte bukes for tegn
						i += 6;
					}
					else if ((midVerdi & 240) == 224 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128)
					{
						utTekst[j++] = ((midVerdi & 15) << 12) | ((ValueFromHex(&innTekst[i], 1) & 63) << 6) | (ValueFromHex(&innTekst[i], 2) & 63); // 3 byte brukes for tegn
						i += 9;
					}
					else if ((midVerdi & 248) == 240 && (ValueFromHex(&innTekst[i], 1) & 192) == 128 && (ValueFromHex(&innTekst[i], 2) & 192) == 128 && (ValueFromHex(&innTekst[i], 3) & 192) == 128) // 4 byte brukes for tegn
					{
						utTekst[j++] = L'�'; // wchar_t er kun 16 bit og tegnet må ha 32 bits plass, skriver derfor '�'
						i += 12;
					}
					else
					{
						utTekst[j++] = midVerdi;
						i += 3;
					}
				}
				else
				{
					utTekst[j++] = L'%';
					i++;
				}
			}
			else if (innTekst[i] > 0) utTekst[j++] = innTekst[i++]; // 1 byte brukes for tegn
			else // URL er kodet feil og tolkes derfor som ANSI kodet
			{
				if (innTekst[i] >= -96) utTekst[j++] = static_cast<unsigned char>(innTekst[i]); // 1 byte brukes for tegn
				else if (innTekst[i] == '€') utTekst[j++] = L'€';
				else if (innTekst[i] == '‚') utTekst[j++] = L'‚';
				else if (innTekst[i] == 'ƒ') utTekst[j++] = L'ƒ';
				else if (innTekst[i] == '„') utTekst[j++] = L'„';
				else if (innTekst[i] == '…') utTekst[j++] = L'…';
				else if (innTekst[i] == '†') utTekst[j++] = L'†';
				else if (innTekst[i] == '‡') utTekst[j++] = L'‡';
				else if (innTekst[i] == 'ˆ') utTekst[j++] = L'ˆ';
				else if (innTekst[i] == '‰') utTekst[j++] = L'‰';
				else if (innTekst[i] == 'Š') utTekst[j++] = L'Š';
				else if (innTekst[i] == '‹') utTekst[j++] = L'‹';
				else if (innTekst[i] == 'Œ') utTekst[j++] = L'Œ';
				else if (innTekst[i] == 'Ž') utTekst[j++] = L'Ž';
				else if (innTekst[i] == '‘') utTekst[j++] = L'‘';
				else if (innTekst[i] == '’') utTekst[j++] = L'’';
				else if (innTekst[i] == '“') utTekst[j++] = L'“';
				else if (innTekst[i] == '”') utTekst[j++] = L'”';
				else if (innTekst[i] == '•') utTekst[j++] = L'•';
				else if (innTekst[i] == '–') utTekst[j++] = L'–';
				else if (innTekst[i] == '—') utTekst[j++] = L'—';
				else if (innTekst[i] == '˜') utTekst[j++] = L'˜';
				else if (innTekst[i] == '™') utTekst[j++] = L'™';
				else if (innTekst[i] == 'š') utTekst[j++] = L'š';
				else if (innTekst[i] == '›') utTekst[j++] = L'›';
				else if (innTekst[i] == 'œ') utTekst[j++] = L'œ';
				else if (innTekst[i] == 'ž') utTekst[j++] = L'ž';
				else if (innTekst[i] == 'Ÿ') utTekst[j++] = L'Ÿ';

				i++;
			}
		}
	}

	// Funksjon som finner hvor lang URL kodet string vil bli, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t UrlEncodeLength(const wchar_t *innTekst)
	{
		size_t lengde = 0;

		for (size_t i = 0; ; i++)
		{
			if (innTekst[i] == L'\0') break;
			else if (innTekst[i] < 128)
			{
				if (innTekst[i] == 32 || innTekst[i] == 45 || innTekst[i] == 46 || innTekst[i] == 95 || (innTekst[i] >= 97 && innTekst[i] <= 122) || (innTekst[i] >= 65 && innTekst[i] <= 90) || (innTekst[i] >= 48 && innTekst[i] <= 57) || innTekst[i] == 126) lengde++; // Består av 1 byte
				else lengde += 3; // Består av 3 byte
			}
			else if (innTekst[i] < 2048) lengde += 6; // 2 byte brukes for tegn
			else lengde += 9; // 3 byte brukes for tegn
		}

		return lengde;
	}

	// Skriver value som hex verdi eks. %3F
	void WriteHexValue(int value, char *tekst)
	{
		const char *hexTall = "0123456789ABCDEF";

		tekst[0] = '%';
		tekst[1] = hexTall[(value - (value % 16)) / 16];
		tekst[2] = hexTall[value % 16];
	}

	// Funksjon som koder unicode til URL kodet string
	void UrlEncode(const wchar_t *innTekst, char *utTekst)
	{
		for (size_t i = 0, j = 0; ;)
		{
			if (innTekst[i] == L'\0')
			{
				utTekst[j] = static_cast<char>(innTekst[i]);
				break;
			}
			else if (innTekst[i] == ' ')
			{
				utTekst[j++] = '+';
				i++;
			}
			else if (innTekst[i] < 128)
			{
				if (innTekst[i] == 45 || innTekst[i] == 46 || innTekst[i] == 95 || (innTekst[i] >= 97 && innTekst[i] <= 122) || (innTekst[i] >= 65 && innTekst[i] <= 90) || (innTekst[i] >= 48 && innTekst[i] <= 57) || innTekst[i] == 126) // Består av 1 byte
				{
					utTekst[j++] = static_cast<char>(innTekst[i++]); // 1 byte brukes for tegn
				}
				else // Består av 3 byte
				{
					WriteHexValue(innTekst[i++], &utTekst[j]);
					j += 3;
				}
			}
			else if (innTekst[i] < 2048) // 2 byte brukes for tegn
			{
				WriteHexValue((innTekst[i] >> 6) | 192, &utTekst[j]);
				WriteHexValue((innTekst[i++] & 191) | 128, &utTekst[j + 3]);
				j += 6;
			}
			else // 3 byte brukes for tegn
			{
				WriteHexValue((innTekst[i] >> 12) | 224, &utTekst[j]);
				WriteHexValue(((innTekst[i] >> 6) & 191) | 128, &utTekst[j + 3]);
				WriteHexValue((innTekst[i++] & 191) | 128, &utTekst[j + 6]);
				j += 9;
			}
		}
	}

	// Funksjon som konverterer ANSI tekst til unicode tekst. utTekst må være større eller lik inntekst i minne størrelse.
	void AnsiDecode(const char *innTekst, wchar_t *utTekst)
	{
		for (size_t i = 0; ; i++)
		{
			if (innTekst[i] == '\0')
			{
				utTekst[i] = innTekst[i];
				break;
			}
			else if (innTekst[i] >= -96) utTekst[i] = static_cast<unsigned char>(innTekst[i]);
			else if (innTekst[i] == '€') utTekst[i] = L'€';
			else if (innTekst[i] == '‚') utTekst[i] = L'‚';
			else if (innTekst[i] == 'ƒ') utTekst[i] = L'ƒ';
			else if (innTekst[i] == '„') utTekst[i] = L'„';
			else if (innTekst[i] == '…') utTekst[i] = L'…';
			else if (innTekst[i] == '†') utTekst[i] = L'†';
			else if (innTekst[i] == '‡') utTekst[i] = L'‡';
			else if (innTekst[i] == 'ˆ') utTekst[i] = L'ˆ';
			else if (innTekst[i] == '‰') utTekst[i] = L'‰';
			else if (innTekst[i] == 'Š') utTekst[i] = L'Š';
			else if (innTekst[i] == '‹') utTekst[i] = L'‹';
			else if (innTekst[i] == 'Œ') utTekst[i] = L'Œ';
			else if (innTekst[i] == 'Ž') utTekst[i] = L'Ž';
			else if (innTekst[i] == '‘') utTekst[i] = L'‘';
			else if (innTekst[i] == '’') utTekst[i] = L'’';
			else if (innTekst[i] == '“') utTekst[i] = L'“';
			else if (innTekst[i] == '”') utTekst[i] = L'”';
			else if (innTekst[i] == '•') utTekst[i] = L'•';
			else if (innTekst[i] == '–') utTekst[i] = L'–';
			else if (innTekst[i] == '—') utTekst[i] = L'—';
			else if (innTekst[i] == '˜') utTekst[i] = L'˜';
			else if (innTekst[i] == '™') utTekst[i] = L'™';
			else if (innTekst[i] == 'š') utTekst[i] = L'š';
			else if (innTekst[i] == '›') utTekst[i] = L'›';
			else if (innTekst[i] == 'œ') utTekst[i] = L'œ';
			else if (innTekst[i] == 'ž') utTekst[i] = L'ž';
			else if (innTekst[i] == 'Ÿ') utTekst[i] = L'Ÿ';
			else utTekst[i] = L'�';
		}
	}

	// Funksjon som konverterer unicode tekst til ANSI tekst. utTekst må være større eller lik inntekst i minne størrelse.
	void AnsiEncode(const wchar_t *innTekst, char *utTekst)
	{
		for (size_t i = 0; ; i++)
		{
			if (innTekst[i] == L'\0')
			{
				utTekst[i] = static_cast<char>(innTekst[i]);
				break;
			}
			else if (innTekst[i] < 128 || (innTekst[i] < 256 && innTekst[i] > 159)) utTekst[i] = static_cast<char>(innTekst[i]);
			else if (innTekst[i] == L'€') utTekst[i] = '€';
			else if (innTekst[i] == L'‚') utTekst[i] = '‚';
			else if (innTekst[i] == L'ƒ') utTekst[i] = 'ƒ';
			else if (innTekst[i] == L'„') utTekst[i] = '„';
			else if (innTekst[i] == L'…') utTekst[i] = '…';
			else if (innTekst[i] == L'†') utTekst[i] = '†';
			else if (innTekst[i] == L'‡') utTekst[i] = '‡';
			else if (innTekst[i] == L'ˆ') utTekst[i] = 'ˆ';
			else if (innTekst[i] == L'‰') utTekst[i] = '‰';
			else if (innTekst[i] == L'Š') utTekst[i] = 'Š';
			else if (innTekst[i] == L'‹') utTekst[i] = '‹';
			else if (innTekst[i] == L'Œ') utTekst[i] = 'Œ';
			else if (innTekst[i] == L'Ž') utTekst[i] = 'Ž';
			else if (innTekst[i] == L'‘') utTekst[i] = '‘';
			else if (innTekst[i] == L'’') utTekst[i] = '’';
			else if (innTekst[i] == L'“') utTekst[i] = '“';
			else if (innTekst[i] == L'”') utTekst[i] = '”';
			else if (innTekst[i] == L'•') utTekst[i] = '•';
			else if (innTekst[i] == L'–') utTekst[i] = '–';
			else if (innTekst[i] == L'—') utTekst[i] = '—';
			else if (innTekst[i] == L'˜') utTekst[i] = '˜';
			else if (innTekst[i] == L'™') utTekst[i] = '™';
			else if (innTekst[i] == L'š') utTekst[i] = 'š';
			else if (innTekst[i] == L'›') utTekst[i] = '›';
			else if (innTekst[i] == L'œ') utTekst[i] = 'œ';
			else if (innTekst[i] == L'ž') utTekst[i] = 'ž';
			else if (innTekst[i] == L'Ÿ') utTekst[i] = 'Ÿ';
			else utTekst[i] = '?';
		}
	}
}