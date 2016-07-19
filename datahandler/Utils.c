#include "Utils.h"
#include <stdio.h>
#include <string.h>

unsigned int getSerialNumber(char* serial, unsigned int len)
{
	char buffer[64] = { 0 };

	FILE* pFile = popen("cat /proc/cpuinfo | grep Serial | awk '{ print $3 }'", "r");
	if (pFile)
	{

		if (fgets(buffer, sizeof(buffer), pFile))
		{
			strncpy(serial, buffer, len);
			// Trim the space from the end of the serial num
			serial[strlen(serial) - 1] = 0;
		}

		pclose(pFile);
		pFile = 0;
	}

	if (*buffer)
		return 0;

	// Failed
	return 1;
}