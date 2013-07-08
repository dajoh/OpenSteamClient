#ifndef OPENSTEAMCLIENT_STEAMUSER_HPP
#define OPENSTEAMCLIENT_STEAMUSER_HPP

#include <string>
#include "SteamId.hpp"
#include "SteamClient.hpp"

namespace Sc
{
	using std::string;

	struct LoginDetails
	{
		string username;
		string password;
		string authCode;
	};
	
	struct LoggedInEvent
	{
		EResult result;
		SteamId steamId;
	};

	struct LoggedOutEvent
	{
		EResult result;
	};

	class OPENSTEAMCLIENT_EXPORT SteamUser
	{
	public:
		SteamUser(SteamClient &client);
		~SteamUser();

		void LogIn(LoginDetails details);
		void LogOut();
		bool IsLoggedIn() const;

		Event<LoggedInEvent> *OnLoggedIn;
		Event<LoggedOutEvent> *OnLoggedOut;
	private:
		class Impl;
		Impl *impl;

		SteamUser(const SteamUser &);
		SteamUser &operator=(const SteamUser &);
	};
}

#endif