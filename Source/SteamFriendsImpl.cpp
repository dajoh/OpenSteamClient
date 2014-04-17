#include "SteamFriendsImpl.hpp"
#include "Messages/MsgProto.hpp"
#include "Messages/MsgClientChatMsg.hpp"
#include "Messages/MsgClientJoinChat.hpp"
#include "Messages/MsgClientChatEnter.hpp"
#include "Messages/MsgClientChatMemberInfo.hpp"
#include "Protobufs/steammessages_clientserver.pb.h"

namespace Sc
{
	// ------------------------------------------------------------------------
	// SteamFriends::Impl Implementation
	// ------------------------------------------------------------------------

	SteamFriends::Impl::Impl(SteamFriends *parent, SteamClient::Impl *client) : SteamBaseImpl(client)
	{
		Persona persona;
		persona.name = "[unknown]";
		persona.state = EPersonaState_Offline;
		persona.relationship = EFriendRelationship_None;
		m_personas[SteamId(0)] = persona;

		m_parent = parent;
	}

	void SteamFriends::Impl::SetPersonaName(const string &name)
	{
		OutputStream stream;
		MsgProto<CMsgClientChangeStatus> msg;

		msg.header.msg = EMsg_ClientChangeStatus;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);
		msg.proto.set_player_name(name);

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::SetPersonaState(EPersonaState state)
	{
		OutputStream stream;
		MsgProto<CMsgClientChangeStatus> msg;

		msg.header.msg = EMsg_ClientChangeStatus;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);
		msg.proto.set_persona_state(state);

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::SetActiveSteamGame(uint32_t appId)
	{
		OutputStream stream;
		MsgProto<CMsgClientGamesPlayed> msg;

		msg.header.msg = EMsg_ClientGamesPlayed;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);

		if(appId != 0)
		{
			auto game = msg.proto.add_games_played();
			game->set_game_id(appId);
		}

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::SetActiveNonSteamGame(const string &name)
	{
		OutputStream stream;
		MsgProto<CMsgClientGamesPlayed> msg;

		msg.header.msg = EMsg_ClientGamesPlayed;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);

		if(name != "")
		{
			auto game = msg.proto.add_games_played();
			game->set_game_id(11409151915401936896ull);
			game->set_game_extra_info(name);
		}

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	const string &SteamFriends::Impl::GetPersonaName() const
	{
		auto it = m_personas.find(m_steamId);
		if(it != m_personas.end())
		{
			return it->second.name;
		}

		return m_personas.find(0)->second.name;
	}

	EPersonaState SteamFriends::Impl::GetPersonaState() const
	{
		auto it = m_personas.find(m_steamId);
		if(it != m_personas.end())
		{
			return it->second.state;
		}

		return m_personas.find(0)->second.state;
	}

	const Persona &SteamFriends::Impl::GetPersonaInfo(SteamId id) const
	{
		auto it = m_personas.find(id);
		if(it != m_personas.end())
		{
			return it->second;
		}

		return m_personas.find(0)->second;
	}

	int SteamFriends::Impl::GetGroupCount() const
	{
		return m_groups.size();
	}

	int SteamFriends::Impl::GetFriendCount() const
	{
		return m_friends.size();
	}

	SteamId SteamFriends::Impl::GetGroupSteamId(int index) const
	{
		return m_groups[index];
	}

	SteamId SteamFriends::Impl::GetFriendSteamId(int index) const
	{
		return m_friends[index];
	}

