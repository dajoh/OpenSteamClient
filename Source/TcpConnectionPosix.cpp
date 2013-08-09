#include <cstdint>
#include <cstring>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <OpenSteamClient/Exception.hpp>
#include "TcpConnection.hpp"

namespace Sc
{
	using std::unique_ptr;

	class TcpConnection::Impl
	{
	public:
		Impl(TcpConnection *parent);
		~Impl();

		void Connect(const string &ip, const string &port);
		void Disconnect();
		bool IsConnected() const;

		void Run(int timeout);
		void Send(const string &data);
	private:
		enum State
		{
			State_Connecting,
			State_Disconnected,
			State_ReadingHeader,
			State_ReadingPayload
		};

		struct Header
		{
			static const uint32_t Magic = 0x31305456; // "VS01"
			static const uint32_t MaxSize = 33554432; // 32 MiB
			uint32_t size;
			uint32_t magic;
		};

		int m_socket;
		State m_state;
		size_t m_msgSize;
		size_t m_msgLeft;
		TcpConnection *m_parent;
		unique_ptr<char[]> m_msgData;
	};

	// ------------------------------------------------------------------------
	// TcpConnection::Impl Implementation
	// ------------------------------------------------------------------------

	TcpConnection::Impl::Impl(TcpConnection *parent)
	{
		m_state = State_Disconnected;
		m_parent = parent;
	}

	TcpConnection::Impl::~Impl()
	{
		if(IsConnected())
		{
			Disconnect();
		}
	}

	void TcpConnection::Impl::Connect(const string &ip, const string &port)
	{
		int flags;
		addrinfo hints;
		addrinfo *result;

		if(IsConnected())
		{
			throw Exception("Attempted to connect while already connected.");
		}

		memset(&hints, 0, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if(getaddrinfo(ip.c_str(), port.c_str(), &hints, &result) != 0)
		{
			throw Exception("getaddrinfo() failed.");
		}

		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if(m_socket == -1)
		{
			freeaddrinfo(result);
			throw Exception("socket() failed.");
		}

		flags = fcntl(m_socket, F_GETFL, 0);
		if(fcntl(m_socket, F_SETFL, flags | O_NONBLOCK) != 0)
		{
			close(m_socket);
			freeaddrinfo(result);
			throw Exception("fcntl() failed.");
		}

		if(connect(m_socket, result->ai_addr, result->ai_addrlen) != 0)
		{
			if(errno != EINPROGRESS)
			{
				close(m_socket);
				freeaddrinfo(result);
				throw Exception("connect() failed.");
			}
		}

		freeaddrinfo(result);

		m_state = State_Connecting;
	}

	void TcpConnection::Impl::Disconnect()
	{
		if(!IsConnected())
		{
			return;
		}

		shutdown(m_socket, SHUT_RDWR);
		close(m_socket);

		m_state = State_Disconnected;
	}

	bool TcpConnection::Impl::IsConnected() const
	{
		return m_state != State_Disconnected;
	}

	void TcpConnection::Impl::Run(int timeout)
	{
		pollfd pollData;
		pollData.fd = m_socket;
		pollData.events = POLLIN;
		pollData.revents = 0;

		if(m_state == State_Connecting)
		{
			pollData.events |= POLLOUT;
		}

		if(!IsConnected())
		{
			return;
		}

		if(poll(&pollData, 1, timeout) > 0)
		{
			bool closed = false;

			do
			{
				if(pollData.revents & POLLOUT)
				{
					pollData.events &= ~POLLOUT;

					m_parent->OnConnect.Call(true);
					m_state = State_ReadingHeader;
					m_msgSize = sizeof(Header);
					m_msgLeft = sizeof(Header);
					m_msgData.reset(new char[sizeof(Header)]);
				}

				if(pollData.revents & POLLHUP)
				{
					closed = true;
				}

				if(pollData.revents & POLLIN)
				{
					auto pos = m_msgSize - m_msgLeft;
					auto buffer = &m_msgData.get()[pos];
					auto received = recv(m_socket, buffer, m_msgLeft, 0);

					if(received <= 0)
					{
						closed = true;
						break;
					}
					else
					{
						m_msgLeft -= received;
					}

					if(m_msgLeft == 0)
					{
						if(m_state == State_ReadingHeader)
						{
							auto header = (Header *)m_msgData.get();

							if(header->size > Header::MaxSize || header->magic != Header::Magic)
							{
								closed = true;
								break;
							}

							m_state = State_ReadingPayload;
							m_msgSize = header->size;
							m_msgLeft = header->size;
							m_msgData.reset(new char[header->size]);
						}
						else if(m_state == State_ReadingPayload)
						{
							m_parent->OnReceive.Call(string(m_msgData.get(), m_msgSize));
							m_state = State_ReadingHeader;
							m_msgSize = sizeof(Header);
							m_msgLeft = sizeof(Header);
							m_msgData.reset(new char[sizeof(Header)]);
						}
					}
				}
			} while(IsConnected() && poll(&pollData, 1, 0) > 0);

			if(closed)
			{
				Disconnect();
				m_parent->OnDisconnect.Call();
			}
		}
	}

	void TcpConnection::Impl::Send(const string &data)
	{
		if(!IsConnected())
		{
			return;
		}

		Header header;
		header.size = data.size();
		header.magic = Header::Magic;

		send(m_socket, (const char *)&header, sizeof(header), 0);
		send(m_socket, data.c_str(), data.size(), 0);
	}

	// ------------------------------------------------------------------------
	// TcpConnection Implementation
	// ------------------------------------------------------------------------

	TcpConnection::TcpConnection()
	{
		impl = new Impl(this);
	}

	TcpConnection::~TcpConnection()
	{
		delete impl;
	}

	void TcpConnection::Connect(const string &ip, const string &port)
	{
		impl->Connect(ip, port);
	}

	void TcpConnection::Disconnect()
	{
		impl->Disconnect();
	}

	bool TcpConnection::IsConnected() const
	{
		return impl->IsConnected();
	}

	void TcpConnection::Run(int timeout)
	{
		impl->Run(timeout);
	}

	void TcpConnection::Send(const string &data)
	{
		impl->Send(data);
	}
}
