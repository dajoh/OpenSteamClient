#ifndef MESSAGES_MSGCHANNELENCRYPTRESULT_HPP
#define MESSAGES_MSGCHANNELENCRYPTRESULT_HPP

#include "MsgHdr.hpp"

namespace Sc
{
	class MsgChannelEncryptResult
	{
	public:
		MsgChannelEncryptResult();
		MsgChannelEncryptResult(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdr header;
		EResult result;
	};

	// ------------------------------------------------------------------------
	// MsgChannelEncryptResult Implementation
	// ------------------------------------------------------------------------

	inline MsgChannelEncryptResult::MsgChannelEncryptResult()
	{
		result = EResult_Invalid;
	}

	inline MsgChannelEncryptResult::MsgChannelEncryptResult(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgChannelEncryptResult::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint32_t>(result);
	}

	inline void MsgChannelEncryptResult::Deserialize(InputStream &stream)
	{
		uint32_t resultTemp;
		header.Deserialize(stream);
		stream.Read<uint32_t>(resultTemp);
		result = (EResult)resultTemp;
	}
}

#endif