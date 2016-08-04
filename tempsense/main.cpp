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

typedef enum
{
	TYPE_DHT11 = 0,
	TYPE_DHT22 = 1,
} DHTTypes;

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

	unsigned int pin = 25;
	if (rconfig_get_int("DHT_PIN", (int*)&pin))
	{
		printf("Failed to set 'DHT_PIN'. Using default of 25.\n");
	}

	unsigned int dht_type = TYPE_DHT11;
	if (rconfig_get_int("DHT_TYPE", (int*)&dht_type))
	{
		printf("Failed to set 'DHT_TYPE'. Using default of DHT11.\n");
	}

	unsigned int pi_type = TYPE_PI0;
	if (rconfig_get_int("PI_TYPE", (int*)&pi_type))
	{
		printf("Failed to set 'PI_TYPE'. Using default of PI0.\n");
	}

	gpiohandler::GetSingleton()->SetGPIOBase(pi_type);

	ClientSock::GetSingleton()->Connect("ipc:///tmp/datasock.sock");

	while ( g_isRunning )
	{
		printf( "Reading data...\n" );
		DHT11_Data data;
		if (DHT11::ReadData(&data, pin))
		{
			double temp = data.tempWhole;
			double humidity = data.humidityWhole;

			if (dht_type == TYPE_DHT22)
			{
				temp = ((data.tempWhole & 0x7F) * 256 + data.tempFraction) / 10.0;
				if ((data.tempWhole & 0x80) != 0) temp *= -1;

				humidity = (data.humidityWhole * 256 + data.humidityFraction) / 10.0;

				printf("Temp: %.2fC, Humidity: %.2f\n", temp, humidity);
			}
			else
				printf("Temp: %d.%d C, Humidity: %d.%d\n", temp, data.tempFraction, humidity, data.humidityFraction);

			// Send the data to the datahandler
			TempSensorPacket::TempAndHumidity(temp, humidity);

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
