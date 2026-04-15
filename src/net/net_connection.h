#pragma once

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"

namespace net
{
	template<typename T>
	class server_interface;

	template<typename T>
	class connection : public std::enable_shared_from_this<connection<T>>
	{
	public:
		enum class owner
		{
			server,
			client
		};

	public:
		connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
			: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
		{
			m_nOwnerType = parent;

			if (m_nOwnerType == owner::server)
			{
				//m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
				// uint64_t tiempo = std::chrono::system_clock::now().time_since_epoch().count();
				// std::cout << "VALORES Server: " << tiempo << "\n";
				m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
				m_nHandshakeCheck = desordenar(m_nHandshakeOut);

				//std::cout << "Handshake Out: " << m_nHandshakeOut << " - Handshake Check: " << m_nHandshakeCheck << "\n";
			}
			else
			{
				// uint64_t tiempo = std::chrono::system_clock::now().time_since_epoch().count();
				// std::cout << "VALORES Client: " << tiempo << "\n";
				m_nHandshakeIn = 0;
				m_nHandshakeOut = 0;
			}
		}

		virtual ~connection()
		{}

		uint32_t GetID() const
		{
			return id;
		}

	public:
		void ConnectToClient(net::server_interface<T>*server, uint32_t uid = 0)
		{
			// uint64_t tiempo = std::chrono::system_clock::now().time_since_epoch().count();
			// std::cout << "Server: " << tiempo << "\n";
			if (m_nOwnerType == owner::server)
			{
				if (m_socket.is_open())
				{
					id = uid;

					WriteValidation();
					ReadValidation(server);
				}
			}
		}

		void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
		{
			// uint64_t tiempo = std::chrono::system_clock::now().time_since_epoch().count();
			// std::cout << "Client: " << tiempo << "\n";
			if (m_nOwnerType == owner::client)
			{
				asio::async_connect(m_socket, endpoints,
					[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
					{
						if (!ec)
						{
							ReadValidation();
						}
					});
			}
		}


		void Disconnect()
		{
			if (IsConnected())
				asio::post(m_asioContext, [this]() { m_socket.close(); });
		}

		bool IsConnected() const
		{
			return m_socket.is_open();
		}

		void StartListening()
		{
			
		}

	public:
		void Send(const message<T>& msg)
		{
			asio::post(m_asioContext,
				[this, msg]()
				{
					bool bWritingMessage = !m_qMessagesOut.empty();
					m_qMessagesOut.push_back(msg);
					if (!bWritingMessage)
					{
						WriteHeader();
					}
				});
		}



	private:
		void WriteHeader()
		{
			asio::async_write(m_socket, asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (m_qMessagesOut.front().body.size() > 0)
						{
							WriteBody();
						}
						else
						{
							m_qMessagesOut.pop_front();

							if (!m_qMessagesOut.empty())
							{
								WriteHeader();
							}
						}
					}
					else
					{
						std::cout << "[" << id << "] Write Header Fail.\n";
						m_socket.close();
					}
				});
		}

		void WriteBody()
		{
			asio::async_write(m_socket, asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						m_qMessagesOut.pop_front();

						if (!m_qMessagesOut.empty())
						{
							WriteHeader();
						}
					}
					else
					{
						std::cout << "[" << id << "] Write Body Fail.\n";
						m_socket.close();
					}
				});
		}

		void ReadHeader()
		{
			asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
				{						
					if (!ec)
					{
						if (m_msgTemporaryIn.header.size > 0)
						{
							m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
							ReadBody();
						}
						else
						{
							AddToIncomingMessageQueue();
						}
					}
					else
					{
						std::cout << "[" << id << "] Read Header Fail.\n";
						m_socket.close();
					}
				});
		}

		void ReadBody()
		{
			asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
				[this](std::error_code ec, std::size_t length)
				{						
					if (!ec)
					{
						AddToIncomingMessageQueue();
					}
					else
					{
						std::cout << "[" << id << "] Read Body Fail.\n";
						m_socket.close();
					}
				});
		}

		uint64_t desordenar(uint64_t n)
		{
			// AVISO - Estaría bien fortalecer esto con una función hash más fuerte
			uint64_t x = n ^ 0xDEADBEEFC0DECAFE;
			x = (x & 0xF0F0F0F0F0F0F0) >> 4 | (x & 0x0F0F0F0F0F0F0F) << 4;
			return x ^ 0xC0DEFACE12345678;
		}

		void WriteValidation()
		{
			asio::async_write(m_socket, asio::buffer(&m_nHandshakeOut, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						if (m_nOwnerType == owner::client)
						{
							ReadHeader();
						}
					}
					else
					{
						m_socket.close();
					}
				});
		}

		void ReadValidation(net::server_interface<T>* server = nullptr)
		{
			asio::async_read(m_socket, asio::buffer(&m_nHandshakeIn, sizeof(uint64_t)),
				[this, server](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						std::cout << "Server about to validate handshake: " << m_nHandshakeIn << std::endl;
		
						if (m_nOwnerType == owner::server)
						{
							std::cout << "Server received handshake: " << m_nHandshakeIn << "\n";
							std::cout << "Server out handshake: " << m_nHandshakeOut << "\n";
							std::cout << "Server check handshake: " << m_nHandshakeCheck << "\n";
							
							if (m_nHandshakeIn == m_nHandshakeCheck)
							{
								std::cout << "Client Validated.\n";
								server->OnClientValidated(this->shared_from_this());
								ReadHeader();
							}
							else
							{
								std::cout << "Client Disconnected (Fail Validation).\n";
								m_socket.close();
							}
						}
						else
						{
							std::cout << "Client received handshake: " << m_nHandshakeIn << "\n";
							m_nHandshakeOut = desordenar(m_nHandshakeIn);
							std::cout << "Client desorden handshake: " << m_nHandshakeOut << "\n";
							std::cout << "Client check handshake: " << m_nHandshakeCheck << "\n";
							WriteValidation();
						}
					}
					else
					{
						std::cout << "Client Disconnected (Read Validation).\n";
						m_socket.close();
					}
				});
		}

		void AddToIncomingMessageQueue()
		{				
			if(m_nOwnerType == owner::server)
				m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
			else
				m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });

			ReadHeader();
		}
		

	protected:
	// Each connection has a unique socket to a remote 
	asio::ip::tcp::socket m_socket;

	// This context is shared with the whole asio instance
	asio::io_context& m_asioContext;

	// This queue holds all messages to be sent to the remote side
	// of this connection
	tsqueue<message<T>> m_qMessagesOut;

	// This references the incoming queue of the parent object
	tsqueue<owned_message<T>>& m_qMessagesIn;

	// Incoming messages are constructed asynchronously, so we will
	// store the part assembled message here, until it is ready
	message<T> m_msgTemporaryIn;

	// The "owner" decides how some of the connection behaves
	owner m_nOwnerType = owner::server;

	// Handshake Validation			
	uint64_t m_nHandshakeOut = 0;
	uint64_t m_nHandshakeIn = 0;
	uint64_t m_nHandshakeCheck = 0;

	uint32_t id = 0;

	};
}