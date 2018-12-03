#pragma once

namespace HttpWebServer
{
	namespace HelpClasses
	{
		class MultiPartData
		{
		public:
			MultiPartData(char *content, int lengde); // Data del fra multipart
			MultiPartData(char *content, char *contentType, int contentLength); // Brukes når fil legges til direkte fra content. Ikke standard i weblesere, men hvis text/plain i form brukes kommer den her.
			char *Name = nullptr;
			char *Filename = nullptr;
			char *ContentType = nullptr;
			int ContentLength = 0;
			char *Content = nullptr;
			bool IsFile = false; // Er true hvis multipart er fil og false hvis multipart kun er vanlig form data
		};
	}
}