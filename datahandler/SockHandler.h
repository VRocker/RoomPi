#pragma once

#include "ISingleton.h"
#include "PacketDefs.h"

enum class SocketErrors
{
	None,
	NoContext,		/// A ZMQ context could not be created
	NoSocket,		/// A ZMQ socket could not be created
	UnableToBind,	/// A ZMQ socket could not bind to the specified socket name
	AlreadyStarted	/// The socket handler has already been started
};

class SockHandler : public ISingleton< SockHandler >
{
public:
	SockHandler()
		: m_isRunning(false), m_zmqContext(nullptr), m_socket(nullptr)
	{}
	~SockHandler()
	{
		Shutdown();
	}

	SocketErrors Startup(const char* socketName);
	void Shutdown();

	void Run();

	void Send( const char* data, unsigned int size );

private:
	static void ParsePacket(const char* data, int len);

private:
	void* m_zmqContext; /// The ZeroMQ context
	void* m_socket;		/// The socket that clients will use to talk to us

	bool m_isRunning;
};

