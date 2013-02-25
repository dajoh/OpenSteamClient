#ifndef STEAMBASEIMPL_HPP
#define STEAMBASEIMPL_HPP

#include <OpenSteamClient/SteamId.hpp>
#include "SteamClientImpl.hpp"

namespace Sc
{
	class SteamBaseImpl
	{
	public:
		SteamBaseImpl(SteamClient::Impl *client);
	protected:
		virtual void HandleMessage(const MessageEvent &ev);
		virtual void HandleDisconnect(const DisconnectEvent &ev);

		virtual void HandleLogin(InputStream &stream);
		virtual void HandleLogout(InputStream &stream);

		bool m_loggedIn;
		SteamId m_steamId;
		uint32_t m_sessionId;
		SteamClient::Impl *m_client;
	};
}

#endif