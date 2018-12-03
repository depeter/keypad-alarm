#include "pch.h"
#include "HttpRequest.h"

using namespace HttpWebServer::DetailClasses;

HttpRequest::HttpRequest(HttpWebServer::HelpClasses::DataHenter &dataHenter, unsigned char *iPAdresseClient)
{
	// RequestMethod
	if (dataHenter.Method < 4) requestMethod = static_cast<RequestMethods>(dataHenter.Method);
	else requestMethod = RequestMethods::GET;

	// URI
	wchar_t buffer[500];
	size_t lengde = Tekst::UrlDecodeLength(dataHenter.URI);
	if (lengde < 500)
	{
		Tekst::UrlDecode(dataHenter.URI, buffer);
		uri = ref new Platform::String(buffer);
	}
	else
	{
		wchar_t *dBuffer = new wchar_t[lengde + 1];
		Tekst::UrlDecode(dataHenter.URI, dBuffer);
		uri = ref new Platform::String(dBuffer);
		delete[] dBuffer;
	}

	// Accept
	if (dataHenter.Accept != nullptr)
	{
		lengde = Tekst::UrlDecodeLength(dataHenter.Accept);
		if (lengde < 500)
		{
			Tekst::UrlDecode(dataHenter.Accept, buffer);
			accept = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::UrlDecode(dataHenter.Accept, dBuffer);
			accept = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	// AcceptLanguage
	if (dataHenter.AcceptLanguage != nullptr)
	{
		lengde = Tekst::UrlDecodeLength(dataHenter.AcceptLanguage);
		if (lengde < 500)
		{
			Tekst::UrlDecode(dataHenter.AcceptLanguage, buffer);
			acceptLanguage = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::UrlDecode(dataHenter.AcceptLanguage, dBuffer);
			acceptLanguage = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	// Host
	if (dataHenter.Host != nullptr)
	{
		lengde = Tekst::UrlDecodeLength(dataHenter.Host);
		if (lengde < 500)
		{
			Tekst::UrlDecode(dataHenter.Host, buffer);
			host = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::UrlDecode(dataHenter.Host, dBuffer);
			host = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	// UserAgent
	if (dataHenter.UserAgent != nullptr)
	{
		lengde = Tekst::UrlDecodeLength(dataHenter.UserAgent);
		if (lengde < 500)
		{
			Tekst::UrlDecode(dataHenter.UserAgent, buffer);
			userAgent = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::UrlDecode(dataHenter.UserAgent, dBuffer);
			userAgent = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	// Authorization
	if (dataHenter.Authorization != nullptr) authorization = ref new HttpCredential(dataHenter.Authorization);
	else authorization = nullptr;

	// Cookies
	if (dataHenter.CookierSize > 0)
	{
		cookies = ref new Platform::Array<Cookie^>(dataHenter.CookierSize);

		for (size_t i = 0; i < dataHenter.CookierSize; i++)
		{
			cookies[i] = ref new Cookie(dataHenter.Cookier[i]);
		}
	}

	// Formdata og Formfiler
	if (dataHenter.MultiPartDataer.size() > 0) // Har multipart data
	{
		std::vector<FormData^> midFormDataer;
		std::vector<FormFile^> midFormFiler;

		if (dataHenter.FormDataerSize > 0)
		{
			for (size_t i = 0; i < dataHenter.FormDataerSize; i++)
			{
				midFormDataer.push_back(ref new FormData(dataHenter.FormDataer[i]));
			}
		}

		for (size_t i = 0; i < dataHenter.MultiPartDataer.size(); i++)
		{
			if (dataHenter.MultiPartDataer[i].IsFile) midFormFiler.push_back(ref new FormFile(dataHenter.MultiPartDataer[i]));
			else midFormDataer.push_back(ref new FormData(dataHenter.MultiPartDataer[i]));
		}

		if (midFormDataer.size() > 0)
		{
			formDatas = ref new Platform::Array<FormData^>(midFormDataer.size());

			for (size_t i = 0; i < midFormDataer.size(); i++)
			{
				formDatas[i] = midFormDataer[i];
			}
		}

		if (midFormFiler.size() > 0)
		{
			formFiles = ref new Platform::Array<FormFile^>(midFormFiler.size());

			for (size_t i = 0; i < midFormFiler.size(); i++)
			{
				formFiles[i] = midFormFiler[i];
			}
		}
	}
	else // Har ikke multipart data
	{
		if (dataHenter.FormDataerSize > 0)
		{
			formDatas = ref new Platform::Array<FormData^>(dataHenter.FormDataerSize);

			for (size_t i = 0; i < dataHenter.FormDataerSize; i++)
			{
				formDatas[i] = ref new FormData(dataHenter.FormDataer[i]);
			}
		}
	}

	// IP adresse
	ipAddress = ref new Platform::Array<uint8>(4) { iPAdresseClient[0], iPAdresseClient[1], iPAdresseClient[2], iPAdresseClient[3] };
}

bool HttpRequest::IsMobileBrowser::get()
{
	if (userAgent == "") return false;

	const wchar_t *tekst = userAgent->Data();
	unsigned int tekstLengde = userAgent->Length();

	// Søker etter Mobile
	if (tekstLengde >= 6)
	{
		for (unsigned int i = 0; i < tekstLengde - 5; i++)
		{
			if ((tekst[i] == L'M' || tekst[i] == L'm') && tekst[i + 1] == L'o' && tekst[i + 2] == L'b' && tekst[i + 3] == L'i' && tekst[i + 4] == L'l' && tekst[i + 5] == L'e') return true;
		}
	}

	return false;
}