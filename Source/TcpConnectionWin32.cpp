#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
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

		State m_state;
		HANDLE m_event;
		SOCKET m_socket;
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
		WSADATA wsaData;

		if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			throw Exception("Couldn't initialize WinSock.");
		}

		m_state = State_Disconnected;
		m_parent = parent;
	}

	TcpConnection::Impl::~Impl()
	{
		if(IsConnected())
		{
			Disconnect();
		}

		WSACleanup();
	}

	void TcpConnection::Impl::Connect(const string &ip, const string &port)
	{
		addrinfo hints;
		addrinfo *result;

		if(IsConnected())
		{
			throw Exception("Attempted to connect while already connected.");
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		if(getaddrinfo(ip.c_str(), port.c_str(), &hints, &result) != 0)
		{
			throw Exception("getaddrinfo() failed.");
		}

		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if(m_socket == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			throw Exception("socket() failed.");
		}

		m_event = WSACreateEvent();
		if(m_event == WSA_INVALID_EVENT)
		{
			closesocket(m_socket);
			freeaddrinfo(result);
			throw Exception("WSACreateEvent() failed.");
		}

		if(WSAEventSelect(m_socket, m_event, FD_READ | FD_CONNECT | FD_CLOSE) != 0)
		{
			WSACloseEvent(m_event);
			closesocket(m_socket);
			freeaddrinfo(result);
			throw Exception("WSAEventSelect() failed.");
		}

		if(connect(m_socket, result->ai_addr, result->ai_addrlen) != 0)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				WSACloseEvent(m_event);
				closesocket(m_socket);
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

		shutdown(m_socket, SD_BOTH);
		WSACloseEvent(m_event);
		closesocket(m_socket);

		m_state = State_Disconnected;
	}

	bool TcpConnection::Impl::IsConnected() const
	{
		return m_state != State_Disconnected;
	}

	void TcpConnection::Impl::Run(int timeout)
	{
		DWORD time = (timeout < 0) ? INFINITE : timeout;
		
		if(!IsConnected())
		{
			return;
		}

		if(WaitForSingleObject(m_event, time) == WAIT_OBJECT_0)
		{
			BOOL closed = FALSE;
			WSANETWORKEVENTS events;

			do
			{
				if(WSAEnumNetworkEvents(m_socket, m_event, &events) != 0)
				{
					throw Exception("WSAEnumNetworkEvents() failed.");
				}

				if(events.lNetworkEvents & FD_CONNECT)
				{
					if(events.iErrorCode[FD_CONNECT_BIT] != 0)
					{
						Disconnect();
						m_parent->OnConnect.Call(false);
						break;
					}
					
					m_parent->OnConnect.Call(true);
					m_state = State_ReadingHeader;
					m_msgSize = sizeof(Header);
					m_msgLeft = sizeof(Header);
					m_msgData.reset(new char[sizeof(Header)]);
				}

				if(events.lNetworkEvents & FD_CLOSE)
				{
					closed = TRUE;
				}

				if(events.lNetworkEvents & FD_READ)
				{
					if(events.iErrorCode[FD_READ_BIT] != 0)
					{
						closed = TRUE;
						break;
					}

					auto pos = m_msgSize - m_msgLeft;
					auto buffer = &m_msgData.get()[pos];
					auto received = recv(m_socket, buffer, m_msgLeft, 0);

					if(received < 0)
					{
						closed = TRUE;
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
								closed = TRUE;
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
			} while(IsConnected() && WaitForSingleObject(m_event, 0) == WAIT_OBJECT_0);

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
