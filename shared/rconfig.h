/** \file
*	\brief Allows executables to interface with the configuration file
*	\author Craig Richards
*	\date 22/07/2016
*/

#pragma once
#ifndef _RCONFIG_H
#define _RCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

	/** Opens the roompi config file
	*	@param fileName The filename of the config file to open, usually roompi.conf
	*	@retval 0 Successfully opened the file
	*	@retval 1 No config name specified
	*	@retval 2 Unable to open configuration file
	*/
	unsigned int rconfig_open(const char* fileName);
	/** Closes the config file */
	void rconfig_close();

	/** Gets a value from the config file as an integer
	*	@param[in] key The key to read from the config file
	*	@param[out] ret The return value
	*	@retval 0 Successfully parsed the integer and placed the value in ret
	*	@retval 1 Failed to parse an integer from the config file
	*/
	unsigned int rconfig_get_int(const char* key, int* ret);

	/** Gets a value from the config file as a string
	*	@param[in] key The key to read from the config file
	*	@param[out] ret The return value
	*	@param[in] retSize The size of the buffer to load the string in to
	*	@retval 0 Successfully parsed the string and placed the value in ret
	*	@retval 1 Failed to parse an integer from the config file
	*/
	unsigned int rconfig_get_string(const char* key, char* ret, unsigned int retSize);

	/** Sets a value in the config as a string
	*	@param key The key to write to the config
	*	@param value The value to write to the config
	*	@retval 0 Successfully wrote the configuration file
	*	@retval 1 Failed tto write to the configuration file (No config open)
	*	@retval 2 Failed to create a temporary file
	*	@retval	3 Failed to rename the temporary file to the new configuration file
	*/
	unsigned int rconfig_set_string(const char* key, const char* value);

	/**	Removes a key from the config file
	*	@param key The key to remove
	*	@retval 0 Successfully deleted the key from the configuration file
	*/
	unsigned int rconfig_remove_key(const char* key);

#ifdef __cplusplus
}
#endif

#endif

