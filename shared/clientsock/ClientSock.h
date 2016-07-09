#pragma once

#include "ISingleton.h"

enum class ClientSocketErrors
{
	None,
	NoContext,		/// A ZMQ context could not be created
	NoSocket,		/// A ZMQ socket could not be created
	UnableToConnect,	/// A ZMQ socket could not bind to the specified socket name
	AlreadyStarted	/// The socket handler has already been started
};

class ClientSock : public ISingleton< ClientSock >
{
public:
	ClientSock()
		: m_isConnected(false), m_zmqContext(nullptr), m_socket(nullptr)
	{}
	~ClientSock()
	{
		Disconnect();
	}

	ClientSocketErrors Connect( const char* socketName );
	void Disconnect();

	void Send(const char* data, unsigned int size);
	// Currently no data is sent from datahandler back to this, so this function just stops ZeroMQ from stalling
	void Recv();

private:
	void* m_zmqContext;
	void* m_socket;

	bool m_isConnected;
};

