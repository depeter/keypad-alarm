#include "pch.h"
#include "FormData.h"

using namespace HttpWebServer::DetailClasses;
using namespace HttpWebServer::HelpClasses;

FormData::FormData(char *formDataText)
{
	char* midName = formDataText;
	char* midValue = nullptr;

	for (size_t i = 0; ; i++)
	{
		if (formDataText[i] == '\0') break;
		else if (formDataText[i] == '=')
		{
			formDataText[i] = '\0';
			midValue = formDataText + i + 1;
			break;
		}
	}

	size_t lengde = Tekst::UrlDecodeLength(midName);
	wchar_t buffer[500];

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
}

FormData::FormData(MultiPartData &multiPart)
{
	wchar_t buffer[500];

	if (multiPart.Name != nullptr)
	{
		size_t lengde = Tekst::Utf8DecodeLength(multiPart.Name);

		if (lengde < 500)
		{
			Tekst::Utf8Decode(multiPart.Name, buffer);
			name = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::Utf8Decode(multiPart.Name, dBuffer);
			name = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	if (multiPart.Content != nullptr && multiPart.ContentLength > 0)
	{
		if (multiPart.ContentLength < 500)
		{
			Tekst::Utf8Decode(multiPart.Content, buffer, multiPart.ContentLength);
			value = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[multiPart.ContentLength + 1];
			Tekst::Utf8Decode(multiPart.Content, dBuffer, multiPart.ContentLength);
			value = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}
}