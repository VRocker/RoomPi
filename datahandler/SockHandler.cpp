#include "SockHandler.h"
#include <zmq.h>
#include <msgpack.h>
#include <zmq_utils.h>
#include <stdlib.h>
#include <memory.h>
#include "SensorInfoRecvPacket.h"

template<>
SockHandler* ISingleton< SockHandler >::m_singleton = nullptr;

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
			zmq_msg_t message;

			zmq_msg_init(&message);

			while (true)
			{
				int size = zmq_msg_recv(&message, m_socket, 0);
				if ( size > 0)
				{
					char* packet = (char*) malloc(size + 1);
					memcpy(packet, zmq_msg_data(&message), size);
					packet[size] = 0;

					// Parse the packet
					ParsePacket(packet, size);

					free(packet);
					packet = nullptr;
				}

				if (!zmq_msg_more(&message))
					break;
			}

			zmq_msg_close(&message);
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
		zmq_msg_init_size(&msg, size+1);
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
	// Packets use msgpack-c

	msgpack_unpacked msg;
	msgpack_unpacker unpacker;

	msgpack_unpacker_init(&unpacker, MSGPACK_UNPACKER_INIT_BUFFER_SIZE);

	msgpack_unpacker_reserve_buffer(&unpacker, len);
	memcpy(msgpack_unpacker_buffer(&unpacker), data, len);
	msgpack_unpacker_buffer_consumed(&unpacker, len);

	msgpack_unpacked_init(&msg);

	msgpack_unpack_return ret = msgpack_unpacker_next(&unpacker, &msg);

	if (ret)
	{
		// First thing should be a 'Packet ID' to determine what the packet actually contains
		if (msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
		{
			PrimaryPacketIDs primaryPacketID = (PrimaryPacketIDs)msg.data.via.u64;

			switch (primaryPacketID)
			{
			case PrimaryPacketIDs::SensorInfo:
			{
				SensorInfoRecvPacket::ParsePacket(&unpacker);
			}
			break;
			}
		}
	}

	msgpack_unpacked_destroy(&msg);
	msgpack_unpacker_destroy(&unpacker);

	// Send a blank reply to the client to stop it freezing up
	SockHandler::GetSingleton()->Send(nullptr, 0);
}
