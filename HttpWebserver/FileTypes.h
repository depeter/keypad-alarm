#pragma once

namespace HttpWebServer
{
	/// <summary>Enum for setting file type sent.</summary>
	public enum class FileTypes
	{
		/// <summary>html document (text/html; charset=utf-8)</summary>
		text_html,
		/// <summary>CSS stylesheet (text/css; charset=utf-8)</summary>
		text_css,
		/// <summary>xml document (text/xml; charset=utf-8)</summary>
		text_xml,
		/// <summary>Plain text (text/plain; charset=utf-8)</summary>
		text_plain,
		/// <summary>javascript file (application/javascript; charset=utf-8)</summary>
		text_javascript,
		/// <summary>json document (application/json; charset=utf-8)</summary>
		text_json,
		/// <summary>soap document (application/soap+xml; charset=utf-8)</summary>
		text_soap,
		/// <summary>svg image (image/svg+xml; charset=utf-8)</summary>
		image_svg,
		/// <summary>jpg image (image/jpeg)</summary>
		image_jpg,
		/// <summary>png image (image/png)</summary>
		image_png,
		/// <summary>gif image (image/gif)</summary>
		image_gif,
		/// <summary>icon (image/x-icon)</summary>
		image_ico,
		/// <summary>mp4 video (video/mp4)</summary>
		vidoe_mp4,
		/// <summary>webm video (video/webm)</summary>
		video_webm,
		/// <summary>mp3 audio (audio/mpeg)</summary>
		audio_mp3,
		/// <summary>wav audio (audio/wav)</summary>
		audio_wav,
		/// <summary>pdf document (application/pdf)</summary>
		pdf,
		/// <summary>zip file (application/zip)</summary>
		zip,
		/// <summary>xps document (application/vnd.ms-xpsdocument)</summary>
		xps,
		/// <summary>excel document (application/vnd.openxmlformats-officedocument.spreadsheetml.sheet)</summary>
		xlsx,
		/// <summary>word document (application/vnd.openxmlformats-officedocument.wordprocessingml.document)</summary>
		docx,
		/// <summary>Binary file (application/octet-stream)</summary>
		bin
	};
}