#pragma once

namespace HttpWebServer
{
	namespace DetailClasses
	{
		/// <summary>Authentication credential for web request.</summary>
		public ref class HttpCredential sealed
		{
		public:
			/// <summary>Username for authentication.</summary>
			property Platform::String ^Username
			{
				Platform::String^ get()
				{
					return username;
				}
			}

			/// <summary>Password for authentication.</summary>
			property Platform::String ^Password
			{
				Platform::String^ get()
				{
					return password;
				}
			}

		internal:
			HttpCredential(char *credentialText);

		private:
			int Base64Decode(unsigned char *utTekst, const unsigned char *innTekst, int innLengde);
			Platform::String ^username;
			Platform::String ^password;
		};
	}
}