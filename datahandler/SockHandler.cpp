#include "SockHandler.h"
#include <zmq.h>
#include <stdlib.h>
#include <memory.h>

SocketErrors SockHandler::Startup(const char* socketName)
{
	if (m_isRunning)
		return SocketErrors::AlreadyStarted;

	m_zmqContext = zmq_ctx_new();
	if (!m_zmqContext)
		return SocketErrors::NoContext;

	m_socket = zmq_socket(m_zmqContext, ZMQ_REP);
	if (!m_socket)
	{
		// Destroy the context if the socket couldn't be created
		zmq_ctx_term(m_zmqContext);
		m_zmqContext = nullptr;
		return SocketErrors::NoSocket;
	}

	if (zmq_bind(m_socket, socketName))
	{
		// Failed to bind to socket
		zmq_close(m_socket);
		m_socket = nullptr;

		zmq_ctx_term(m_zmqContext);
		m_zmqContext = nullptr;
		return SocketErrors::UnableToBind;
	}

	m_isRunning = true;

	return SocketErrors::None;
}

void SockHandler::Shutdown()
{
	if (m_isRunning)
	{
		m_isRunning = false;

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

void SockHandler::Run(void)
{
	zmq_pollitem_t items [] = {
		{ m_socket, 0, ZMQ_POLLIN, 0 }
	};

	while (m_isRunning)
	{
		zmq_poll(items, 1, -1);

		if (items[0].revents & ZMQ_POLLIN)
		{
			while (true)
			{
				zmq_msg_t message;

				int size = zmq_msg_recv(&message, m_socket, 0);

				char* packet = (char*) malloc(size + 1);
				memcpy(packet, zmq_msg_data(&message), size);
				packet[size] = 0;

				// Parse the packet
				
				free(packet);
				packet = nullptr;

				if (!zmq_msg_more(&message))
					break;
			}
		}
		
		zmq_sleep(1);
	}
}

void SockHandler::Send(const char* data, unsigned int size)
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

void SockHandler::ParsePacket(const char* data, int len)
{
	// Parse the incoming packet
}
