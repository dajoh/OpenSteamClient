#ifndef MESSAGES_MSGHDRPROTO_HPP
#define MESSAGES_MSGHDRPROTO_HPP

#include <OpenSteamClient/Enums.hpp>
#include "../InputStream.hpp"
#include "../OutputStream.hpp"
#include "../Protobufs/steammessages_base.pb.h"

namespace Sc
{
	class MsgHdrProto
	{
	public:
		MsgHdrProto();
		MsgHdrProto(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		EMsg msg;
		CMsgProtoBufHeader proto;
	};

	// ------------------------------------------------------------------------
	// MsgHdrProto Implementation
	// ------------------------------------------------------------------------

	inline MsgHdrProto::MsgHdrProto()
	{
		msg = EMsg_Invalid;
	}

	inline MsgHdrProto::MsgHdrProto(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgHdrProto::Serialize(OutputStream &stream)
	{
		uint32_t protoMsg = msg | 0x80000000;
		uint32_t protoSize = proto.ByteSize();

		stream.Write<uint32_t>(protoMsg);
		stream.Write<uint32_t>(protoSize);
		stream.Write<MessageLite>(proto);
	}

	inline void MsgHdrProto::Deserialize(InputStream &stream)
	{
		uint32_t protoMsg;
		uint32_t protoSize;

		stream.Read<uint32_t>(protoMsg);
		stream.Read<uint32_t>(protoSize);
		stream.Read<MessageLite>(proto, protoSize);

		msg = (EMsg)(protoMsg & ~0x80000000);
	}
}

#endif