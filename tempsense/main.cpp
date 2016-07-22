#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "dht11.h"
#include "gpiohandler.h"
#include "clientsock/ClientSock.h"
#include "TempSensorPacket.h"

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

void daemonise( void )
{
	int pid = 0;

	if ( (pid = fork()) < 0 )
	{
		// It forking failed!
		exit( 1 );
	}

	if ( pid > 0 )
		exit( 0 );
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

	while ( g_isRunning )
	{
		printf( "Reading data...\n" );
		DHT11_Data data;
		if (DHT11::ReadData(&data))
		{
			printf("Temp: %d.%d C, Humidity: %d.%d\n", data.tempWhole, data.tempFraction, data.humidityWhole, data.humidityFraction);
			// Send the data to the datahandler
			TempSensorPacket::TempAndHumidity(data.tempWhole, data.humidityWhole);
		}

		// Report every minute
		sleep( 60 );
	}

	ClientSock::GetSingleton()->Disconnect();

	return 0;
}
