#include "pch.h"
#include "FormFile.h"
#include <wrl.h>
#include <robuffer.h>

using namespace HttpWebServer::DetailClasses;
using namespace HttpWebServer::HelpClasses;

bool FormFile::IsString()
{
	if (contentType->Length() < 8) return false;

	const wchar_t *tekst = contentType->Data();

	if (tekst[0] == 't' && tekst[1] == 'e' && tekst[2] == 'x' && tekst[3] == 't' && tekst[4] == '/') return true;
	return false;
}

Platform::String^ FormFile::GetString()
{
	if (file->Length == 0 || !IsString()) return "";

	char *tekst = FaaIbufferPointer(file);

	if (file->Length >= 3 && (unsigned char)tekst[0] == 239 && (unsigned char)tekst[1] == 187 && (unsigned char)tekst[2] == 191) // Er UTF-8
	{
		wchar_t *buffer = new wchar_t[file->Length]; // Blir st�rre enn n�dvendig hvis det er mange multibyte tegn, men buffer slettes fort slik at dette er bedre enn � beregne plass f�rst
		Tekst::Utf8Decode(tekst + 3, buffer, file->Length - 3);
		Platform::String^ nyTekst = ref new Platform::String(buffer);
		delete[] buffer;
		return nyTekst;
	}
	else // Tolkes som ANSI
	{
		wchar_t *buffer = new wchar_t[file->Length + 1];

		for (size_t i = 0; i < file->Length; i++)
		{
			if (tekst[i] >= -96) buffer[i] = static_cast<unsigned char>(tekst[i]);
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else if (tekst[i] == '�') buffer[i] = L'�';
			else buffer[i] = L'?';
		}

		buffer[file->Length] = '\0';
		Platform::String^ nyTekst = ref new Platform::String(buffer);
		delete[] buffer;
		return nyTekst;
	}
}

FormFile::FormFile(MultiPartData &multiPart)
{
	wchar_t buffer[500];
	size_t lengde;

	if (multiPart.Name != nullptr)
	{
		lengde = Tekst::Utf8DecodeLength(multiPart.Name);

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

	if (multiPart.Filename != nullptr)
	{
		lengde = Tekst::Utf8DecodeLength(multiPart.Filename);

		if (lengde < 500)
		{
			Tekst::Utf8Decode(multiPart.Filename, buffer);
			filename = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::Utf8Decode(multiPart.Filename, dBuffer);
			filename = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	if (multiPart.ContentType != nullptr)
	{
		size_t lengde = Tekst::Utf8DecodeLength(multiPart.ContentType);

		if (lengde < 500)
		{
			Tekst::Utf8Decode(multiPart.ContentType, buffer);
			contentType = ref new Platform::String(buffer);
		}
		else
		{
			wchar_t *dBuffer = new wchar_t[lengde + 1];
			Tekst::Utf8Decode(multiPart.ContentType, dBuffer);
			contentType = ref new Platform::String(dBuffer);
			delete[] dBuffer;
		}
	}

	if (multiPart.Content != nullptr && multiPart.ContentLength > 0)
	{
		file = ref new Windows::Storage::Streams::Buffer(multiPart.ContentLength); // Lager Ibuffer
		char *buffer = FaaIbufferPointer(file); // F�r pinter til databuffer i Ibuffer
		memcpy(buffer, multiPart.Content, multiPart.ContentLength); // Kopierer data
		file->Length = multiPart.ContentLength; // Setter lengde p� Ibuffer
	}
	else file = ref new Windows::Storage::Streams::Buffer(0); // Lager en tom Ibuffer
}

char* FormFile::FaaIbufferPointer(Windows::Storage::Streams::IBuffer ^buffer)
{
	// Query the IBufferByteAccess interface.
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
	reinterpret_cast<IInspectable*>(buffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

	// Retrieve the buffer data.
	byte *pBuffer = nullptr;
	bufferByteAccess->Buffer(&pBuffer);
	return (char*)pBuffer;
}