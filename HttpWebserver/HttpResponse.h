#pragma once

#include "FileTypes.h"
#include "Cookie.h"

namespace HttpWebServer
{
	namespace DetailClasses
	{
		/// <summary>Set data for http response.</summary>
		public ref class HttpResponse sealed
		{
		public:
			/// <summary>Set string content to the client. Select file type (normal text type). MaxAge is how long the text can be stored without downloading again in second.</summary>
			[Windows::Foundation::Metadata::DefaultOverloadAttribute]
			void SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge);

			/// <summary>Set string content to the client. Select file type (normal text type). MaxAge is how long the text can be stored without downloading again in second. A cookie to the client can be attached.</summary>
			void SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge, Cookie ^cookie);

			/// <summary>Set string content to the client. Select file type (normal text type). MaxAge is how long the text can be stored without downloading again in second. Array with cookies to the client can be attached.</summary>
			void SetData(Platform::String ^data, FileTypes fileType, uint16 maxAge, const Platform::Array<Cookie^> ^cookies);

			/// <summary>Set Ibuffer data content to the client. If the data is text it must be in UTF-8 format. Select file type. MaxAge is how long the text can be stored without downloading again in second.</summary>
			void SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge);

			/// <summary>Set Ibuffer data content to the client. If the data is text it must be in UTF-8 format. Select file type. MaxAge is how long the text can be stored without downloading again in second. A cookie to the client can be attached.</summary>
			void SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge, Cookie ^cookie);

			/// <summary>Set Ibuffer data content to the client. If the data is text it must be in UTF-8 format. Select file type. MaxAge is how long the text can be stored without downloading again in second. Array with cookies to the client can be attached.</summary>
			void SetData(Windows::Storage::Streams::IBuffer ^data, FileTypes fileType, uint16 maxAge, const Platform::Array<Cookie^> ^cookies);

			/// <summary>Set a file to send to client. MaxAge is how long the text can be stored without downloading again in second. Response settings must not be driven asynchronously with await. Set file like this (c#): "Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Filer/TestAnsi.html")).AsTask().Result"</summary>
			void SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge);

			/// <summary>Set a file to send to client. MaxAge is how long the text can be stored without downloading again in second. A cookie to the client can be attached. Response settings must not be driven asynchronously with await. Set file like this (c#): "Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Filer/TestAnsi.html")).AsTask().Result"</summary>
			void SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge, Cookie ^cookie);

			/// <summary>Set a file to send to client. MaxAge is how long the text can be stored without downloading again in second. Array with cookies to the client can be attached. Response settings must not be driven asynchronously with await. Set file like this (c#): "Windows.Storage.StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Filer/TestAnsi.html")).AsTask().Result"</summary>
			void SetData(Windows::Storage::IStorageFile ^file, uint16 maxAge, const Platform::Array<Cookie^> ^cookies);

			/// <summary>Gives feedback that request is Unauthorized. Client will ask normal user for a username/password and send a new request with this attached. Name is the name of the authorization and may be used by web browsers to save your password.</summary>
			void SetUnauthorized(Platform::String ^name);

			/// <summary>Gives feedback on to go to a new web address. Example: /login or http://www.sol.no/. If permanently = true status code "301 Moved Permanently" is used else status code "303 See Other" is used.</summary>
			void SetRedirect(Platform::String ^location, bool permanently);

			/// <summary>Gives feedback that the request is not found.</summary>
			void SetNotFound();

		internal:
			HttpResponse(bool isHeadMethod);
			bool isSet = false;
			bool isData = false;
			int typeData = 0; // 0 = String, 1 = IBuffer og 2 = IStorageFile
			std::string notDataTekst;
			Platform::String ^dataString;
			Windows::Storage::Streams::IBuffer ^dataBuffer;
			Windows::Storage::IStorageFile ^file;
			FileTypes fileType;
			std::string cookieTekst;
			int maxAge;

		private:
			bool mIsHeadMethod;
		};
	}
}

