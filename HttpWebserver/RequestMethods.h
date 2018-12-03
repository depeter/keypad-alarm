#pragma once

#undef DELETE // DELETE lager konflikt, forsøker å undefinere den og se om det går bra uten

namespace HttpWebServer
{
	/// <summary>Enum to see request command.</summary>
	public enum class RequestMethods
	{
		/// <summary>GET or HEAD method is received. Requests using GET should only retrieve data and should have no other effect. GET is therefore a secure method for client. The HEAD method asks for a response identical to that of a GET request, but without the response body. HEAD and GET is therefore merged. Web Server omits response body if the method was originally HEAD.</summary>
		GET,
		/// <summary>POST method is used to update and store data. POST is therefore an unsafe method for client.</summary>
		POST,
		/// <summary>Not used from browsers. PUT method is used to store data. PUT is therefore an unsafe method for client.</summary>
		PUT,
		/// <summary>Not used from browsers. DELETE method is used to delete data. DELETE is therefore an unsafe method for client.</summary>
		DELETE
	};
}