OpenSteamClient
===============

An open source C++ library to interface with the Steam network.

Example Code
------------

Here's how you connect to the Steam network with OpenSteamClient and set your friend status to online.

```cpp
auto client  = Sc::SteamClient();
auto user    = Sc::SteamUser(client);
auto friends = Sc::SteamFriends(client);

client.OnConnect.Add([&](Sc::ConnectEvent ev)
{
    if(ev.result == Sc::EResult_OK)
	{
		Sc::LoginDetails details;
		details.username = "";
		details.password = "";
		user.LogIn(details);
	}
	else
	{
		// Couldn't connect.
	}
});

client.OnDisconnect.Add([&](Sc::DisconnectEvent ev)
{
	// Disconnected.
});

user.OnLoggedIn.Add([&](Sc::LoggedInEvent ev)
{
	if(ev.result == Sc::EResult_OK)
	{
		friends.SetPersonaName("dajoh");
		friends.SetPersonaState(Sc::EPersonaState_Online);
	}
	else
	{
		// Couldn't log in.
	}
});

// Start connecting.
client.Connect();

while(client.IsConnected())
{
	// -1 is the timeout in ms for waiting for packets.
	// -1 means infinite.
	client.Run(-1);
}
```

Make sure to catch any exceptions thrown by SteamClient::Run(), if an exception is thrown you have to recreate the SteamClient object (and SteamFriends, etc) and connect again.

Special Thanks
--------------

Special thanks to everybody who contributed to SteamKit2, without it this project would never have been created.