	void SteamFriends::Impl::JoinChat(SteamId id)
	{
		OutputStream stream;
		MsgClientJoinChat msg;

		msg.header.msg = EMsg_ClientJoinChat;
		msg.header.steamId = m_steamId;
		msg.header.sessionId = m_sessionId;
		msg.chatId = FixChatId(id);

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::LeaveChat(SteamId id)
	{
		OutputStream stream;
		MsgClientChatMemberInfo msg;

		msg.header.msg = EMsg_ClientChatMemberInfo;
		msg.header.steamId = m_steamId;
		msg.header.sessionId = m_sessionId;
		msg.chatId = FixChatId(id);
		msg.infoType = EChatInfoType_StateChange;
		msg.stateChange.targetId = m_steamId;
		msg.stateChange.stateChange = EChatMemberStateChange_Left;
		msg.stateChange.sourceId = m_steamId;

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::SendChatMessage(SteamId chatId, const string &msg)
	{
		OutputStream stream;
		MsgClientChatMsg netMsg;

		netMsg.header.msg = EMsg_ClientChatMsg;
		netMsg.header.steamId = m_steamId;
		netMsg.header.sessionId = m_sessionId;

		netMsg.chatterId = m_steamId;
		netMsg.chatId = FixChatId(chatId);
		netMsg.type = EChatEntryType_ChatMsg;
		netMsg.msg = msg;

		netMsg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::SendFriendMessage(SteamId friendId, const string &msg)
	{
		OutputStream stream;
		MsgProto<CMsgClientFriendMsg> netMsg;

		netMsg.header.msg = EMsg_ClientFriendMsg;
		netMsg.header.proto.set_steamid(m_steamId);
		netMsg.header.proto.set_client_sessionid(m_sessionId);

		netMsg.proto.set_steamid(friendId);
		netMsg.proto.set_chat_entry_type(EChatEntryType_ChatMsg);
		netMsg.proto.set_message(msg);

		netMsg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::RequestPersonas()
	{
		OutputStream stream;
		MsgProto<CMsgClientRequestFriendData> msg;

		msg.header.msg = EMsg_ClientRequestFriendData;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);
		msg.proto.set_persona_state_requested(0xFFFFFFFF);

		for(size_t i = 0; i < m_friends.size(); i++)
		{
			msg.proto.add_friends(m_friends[i]);
		}

		for(size_t i = 0; i < m_groups.size(); i++)
		{
			msg.proto.add_friends(m_groups[i]);
		}

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamFriends::Impl::HandleMessage(const MessageEvent &ev)
	{
		InputStream stream(ev.data);
		SteamBaseImpl::HandleMessage(ev);

		switch(ev.msg)
		{
		case EMsg_ClientChatMsg:
			HandleChatMessage(stream);
			break;
		case EMsg_ClientChatEnter:
			HandleChatEnter(stream);
			break;
		case EMsg_ClientFriendsList:
			HandleFriendsList(stream);
			break;
		case EMsg_ClientPersonaState:
			HandlePersonaState(stream);
			break;
		case EMsg_ClientChatMemberInfo:
			HandleChatChange(stream);
			break;
		case EMsg_ClientFriendMsgIncoming:
			HandleFriendMessage(stream);
			break;
		}
	}

	void SteamFriends::Impl::HandleDisconnect(const DisconnectEvent &ev)
	{
		SteamBaseImpl::HandleDisconnect(ev);

		m_name = "[unknown]";
		m_state = EPersonaState_Offline;
	}

	void SteamFriends::Impl::HandleChatEnter(InputStream &stream)
	{
		ChatEnterEvent ev;
		MsgClientChatEnter msg(stream);

		ev.chatName = msg.chatName;
		ev.chatId = msg.chatId;
		ev.clanId = msg.clanId;
		ev.ownerId = msg.ownerId;
		ev.friendId = msg.friendId;
		ev.chatFlags = msg.chatFlags;
		ev.chatType = msg.chatType;
		ev.response = msg.response;

		for(auto it = msg.memberInfo.begin(); it != msg.memberInfo.end(); ++it)
		{
			auto kv = *it;
			auto id = kv["steamid"].GetUint64();
			ev.memberChatPerms[id] = kv["permissions"].GetInt32();
			ev.memberClanPerms[id] = kv["Details"].GetInt32();
		}

		m_client->Delay([=]()
		{
			m_parent->OnChatEnter->Call(ev);
		});
	}

	void SteamFriends::Impl::HandleChatChange(InputStream &stream)
	{
		MsgClientChatMemberInfo msg(stream);

		if(msg.infoType == EChatInfoType_StateChange)
		{
			ChatMemberStateChangeEvent ev;
			ev.targetId = msg.stateChange.targetId;
			ev.sourceId = msg.stateChange.sourceId;
			ev.stateChange = msg.stateChange.stateChange;

			if(msg.stateChange.stateChange == EChatMemberStateChange_Entered)
			{
				ev.memberChatPerms = msg.stateChange.memberInfo["permissions"].GetInt32();
				ev.memberClanPerms = msg.stateChange.memberInfo["Details"].GetInt32();
			}
			else
			{
				ev.memberChatPerms = 0;
				ev.memberClanPerms = 0;
			}

			m_client->Delay([=]()
			{
				m_parent->OnChatMemberStateChange->Call(ev);
			});
		}
		else if(msg.infoType == EChatInfoType_InfoUpdate)
		{
			ChatMemberInfoUpdateEvent ev;
			ev.memberId = msg.infoUpdate.memberInfo["steamid"].GetUint64();
			ev.memberChatPerms = msg.infoUpdate.memberInfo["permissions"].GetInt32();
			ev.memberClanPerms = msg.infoUpdate.memberInfo["Details"].GetInt32();

			m_client->Delay([=]()
			{
				m_parent->OnChatMemberInfoUpdate->Call(ev);
			});
		}
	}

	void SteamFriends::Impl::HandleChatMessage(InputStream &stream)
	{
		ChatMessageEvent ev;
		MsgClientChatMsg msg(stream);

		ev.msg = msg.msg;
		ev.type = msg.type;
		ev.chatId = msg.chatId;
		ev.chatterId = msg.chatterId;

		m_client->Delay([=]()
		{
			m_parent->OnChatMessage->Call(ev);
		});
	}

	void SteamFriends::Impl::HandleFriendsList(InputStream &stream)
	{
		FriendsListChangedEvent ev;
		MsgProto<CMsgClientFriendsList> msg(stream);

		for(int i = 0; i < msg.proto.friends_size(); i++)
		{
			auto friendData = msg.proto.friends(i);
			auto friendId = SteamId(friendData.ulfriendid());
			auto iterator = m_personas.find(friendId);

			for(auto it = m_groups.begin(); it != m_groups.end(); ++it)
			{
				if(*it == friendId)
				{
					m_groups.erase(it);
					break;
				}
			}

			for(auto it = m_friends.begin(); it != m_friends.end(); ++it)
			{
				if(*it == friendId)
				{
					m_friends.erase(it);
					break;
				}
			}

			if(friendData.efriendrelationship() != EFriendRelationship_None)
			{
				if(friendId.GetType() != EAccountType_Individual)
				{
					m_groups.push_back(friendId);
				}
				else
				{
					m_friends.push_back(friendId);
				}
			}

			if(iterator != m_personas.end())
			{
				iterator->second.relationship = (EFriendRelationship)friendData.efriendrelationship();
			}
			else
			{
				Persona persona;
				persona.name = "[unknown]";
				persona.state = EPersonaState_Offline;
				persona.relationship = (EFriendRelationship)friendData.efriendrelationship();
				m_personas[friendId] = persona;
			}
		}
		
		m_client->Delay([=]()
		{
			m_parent->OnFriendsListChanged->Call(ev);
		});

		RequestPersonas();
	}

	void SteamFriends::Impl::HandlePersonaState(InputStream &stream)
	{
		MsgProto<CMsgClientPersonaState> msg(stream);

		for(int i = 0; i < msg.proto.friends_size(); i++)
		{
			auto friendData = msg.proto.friends(i);
			auto friendId = friendData.friendid();
			auto iterator = m_personas.find(friendId);

			if(m_steamId == friendId)
			{
				m_name = friendData.player_name();
				m_state = (EPersonaState)friendData.persona_state();
			}

			if(iterator != m_personas.end())
			{
				iterator->second.name = friendData.player_name();
				iterator->second.state = (EPersonaState)friendData.persona_state();
				iterator->second.gameName = friendData.game_name();
				iterator->second.avatarHash = friendData.avatar_hash();
			}
			else
			{
				Persona persona;
				persona.name = friendData.player_name();
				persona.state = (EPersonaState)friendData.persona_state();
				persona.gameName = friendData.game_name();
				persona.avatarHash = friendData.avatar_hash();
				persona.relationship = EFriendRelationship_None;
				m_personas[friendId] = persona;
			}

			m_client->Delay([=]()
			{
				PersonaUpdateEvent ev;
				ev.id = friendId;
				m_parent->OnPersonaUpdate->Call(ev);
			});
		}
	}

	void SteamFriends::Impl::HandleFriendMessage(InputStream &stream)
	{
		FriendMessageEvent ev;
		MsgProto<CMsgClientFriendMsgIncoming> msg(stream);

		ev.msg = msg.proto.message();
		ev.type = (EChatEntryType)msg.proto.chat_entry_type();
		ev.friendId = msg.proto.steamid_from();

		// Remove null terminator.
		ev.msg.pop_back();

		m_client->Delay([=]()
		{
			m_parent->OnFriendMessage->Call(ev);
		});
	}

	SteamId SteamFriends::Impl::FixChatId(SteamId id) const
	{
		if(id.GetType() == EAccountType_Clan)
		{
			// TODO: Enum these instances.
			id.SetInstance((0x000FFFFF + 1) >> 1);
			id.SetType(EAccountType_Chat);
		}

		return id;
	}

	// ------------------------------------------------------------------------
	// SteamFriends Implementation
	// ------------------------------------------------------------------------

	SteamFriends::SteamFriends(SteamClient &client)
	{
		impl = new Impl(this, client.impl);
		OnChatEnter = new Event<ChatEnterEvent>();
		OnChatMessage = new Event<ChatMessageEvent>();
		OnFriendMessage = new Event<FriendMessageEvent>();
		OnPersonaUpdate = new Event<PersonaUpdateEvent>();
		OnFriendsListChanged = new Event<FriendsListChangedEvent>();
		OnChatMemberInfoUpdate = new Event<ChatMemberInfoUpdateEvent>();
		OnChatMemberStateChange = new Event<ChatMemberStateChangeEvent>();
	}

	SteamFriends::~SteamFriends()
	{
		delete OnChatMemberStateChange;
		delete OnChatMemberInfoUpdate;
		delete OnFriendsListChanged;
		delete OnPersonaUpdate;
		delete OnFriendMessage;
		delete OnChatMessage;
		delete OnChatEnter;
		delete impl;
	}

	void SteamFriends::SetPersonaName(const string &name)
	{
		impl->SetPersonaName(name);
	}

	void SteamFriends::SetPersonaState(EPersonaState state)
	{
		impl->SetPersonaState(state);
	}

	void SteamFriends::SetActiveSteamGame(uint32_t appId)
	{
		impl->SetActiveSteamGame(appId);
	}

	void SteamFriends::SetActiveNonSteamGame(const string &name)
	{
		impl->SetActiveNonSteamGame(name);
	}

	const string &SteamFriends::GetPersonaName() const
	{
		return impl->GetPersonaName();
	}

	EPersonaState SteamFriends::GetPersonaState() const
	{
		return impl->GetPersonaState();
	}

	const Persona &SteamFriends::GetPersonaInfo(SteamId id) const
	{
		return impl->GetPersonaInfo(id);
	}

	int SteamFriends::GetGroupCount() const
	{
		return impl->GetGroupCount();
	}

	int SteamFriends::GetFriendCount() const
	{
		return impl->GetFriendCount();
	}

	SteamId SteamFriends::GetGroupSteamId(int index) const
	{
		return impl->GetGroupSteamId(index);
	}

	SteamId SteamFriends::GetFriendSteamId(int index) const
	{
		return impl->GetFriendSteamId(index);
	}

	void SteamFriends::JoinChat(SteamId id)
	{
		impl->JoinChat(id);
	}

	void SteamFriends::LeaveChat(SteamId id)
	{
		impl->LeaveChat(id);
	}

	void SteamFriends::SendChatMessage(SteamId chatId, const string &msg)
	{
		impl->SendChatMessage(chatId, msg);
	}

	void SteamFriends::SendFriendMessage(SteamId friendId, const string &msg)
	{
		impl->SendFriendMessage(friendId, msg);
	}
}
