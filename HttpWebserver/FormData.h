#pragma once
#include "MultiPartData.h"

namespace HttpWebServer
{
	namespace DetailClasses
	{
		/// <summary>Form data from web request.</summary>
		public ref class FormData sealed
		{
		public:
			/// <summary>Name of the formdata.</summary>
			property Platform::String ^Name
			{
				Platform::String^ get()
				{
					return name;
				}
			}

			/// <summary>Value for form data.</summary>
			property Platform::String ^Value
			{
				Platform::String^ get()
				{
					return value;
				}
			}

		internal:
			FormData(char *formDataText);
			FormData(HelpClasses::MultiPartData &multiPart);

		private:
			Platform::String ^name;
			Platform::String ^value;
		};
	}
}