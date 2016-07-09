#include "ClientSock.h"
#include <zmq.h>
#include <memory.h>

template<>
ClientSock* ISingleton< ClientSock >::m_singleton = nullptr;

ClientSocketErrors ClientSock::Connect(const char * socketName)
{
	if (m_isConnected)
		return ClientSocketErrors::AlreadyStarted;

	m_zmqContext = zmq_ctx_new();
	if (!m_zmqContext)
		return ClientSocketErrors::NoContext;

	m_socket = zmq_socket(m_zmqContext, ZMQ_REQ);
	if (!m_socket)
	{
		// Destroy the context if the socket couldn't be created
		zmq_ctx_term(m_zmqContext);
		m_zmqContext = nullptr;
		return ClientSocketErrors::NoSocket;
	}

	if (zmq_connect(m_socket, socketName))
	{
		// Failed to connect to socket
		zmq_close(m_socket);
		m_socket = nullptr;

		zmq_ctx_term(m_zmqContext);
		m_zmqContext = nullptr;
		return ClientSocketErrors::UnableToConnect;
	}

	m_isConnected = true;

	return ClientSocketErrors::None;
}

void ClientSock::Disconnect()
{
	if (m_isConnected)
	{
		m_isConnected = false;

		if (m_socket)
		{
			zmq_close(m_socket);
			m_socket = nullptr;
		}

		if (m_zmqContext)
		{
			zmq_ctx_term(m_zmqContext);
			m_zmqContext = nullptr;
		}
	}
}

void ClientSock::Send(const char * data, unsigned int size)
{
	if (!m_socket)
		return;

	zmq_msg_t msg;

	// We do this so we can send a null message back to the client
	if (size)
	{
		zmq_msg_init_size(&msg, size);
		memcpy(zmq_msg_data(&msg), data, size);
	}
	else
		zmq_msg_init(&msg);

	zmq_msg_send(&msg, m_socket, 0);

	zmq_msg_close(&msg);
}

void ClientSock::Recv()
{
	if (!m_socket)
		return;

	zmq_msg_t msg;

	zmq_msg_init(&msg);
	int size = zmq_msg_recv(&msg, m_socket, 0);

	zmq_msg_close(&msg);
}
