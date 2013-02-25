#ifndef OPENSTEAMCLIENT_STEAMFRIENDS_HPP
#define OPENSTEAMCLIENT_STEAMFRIENDS_HPP

#include <list>
#include <string>
#include <unordered_map>
#include "SteamId.hpp"
#include "SteamClient.hpp"

namespace Sc
{
	using std::list;
	using std::string;
	using std::unordered_map;

	struct Persona
	{
		string name;
		string gameName;
		string avatarHash;
		EPersonaState state;
		EFriendRelationship relationship;
	};

	struct ChatEnterEvent
	{
		string chatName;
		SteamId chatId;
		SteamId clanId;
		SteamId ownerId;
		SteamId friendId;
		uint32_t chatFlags;
		EChatRoomType chatType;
		EChatRoomEnterResponse response;
		unordered_map<SteamId, uint32_t> memberChatPerms;
		unordered_map<SteamId, uint32_t> memberClanPerms;
	};

	struct ChatMessageEvent
	{
		string msg;
		SteamId chatId;
		SteamId chatterId;
		EChatEntryType type;
	};

	struct FriendMessageEvent
	{
		string msg;
		SteamId friendId;
		EChatEntryType type;
	};

	struct PersonaUpdateEvent
	{
		SteamId id;
	};

	struct FriendsListChangedEvent
	{
	};

	struct ChatMemberInfoUpdateEvent
	{
		SteamId memberId;
		uint32_t memberChatPerms;
		uint32_t memberClanPerms;
	};

	struct ChatMemberStateChangeEvent
	{
		SteamId targetId;
		SteamId sourceId;
		uint32_t stateChange;
		uint32_t memberChatPerms;
		uint32_t memberClanPerms;
	};

	class OPENSTEAMCLIENT_EXPORT SteamFriends
	{
	public:
		SteamFriends(SteamClient &client);
		~SteamFriends();

		void SetPersonaName(const string &name);
		void SetPersonaState(EPersonaState state);
		void SetPersonaGameName(const string &name);

		const string &GetPersonaName() const;
		EPersonaState GetPersonaState() const;
		const Persona &GetPersonaInfo(SteamId id) const;

		int GetGroupCount() const;
		int GetFriendCount() const;
		SteamId GetGroupSteamId(int index) const;
		SteamId GetFriendSteamId(int index) const;

		void JoinChat(SteamId id);
		void LeaveChat(SteamId id);

		void SendChatMessage(SteamId chatId, const string &msg);
		void SendFriendMessage(SteamId friendId, const string &msg);

		Event<ChatEnterEvent> *OnChatEnter;
		Event<ChatMessageEvent> *OnChatMessage;
		Event<FriendMessageEvent> *OnFriendMessage;
		Event<PersonaUpdateEvent> *OnPersonaUpdate;
		Event<FriendsListChangedEvent> *OnFriendsListChanged;
		Event<ChatMemberInfoUpdateEvent> *OnChatMemberInfoUpdate;
		Event<ChatMemberStateChangeEvent> *OnChatMemberStateChange;
	private:
		class Impl;
		Impl *impl;

		SteamFriends(const SteamFriends &);
		SteamFriends &operator=(const SteamFriends &);
	};
}

#endif