#ifndef MESSAGES_MSGCHANNELENCRYPTREQUEST_HPP
#define MESSAGES_MSGCHANNELENCRYPTREQUEST_HPP

#include "MsgHdr.hpp"

namespace Sc
{
	class MsgChannelEncryptRequest
	{
	public:
		MsgChannelEncryptRequest();
		MsgChannelEncryptRequest(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdr header;
		uint32_t protocolVersion;
		EUniverse universe;
	};

	// ------------------------------------------------------------------------
	// MsgChannelEncryptRequest Implementation
	// ------------------------------------------------------------------------

	inline MsgChannelEncryptRequest::MsgChannelEncryptRequest()
	{
		protocolVersion = 1;
		universe = EUniverse_Invalid;
	}

	inline MsgChannelEncryptRequest::MsgChannelEncryptRequest(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgChannelEncryptRequest::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint32_t>(protocolVersion);
		stream.Write<uint32_t>(universe);
	}

	inline void MsgChannelEncryptRequest::Deserialize(InputStream &stream)
	{
		uint32_t universeTemp;
		header.Deserialize(stream);
		stream.Read<uint32_t>(protocolVersion);
		stream.Read<uint32_t>(universeTemp);
		universe = (EUniverse)universeTemp;
	}
}

#endif