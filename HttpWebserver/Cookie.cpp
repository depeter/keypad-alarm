#include "pch.h"
#include "Cookie.h"

using namespace HttpWebServer;

Cookie::Cookie(Platform::String ^name, Platform::String ^value)
{
	// Setter SetText i formatet: "Set-Cookie: Bruker=Geir+Morten+Jakobsen"

	this->name = name;
	this->value = value;

	char buffer[300];
	bool dynamisk = false;
	
	size_t lengdeName = Tekst::UrlEncodeLength(name->Data());
	size_t lengdeValue = Tekst::UrlEncodeLength(value->Data());

	char *setText = buffer;

	if (lengdeName + lengdeValue > 300 - 16) // 16 er faste plasser
	{
		setText = new char[lengdeName + lengdeValue + 16];
		dynamisk = true;
	}

	// Setter inn "Set-Cookie: "
	setText[0] = 'S';
	setText[1] = 'e';
	setText[2] = 't';
	setText[3] = '-';
	setText[4] = 'C';
	setText[5] = 'o';
	setText[6] = 'o';
	setText[7] = 'k';
	setText[8] = 'i';
	setText[9] = 'e';
	setText[10] = ':';
	setText[11] = ' ';

	Tekst::UrlEncode(name->Data(), setText + 12); // Setter inn cookie navn
	setText[12 + lengdeName] = '=';
	Tekst::UrlEncode(value->Data(), setText + 13 + lengdeName); // Setter inn cookie verdi
	setText[13 + lengdeName + lengdeValue] = '\r';
	setText[14 + lengdeName + lengdeValue] = '\n';
	setText[15 + lengdeName + lengdeValue] = '\0';

	SetText = setText;

	if (dynamisk) delete[] setText;
}

Cookie::Cookie(Platform::String ^name, Platform::String ^value, Windows::Foundation::DateTime expires)
{
	// Setter SetText i formatet: "Set-Cookie: Bruker=Geir+Morten+Jakobsen; Expires=Sun, 25 Oct 2015 08:14:08 GMT"

	this->name = name;
	this->value = value;

	char buffer[400];
	bool dynamisk = false;

	size_t lengdeName = Tekst::UrlEncodeLength(name->Data());
	size_t lengdeValue = Tekst::UrlEncodeLength(value->Data());

	char *setText = buffer;

	if (lengdeName + lengdeValue > 400 - 55) // 55 er faste plasser
	{
		setText = new char[lengdeName + lengdeValue + 55];
		dynamisk = true;
	}

	// Setter inn "Set-Cookie: "
	setText[0] = 'S';
	setText[1] = 'e';
	setText[2] = 't';
	setText[3] = '-';
	setText[4] = 'C';
	setText[5] = 'o';
	setText[6] = 'o';
	setText[7] = 'k';
	setText[8] = 'i';
	setText[9] = 'e';
	setText[10] = ':';
	setText[11] = ' ';
	char* pSetText = setText + 12;

	Tekst::UrlEncode(name->Data(), pSetText); // Setter inn cookie navn
	pSetText += lengdeName;

	pSetText[0] = '=';
	pSetText++;

	Tekst::UrlEncode(value->Data(), pSetText); // Setter inn cookie verdi
	pSetText += lengdeValue;

	// Setter inn "; expires="
	pSetText[0] = ';';
	pSetText[1] = ' ';
	pSetText[2] = 'e';
	pSetText[3] = 'x';
	pSetText[4] = 'p';
	pSetText[5] = 'i';
	pSetText[6] = 'r';
	pSetText[7] = 'e';
	pSetText[8] = 's';
	pSetText[9] = '=';
	pSetText += 10;

	Tid::GetHttpDateTime(expires, pSetText);
	pSetText += 29;

	pSetText[0] = '\r';
	pSetText[1] = '\n';
	pSetText[2] = '\0';

	SetText = setText;

	if (dynamisk) delete[] setText;
}

Cookie::Cookie(Platform::String ^name, Platform::String ^value, Platform::String ^domain, Platform::String ^path)
{
	// Setter SetText i formatet: "Set-Cookie: Bruker=Geir+Morten+Jakobsen; Domain=.sol.no; Path=/Account"

	this->name = name;
	this->value = value;

	char buffer[500];
	bool dynamisk = false;

	size_t lengdeName = Tekst::UrlEncodeLength(name->Data());
	size_t lengdeValue = Tekst::UrlEncodeLength(value->Data());
	size_t lengdeDomain = domain->Length();
	size_t lengdePath = path->Length();

	size_t totalLengde = lengdeName + lengdeValue + 16;
	if (lengdeDomain > 0) totalLengde += lengdeDomain + 9;
	if (lengdePath > 0) totalLengde += lengdePath + 7;

	char *setText = buffer;

	if (totalLengde > 500)
	{
		setText = new char[totalLengde];
		dynamisk = true;
	}

	// Setter inn "Set-Cookie: "
	setText[0] = 'S';
	setText[1] = 'e';
	setText[2] = 't';
	setText[3] = '-';
	setText[4] = 'C';
	setText[5] = 'o';
	setText[6] = 'o';
	setText[7] = 'k';
	setText[8] = 'i';
	setText[9] = 'e';
	setText[10] = ':';
	setText[11] = ' ';
	char* pSetText = setText + 12;

	Tekst::UrlEncode(name->Data(), pSetText); // Setter inn cookie navn
	pSetText += lengdeName;

	pSetText[0] = '=';
	pSetText++;

	Tekst::UrlEncode(value->Data(), pSetText); // Setter inn cookie verdi
	pSetText += lengdeValue;

	// Setter inn domain hvis det er det
	if (lengdeDomain > 0)
	{
		pSetText[0] = ';';
		pSetText[1] = ' ';
		pSetText[2] = 'd';
		pSetText[3] = 'o';
		pSetText[4] = 'm';
		pSetText[5] = 'a';
		pSetText[6] = 'i';
		pSetText[7] = 'n';
		pSetText[8] = '=';
		pSetText += 9;

		Tekst::AnsiEncode(domain->Data(), pSetText);
		pSetText += lengdeDomain;
	}

	// Setter inn path hvis det er det
	if (lengdePath > 0)
	{
		pSetText[0] = ';';
		pSetText[1] = ' ';
		pSetText[2] = 'p';
		pSetText[3] = 'a';
		pSetText[4] = 't';
		pSetText[5] = 'h';
		pSetText[6] = '=';
		pSetText += 7;

		Tekst::AnsiEncode(path->Data(), pSetText);
		pSetText += lengdePath;
	}

	pSetText[0] = '\r';
	pSetText[1] = '\n';
	pSetText[2] = '\0';

	SetText = setText;

	if (dynamisk) delete[] setText;
}

