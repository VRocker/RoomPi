#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "dht11.h"
#include "gpiohandler.h"
#include "clientsock/ClientSock.h"
#include "TempSensorPacket.h"
#include "rconfig.h"

static bool g_isRunning = true;

void handleExit()
{
	rconfig_close();

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

	rconfig_open("/pi/etc/roompi.conf");

	unsigned int sensingInterval = 300;	// How many seconds the application should wait before reporting the temperature
	if (rconfig_get_int("TEMP_INTERVAL", (int*)&sensingInterval))
	{
		printf("Failed to set temp sensing interval. Using default of 5 minutes.\n");
	}

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

			// The data was retrieved, wait for the set time before reporting again
			sleep(sensingInterval);
		}
		else
		{
			// Failed to retrieve the data, try again in a second
			sleep(1);
		}
	}

	ClientSock::GetSingleton()->Disconnect();

	return 0;
}
