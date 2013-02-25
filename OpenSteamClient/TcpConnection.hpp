#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include <string>
#include <OpenSteamClient/Event.hpp>
#include <OpenSteamClient/NonCopyable.hpp>

namespace Sc
{
	using std::string;
	
	class TcpConnection : NonCopyable
	{
	public:
		TcpConnection();
		~TcpConnection();

		void Connect(const string &ip, const string &port);
		void Disconnect();
		bool IsConnected() const;

		void Run(int timeout);
		void Send(const string &data);

		Event<bool> OnConnect;
		Event<void> OnDisconnect;
		Event<string> OnReceive;
	private:
		class Impl;
		Impl *impl;
	};
}

#endif