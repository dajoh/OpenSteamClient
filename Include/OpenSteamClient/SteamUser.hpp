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

	class SteamUser : NonCopyable
	{
	public:
		SteamUser(SteamClient &client);
		~SteamUser();

		void LogIn(LoginDetails details);
		void LogOut();
		bool IsLoggedIn() const;

		Event<LoggedInEvent> OnLoggedIn;
		Event<LoggedOutEvent> OnLoggedOut;
	private:
		class Impl;
		Impl *impl;
	};
}

#endif