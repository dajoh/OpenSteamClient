#ifndef MESSAGES_MSGCHANNELENCRYPTRESPONSE_HPP
#define MESSAGES_MSGCHANNELENCRYPTRESPONSE_HPP

#include "MsgHdr.hpp"

namespace Sc
{
	class MsgChannelEncryptResponse
	{
	public:
		MsgChannelEncryptResponse();
		MsgChannelEncryptResponse(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdr header;
		uint32_t protocolVersion;
		string key;
		uint32_t keyCrc;
		uint32_t unknown;
	};

	// ------------------------------------------------------------------------
	// MsgChannelEncryptResponse Implementation
	// ------------------------------------------------------------------------

	inline MsgChannelEncryptResponse::MsgChannelEncryptResponse()
	{
		protocolVersion = 1;
		keyCrc = 0;
		unknown = 0;
	}

	inline MsgChannelEncryptResponse::MsgChannelEncryptResponse(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgChannelEncryptResponse::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint32_t>(protocolVersion);
		stream.Write<uint32_t>(key.size());
		stream.Write<string>(key);
		stream.Write<uint32_t>(keyCrc);
		stream.Write<uint32_t>(unknown);
	}

	inline void MsgChannelEncryptResponse::Deserialize(InputStream &stream)
	{
		uint32_t keySize;
		header.Deserialize(stream);
		stream.Read<uint32_t>(protocolVersion);
		stream.Read<uint32_t>(keySize);
		stream.Read<string>(key, keySize);
		stream.Read<uint32_t>(keyCrc);
		stream.Read<uint32_t>(unknown);
	}
}

#endif