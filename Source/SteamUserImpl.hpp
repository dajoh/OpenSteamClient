#ifndef STEAMUSERIMPL_HPP
#define STEAMUSERIMPL_HPP

#include <OpenSteamClient/SteamUser.hpp>
#include "SteamBaseImpl.hpp"

namespace Sc
{
	class SteamUser::Impl : public SteamBaseImpl
	{
	public:
		Impl(SteamUser *parent, SteamClient::Impl *client);
		
		void LogIn(LoginDetails details);
		void LogOut();
		bool IsLoggedIn() const;
	protected:
		virtual void HandleDisconnect(const DisconnectEvent &ev);

		virtual void HandleLogin(InputStream &stream);
		virtual void HandleLogout(InputStream &stream);

		SteamUser *m_parent;
	};
}

#endif