#include "rconfig.h"
#include "strutils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static FILE* g_configFile = 0;
static char g_configFileName[260] = { 0 };

unsigned int rconfig_open(const char* fileName)
{
	// No config name specified
	if ((!fileName) || (!*fileName))
		return 1;

	// Open the file in read-only mode
	FILE* tmpFile = fopen(fileName, "r");
	// Could not open file
	if (!tmpFile)
		return 2;

	// Check if it has already been initialised. If so then close the old file handle
	if (g_configFile)
	{
		fclose(g_configFile);
		g_configFile = 0;
	}

	// Assign the file handle to our global variable
	g_configFile = tmpFile;

	str_cpy(g_configFileName, fileName, sizeof(g_configFileName));

	return 0;
}

void rconfig_close()
{
	// Check if the pointer is valid
	if (g_configFile)
	{
		// Close the file
		fclose(g_configFile);
		// Nullify the pointer so we don't try to free this again
		g_configFile = 0;
	}
}

unsigned int rconfig_get_int(const char* key, int* ret)
{
	if (g_configFile)
	{
		static const char* tok = "\t =\r\n";

		char line[256] = { 0 };
		char* pch, *configKey;

		rewind(g_configFile);

		while (fgets(line, sizeof(line), g_configFile))
		{
			if (*line == '#')
				continue;

			pch = (char*)strtok(line, tok);
			if (pch)
			{
				configKey = pch;
				if (((pch = (char*)strtok(0, tok)) == 0) || (*pch == '\n'))
					continue;

				if (str_nocase_equal(configKey, key))
				{
					if (isnum(pch))
					{
						*ret = atoi(pch);
						return 0;
					}
				}
			}
		}
	}

	return 1;
}

unsigned int rconfig_get_string(const char* key, char* ret, unsigned int retSize)
{
	if (g_configFile)
	{
		static const char* tok = "\t =\r\n";

		char line[256] = { 0 };
		char* pch, *configKey;

		rewind(g_configFile);

		while (fgets(line, sizeof(line), g_configFile))
		{
			if (*line == '#')
				continue;

			pch = (char*)strtok(line, tok);
			if (pch)
			{
				configKey = pch;
				if (((pch = (char*)strtok(0, tok)) == 0) || (*pch == '\n'))
					continue;

				if (str_nocase_equal(configKey, key))
				{
					str_cpy(ret, pch, retSize);
					return 0;
				}
			}
		}
	}

	return 1;
}

unsigned int rconfig_set_string(const char* key, const char* value)
{
	if (g_configFile)
	{
		static const char* tok = "\t =\r\n";

		char line[256] = { 0 };
		char origLine[256] = { 0 };
		char* pch, *configKey;
		unsigned int written = 0;
		int outFile = -1;
		char tmpFileName[] = "/tmp/tmpXXXXXX";

		if ((outFile = mkstemp(tmpFileName)) < 0)
			return 2;

		rewind(g_configFile);

		while (fgets(origLine, sizeof(origLine), g_configFile))
		{
			if (*origLine == '#')
				goto writeLineAsNormal;

			strcpy(line, origLine);

			pch = (char*)strtok(line, tok);
			if (pch)
			{
				configKey = pch;
				if (str_nocase_equal(key, configKey))
				{
					write(outFile, key, strlen(key));
					write(outFile, "=", 1);
					write(outFile, value, strlen(value));
					write(outFile, "\n", 1);

					written = 1;
					continue;
				}
			}
			else
				goto writeLineAsNormal;

		writeLineAsNormal:
			write(outFile, origLine, strlen(origLine));

		}

		if (!written)
		{
			write(outFile, key, strlen(key));
			write(outFile, "=", 1);
			write(outFile, value, strlen(value));
			write(outFile, "\n", 1);
		}

		fsync(outFile);
		close(outFile);

		// Close the config stuff so we can rename the file
		rconfig_close();
		if (rename(tmpFileName, g_configFileName))
		{
			// Failed to rename the file, reopen the old config
			rconfig_open(g_configFileName);
			return 3;
		}

		// Reopen the config
		if (!rconfig_open(g_configFileName))
			return 0;
	}

	return 1;
}

unsigned int rconfig_remove_key(const char* key)
{
	if (g_configFile)
	{
		static const char* tok = "\t =\r\n";

		char line[256] = { 0 };
		char origLine[256] = { 0 };
		char* pch, *configKey;
		unsigned int removed = 0;
		int outFile = -1;
		char tmpFileName[] = "/tmp/tmpXXXXXX";

		if ((outFile = mkstemp(tmpFileName)) < 0)
			return 2;

		rewind(g_configFile);

		while (fgets(origLine, sizeof(origLine), g_configFile))
		{
			if (*origLine == '#')
				goto writeLineAsNormal;

			strcpy(line, origLine);

			pch = (char*)strtok(line, tok);
			if (pch)
			{
				configKey = pch;
				if (str_nocase_equal(key, configKey))
				{
					removed = 1;
					continue;
				}
			}
			else
				goto writeLineAsNormal;

		writeLineAsNormal:
			write(outFile, origLine, strlen(origLine));
		}

		fsync(outFile);
		close(outFile);

		// Close the config stuff so we can rename the file
		rconfig_close();
		if (rename(tmpFileName, g_configFileName))
		{
			// Failed to rename the file, reopen the old config
			rconfig_open(g_configFileName);
			return 3;
		}

		// Reopen the config
		if (!rconfig_open(g_configFileName))
			return 0;
	}

	return 1;
}