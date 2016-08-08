#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "gpiohandler.h"
#include "clientsock/ClientSock.h"
#include "DoorStatePacket.h"
#include "SensorDefs.h"

static bool g_isRunning = true;

void handleExit()
{
	ClientSock::CleanupSingleton();
	gpiohandler::CleanupSingleton();
}

void sig_handler(int signo)
{
	// A SIGTERM or SIGINT has been fired, tell the application to stop running
	g_isRunning = false;
}

void daemonise(void)
{
	int pid = 0;

	if ((pid = fork()) < 0)
	{
		// It forking failed!
		exit(1);
	}

	if (pid > 0)
		exit(0);
	else
	{
		// Daemonising
		setsid();
	}
}

int main()
{
	// Throw the application in the background
#ifndef _DEBUG
	daemonise();
#endif

	atexit(handleExit);

	// Handle signals
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	ClientSock::GetSingleton()->Connect("ipc:///tmp/datasock.sock");

	DoorStatePacket::SensorType((unsigned char)SensorTypes::Door, true);

	gpiohandler::GetSingleton()->SetDirection(22, GPIO_Direction::In);

	bool oldState = false;
	while (g_isRunning)
	{
		bool state = gpiohandler::GetSingleton()->ReadGPIO(22);
		if (state != oldState)
		{
			printf("Door state is now: %s\n", state ? "Closed" : "Open");
			// Packet expects an IsOpen but the state is reversed
			DoorStatePacket::State(!state);
		}

		oldState = state;

		sleep( 1 );
	}

	ClientSock::GetSingleton()->Disconnect();

	return 0;
}
