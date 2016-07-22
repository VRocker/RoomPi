#include "SockHandler.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "webapiv1.h"
#include "Utils.h"
#include <stdio.h>

void handleExit()
{
	SockHandler::CleanupSingleton();
}

void sig_handler(int signum)
{
	SockHandler* handler = SockHandler::GetSingleton();
	if (handler)
	{
		handler->Shutdown();
	}
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

int main(int argc, char *argv[])
{
#ifndef DEBUG
	daemonise();
#endif

	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	atexit(handleExit);

	SockHandler* handler = SockHandler::GetSingleton();
	handler->Startup("ipc:///tmp/datasock.sock");

	{
		printf( "Getting serial..." );
		char serial[20] = { 0 };
		getSerialNumber(serial, sizeof(serial));
		printf( "serial: %s\n", serial );
		webapiv1::GetSingleton()->SetDeviceSerial(serial);
	}

	printf( "setting key\n" );
	// TODO: read from the config file
	webapiv1::GetSingleton()->SetDeviceAPIKey("ratableprestonpanskudu");
	// Login to the web API
	while (webapiv1::GetSingleton()->Authenticate() != eAPIErrors::Okay)
	{
		// Keep trying to login
		// Try agin in 5 minutes if it failed
		sleep(300);
	}

	handler->Run();

	handler->Shutdown();

	return 0;
}
