#pragma once

#include <collection.h>
#include <ppltasks.h>

namespace Tid
{
	// Gir dato string for tid nå i http format. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	std::string GetHttpDateTime();

	// Skriver dato nå til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(char* utTid);

	// Gir dato string for innlagt tid i http format. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	std::string GetHttpDateTime(Windows::Foundation::DateTime tid);

	// Skriver dato for innlagt tid til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(Windows::Foundation::DateTime tid, char* utTid);

	// Gir dato string for inlagt tid i http format. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	std::string GetHttpDateTime(Windows::Foundation::DateTime tid);

	// Skriver dato for innlagt tid til utTid i http format. utTid må ha minst 30 minneplasser med regnet '\0' terminering. Eksempel: "Sun, 18 Oct 2015 16:55:15 GMT"
	void GetHttpDateTime(SYSTEMTIME tid, char* utTid);
}

namespace Tekst
{
	// Finner unicode lengde på en utf-8 string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8DecodeLength(const char *innTekst);

	// Dekoder en utf-8 string til unicode
	void Utf8Decode(const char *innTekst, wchar_t *utTekst);

	// Dekoder en utf-8 string til unicode, innTekst kan mangle \0 terminering på grunn av at det er gitt lengde.
	void Utf8Decode(const char *innTekst, wchar_t *utTekst, size_t lengde);

	// Finner utf-8 lengde på en unicode string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8EncodeLength(const wchar_t *innTekst);

	// Finner utf-8 lengde på en ANSI string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t Utf8EncodeLength(const char *innTekst, size_t lengde);

	// Koder unicode til utf-8 string
	void Utf8Encode(const wchar_t *innTekst, char *utTekst);

	// Koder unicode til utf-8 string
	void Utf8Encode(const char *innTekst, char *utTekst, size_t lengde);

	// Funksjon som finner unicode lengde på en URL kodet string, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t UrlDecodeLength(const char *innTekst);

	// Funksjon som dekoder en URL kodet string til unicode
	void UrlDecode(const char *innTekst, wchar_t *utTekst);

	// Funksjon som finner hvor lang URL kodet string vil bli, lengde er uten '\0' terminering så husk å legge til 1 når plass lages.
	size_t UrlEncodeLength(const wchar_t *innTekst);

	// Funksjon som koder unicode til URL kodet string
	void UrlEncode(const wchar_t *innTekst, char *utTekst);

	// Funksjon som konverterer ANSI tekst til unicode tekst. utTekst må være større eller lik inntekst i minne størrelse.
	void AnsiDecode(const char *innTekst, wchar_t *utTekst);

	// Funksjon som konverterer unicode tekst til ANSI tekst. utTekst må være større eller lik inntekst i minne størrelse.
	void AnsiEncode(const wchar_t *innTekst, char *utTekst);
}

