#ifndef MESSAGES_MSGCLIENTCHATMEMBERINFO_HPP
#define MESSAGES_MSGCLIENTCHATMEMBERINFO_HPP

#include <OpenSteamClient/Exception.hpp>
#include "MsgHdrExt.hpp"
#include "../KeyValues.hpp"

namespace Sc
{
	class MsgClientChatMemberInfo
	{
	public:
		MsgClientChatMemberInfo();
		MsgClientChatMemberInfo(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdrExt header;
		SteamId chatId;
		EChatInfoType infoType;

		struct
		{
			SteamId targetId;
			uint32_t stateChange;
			SteamId sourceId;
			KeyValues memberInfo;
		} stateChange;

		struct
		{
			KeyValues memberInfo;
		} infoUpdate;
	};

	// ------------------------------------------------------------------------
	// MsgClientChatMemberInfo Implementation
	// ------------------------------------------------------------------------

	inline MsgClientChatMemberInfo::MsgClientChatMemberInfo()
	{
		infoType = EChatInfoType_StateChange;
		stateChange.stateChange = 0;
	}

	inline MsgClientChatMemberInfo::MsgClientChatMemberInfo(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgClientChatMemberInfo::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint64_t>(chatId);
		stream.Write<uint32_t>(infoType);

		if(infoType == EChatInfoType_StateChange)
		{
			stream.Write<uint64_t>(stateChange.targetId);
			stream.Write<uint32_t>(stateChange.stateChange);
			stream.Write<uint64_t>(stateChange.sourceId);

			if(stateChange.stateChange == EChatMemberStateChange_Entered)
			{
				throw Exception("Not implemented.");
			}
		}
		else if(infoType == EChatInfoType_InfoUpdate)
		{
			throw Exception("Not implemented.");
		}
	}

	inline void MsgClientChatMemberInfo::Deserialize(InputStream &stream)
	{
		uint32_t infoTypeTemp;

		header.Deserialize(stream);
		stream.Read<uint64_t>(chatId);
		stream.Read<uint32_t>(infoTypeTemp);

		if(infoTypeTemp == EChatInfoType_StateChange)
		{
			stream.Read<uint64_t>(stateChange.targetId);
			stream.Read<uint32_t>(stateChange.stateChange);
			stream.Read<uint64_t>(stateChange.sourceId);

			if(stateChange.stateChange == EChatMemberStateChange_Entered)
			{
				stateChange.memberInfo = KeyValues(stream);
			}
		}
		else if(infoTypeTemp == EChatInfoType_InfoUpdate)
		{
			infoUpdate.memberInfo = KeyValues(stream);
		}

		infoType = (EChatInfoType)infoTypeTemp;
	}
}

#endif