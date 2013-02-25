#ifndef STEAMFRIENDSIMPL_HPP
#define STEAMFRIENDSIMPL_HPP

#include <vector>
#include <OpenSteamClient/SteamFriends.hpp>
#include "SteamBaseImpl.hpp"

namespace Sc
{
	using std::vector;

	class SteamFriends::Impl : public SteamBaseImpl
	{
	public:
		Impl(SteamFriends *parent, SteamClient::Impl *client);

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
	protected:
		void RequestPersonas();

		virtual void HandleMessage(const MessageEvent &ev);
		virtual void HandleDisconnect(const DisconnectEvent &ev);

		void HandleChatEnter(InputStream &stream);
		void HandleChatChange(InputStream &stream);
		void HandleChatMessage(InputStream &stream);
		void HandleFriendsList(InputStream &stream);
		void HandlePersonaState(InputStream &stream);
		void HandleFriendMessage(InputStream &stream);
		
		SteamId FixChatId(SteamId id) const;

		string m_name;
		EPersonaState m_state;
		SteamFriends *m_parent;
		vector<SteamId> m_groups;
		vector<SteamId> m_friends;
		unordered_map<SteamId, Persona> m_personas;
	};
}

#endif