Cookie::Cookie(Platform::String ^name, Platform::String ^value, Platform::String ^domain, Platform::String ^path, Windows::Foundation::DateTime expires)
{
	// Setter SetText i formatet: "Set-Cookie: Bruker=Geir+Morten+Jakobsen; Domain=.sol.no; Path=/Account; Expires=Sun, 25 Oct 2015 08:14:08 GMT"

	this->name = name;
	this->value = value;

	char buffer[500];
	bool dynamisk = false;

	size_t lengdeName = Tekst::UrlEncodeLength(name->Data());
	size_t lengdeValue = Tekst::UrlEncodeLength(value->Data());
	size_t lengdeDomain = domain->Length();
	size_t lengdePath = path->Length();

	size_t totalLengde = lengdeName + lengdeValue + 55;
	if (lengdeDomain > 0) totalLengde += lengdeDomain + 9;
	if (lengdePath > 0) totalLengde += lengdePath + 7;

	char *setText = buffer;

	if (totalLengde > 500)
	{
		setText = new char[totalLengde];
		dynamisk = true;
	}

	// Setter inn "Set-Cookie: "
	setText[0] = 'S';
	setText[1] = 'e';
	setText[2] = 't';
	setText[3] = '-';
	setText[4] = 'C';
	setText[5] = 'o';
	setText[6] = 'o';
	setText[7] = 'k';
	setText[8] = 'i';
	setText[9] = 'e';
	setText[10] = ':';
	setText[11] = ' ';
	char* pSetText = setText + 12;

	Tekst::UrlEncode(name->Data(), pSetText); // Setter inn cookie navn
	pSetText += lengdeName;

	pSetText[0] = '=';
	pSetText++;

	Tekst::UrlEncode(value->Data(), pSetText); // Setter inn cookie verdi
	pSetText += lengdeValue;

	// Setter inn domain hvis det er det
	if (lengdeDomain > 0)
	{
		pSetText[0] = ';';
		pSetText[1] = ' ';
		pSetText[2] = 'd';
		pSetText[3] = 'o';
		pSetText[4] = 'm';
		pSetText[5] = 'a';
		pSetText[6] = 'i';
		pSetText[7] = 'n';
		pSetText[8] = '=';
		pSetText += 9;

		Tekst::AnsiEncode(domain->Data(), pSetText);
		pSetText += lengdeDomain;
	}

	// Setter inn path hvis det er det
	if (lengdePath > 0)
	{
		pSetText[0] = ';';
		pSetText[1] = ' ';
		pSetText[2] = 'p';
		pSetText[3] = 'a';
		pSetText[4] = 't';
		pSetText[5] = 'h';
		pSetText[6] = '=';
		pSetText += 7;

		Tekst::AnsiEncode(path->Data(), pSetText);
		pSetText += lengdePath;
	}

	// Setter inn "; expires="
	pSetText[0] = ';';
	pSetText[1] = ' ';
	pSetText[2] = 'e';
	pSetText[3] = 'x';
	pSetText[4] = 'p';
	pSetText[5] = 'i';
	pSetText[6] = 'r';
	pSetText[7] = 'e';
	pSetText[8] = 's';
	pSetText[9] = '=';
	pSetText += 10;

	Tid::GetHttpDateTime(expires, pSetText);
	pSetText += 29;

	pSetText[0] = '\r';
	pSetText[1] = '\n';
	pSetText[2] = '\0';

	SetText = setText;

	if (dynamisk) delete[] setText;
}

Cookie::Cookie(char *cookieRequestText)
{
	char* midName = cookieRequestText;
	char* midValue = nullptr;

	for (size_t i = 0; ; i++)
	{
		if (cookieRequestText[i] == '\0') break;
		else if (cookieRequestText[i] == '=')
		{
			cookieRequestText[i] = '\0';
			midValue = cookieRequestText + i + 1;
			break;
		}
	}

	wchar_t buffer[500];
	size_t lengde = Tekst::UrlDecodeLength(midName);

	if (lengde < 500)
	{
		Tekst::UrlDecode(midName, buffer);
		name = ref new Platform::String(buffer);
	}
	else
	{
		wchar_t *dBuffer = new wchar_t[lengde + 1];
		Tekst::UrlDecode(midName, dBuffer);
		name = ref new Platform::String(dBuffer);
		delete[] dBuffer;
	}

	if (midValue != nullptr)
	{
		lengde = Tekst::UrlDecodeLength(midValue);

		if (lengde < 500)
		{
			Tekst::UrlDecode(midValue, buffer);
			value = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::UrlDecode(midValue, dBuffer);
			value = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}
	else value = "";
}