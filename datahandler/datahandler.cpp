#include "SockHandler.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "webapiv1.h"
#include "Utils.h"
#include <stdio.h>
#include "rconfig.h"
#include "DeviceInfo.h"

void handleExit()
{
	rconfig_close();

	webapiv1::CleanupSingleton();
	DeviceInfo::CleanupSingleton();
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

	rconfig_open("/pi/etc/roompi.conf");

	SockHandler* handler = SockHandler::GetSingleton();
	handler->Startup("ipc:///tmp/datasock.sock");

	{
		printf( "Getting serial number... " );
		char serial[20] = { 0 };
		getSerialNumber(serial, sizeof(serial));
		printf( "%s\n", serial );
		webapiv1::GetSingleton()->SetDeviceSerial(serial);
	}

	{
loadAPIKey:
		printf("Getting API Key... ");
		char apiKey[128] = { 0 };
		if (!rconfig_get_string("API_KEY", apiKey, sizeof(apiKey)))
		{
			printf("%s\n", apiKey);
			// TODO: read from the config file
			webapiv1::GetSingleton()->SetDeviceAPIKey(apiKey);
		}
		else
		{
			printf("ERROR: Failed to find API key\n");
			printf("Rechecking in 1 minute...\n");
			sleep(60);
			goto loadAPIKey;
		}
	}

	{
		printf("Getting reporting URL... ");
		char reportingURL[255] = { 0 };
		if (!rconfig_get_string("REPORTING_URL", reportingURL, sizeof(reportingURL)))
		{
			printf("%s\n", reportingURL);

			webapiv1::GetSingleton()->SetBaseUrl(reportingURL);
		}
		else
			printf("NOT FOUND. Using default.\n");
	}

	// Login to the web API
	while (webapiv1::GetSingleton()->Authenticate() != eAPIErrors::Okay)
	{
		// Keep trying to login
		// Try agin in 5 minutes if it failed
		sleep(300);
	}

	DeviceInfo* info = DeviceInfo::GetSingleton();

	char netName[128];
	if (!rconfig_get_string("NETDEV_NAME", netName, sizeof(netName)))
		info->SetName(netName);
	else
		info->SetName("wlan0"); // default.

	info->Update();

	handler->Run();

	handler->Shutdown();

	return 0;
}
