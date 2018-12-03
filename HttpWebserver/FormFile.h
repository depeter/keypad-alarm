#pragma once
#include "MultiPartData.h"

namespace HttpWebServer
{
	namespace DetailClasses
	{
		/// <summary>Form file from web request.</summary>
		public ref class FormFile sealed
		{
		public:
			/// <summary>Name of the formfile.</summary>
			property Platform::String ^Name
			{
				Platform::String^ get()
				{
					return name;
				}
			}

			/// <summary>Filename to file.</summary>
			property Platform::String ^Filename
			{
				Platform::String^ get()
				{
					return filename;
				}
			}

			/// <summary>File type.</summary>
			property Platform::String ^ContentType
			{
				Platform::String^ get()
				{
					return contentType;
				}
			}

			/// <summary>File in IBuffer format.</summary>
			property Windows::Storage::Streams::IBuffer ^File
			{
				Windows::Storage::Streams::IBuffer^ get()
				{
					return file;
				}
			}

			/// <summary>Is true if file is text file.</summary>
			bool IsString();

			/// <summary>Get text file as a string. If file is not a text file, empty string is returned.</summary>
			Platform::String^ GetString();

		internal:
			FormFile(HelpClasses::MultiPartData &multiPart);
			char* FaaIbufferPointer(Windows::Storage::Streams::IBuffer ^buffer);

		private:
			Platform::String ^name;
			Platform::String ^filename;
			Platform::String ^contentType;
			Windows::Storage::Streams::IBuffer ^file;
		};
	}
}