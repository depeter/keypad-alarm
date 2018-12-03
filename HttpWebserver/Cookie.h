#pragma once

namespace HttpWebServer
{
	/// <summary>Cookie for http request and response.</summary>
	public ref class Cookie sealed
	{
	public:
		/// <summary>Sets cookie name and value. Lifetime of this cookie is to the browser is closed because of no expiration date.</summary>
		Cookie(Platform::String ^name, Platform::String ^value);

		/// <summary>Sets cookie name and value. Lifetime of this cookie is determined by the time set in the expires.</summary>
		Cookie(Platform::String ^name, Platform::String ^value, Windows::Foundation::DateTime expires);

		/// <summary>Sets cookie name, value, domain and path. Lifetime of this cookie is determined by the time set in the expires. Path of domain can be empty string. Client sends cookie only when path and domain votes by request. For example path ="/Account" will cookie only sent when the address is /Account.</summary>
		Cookie(Platform::String ^name, Platform::String ^value, Platform::String ^domain, Platform::String ^path);

		/// <summary>Sets cookie name, value, domain and path. Lifetime of this cookie is to the browser is closed because of no expiration date. Path of domain can be empty string. Client sends cookie only when path and domain votes by request. For example path ="/Account" will cookie only sent when the address is /Account.</summary>
		Cookie(Platform::String ^name, Platform::String ^value, Platform::String ^domain, Platform::String ^path, Windows::Foundation::DateTime expires);

		/// <summary>Name of the cookie.</summary>
		property Platform::String ^Name
		{
			Platform::String^ get()
			{
				return name;
			}
		}

		/// <summary>Value for cookie.</summary>
		property Platform::String ^Value
		{
			Platform::String^ get()
			{
				return value;
			}
		}

	internal:
		Cookie(char *cookieRequestText);
		std::string SetText;

	private:
		Platform::String ^name;
		Platform::String ^value;
	};
}