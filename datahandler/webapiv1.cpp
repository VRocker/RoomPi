#include "webapiv1.h"
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"
#include "json-c/json.h"
#include "SensorDefs.h"

template<>
webapiv1* ISingleton< webapiv1 >::m_singleton = nullptr;

void webapiv1::SetBaseUrl(const char * base)
{
	size_t len = strlen(base) + 1;
	char* tmp = (char*)realloc(m_baseUrl, len);
	if (tmp)
	{
		m_baseUrl = tmp;
		strncpy(m_baseUrl, base, len);
	}
}

void webapiv1::SetDeviceSerial(const char * serial)
{
	strncpy(m_deviceSerial, serial, sizeof(m_deviceSerial));
}

void webapiv1:: SetDeviceAPIKey(const char * key)
{
	size_t len = strlen(key) + 1;
	char* tmp = (char*)realloc(m_deviceAPIKey, len);
	if (tmp)
	{
		m_deviceAPIKey = tmp;
		strncpy(m_deviceAPIKey, key, len);
	}
}

webapiv1::webapiv1()
	: m_baseUrl(nullptr), m_deviceAPIKey(nullptr)
{
	*m_accessToken = 0;
	*m_deviceSerial = 0;

	m_curl = curl_easy_init();
}

webapiv1::~webapiv1()
{
	if (m_curl)
	{
		curl_easy_cleanup(m_curl);
		m_curl = nullptr;
	}

	if (m_baseUrl)
	{
		free(m_baseUrl);
		m_baseUrl = nullptr;
	}

	if (m_deviceAPIKey)
	{
		free(m_deviceAPIKey);
		m_deviceAPIKey = nullptr;
	}
}

eAPIErrors webapiv1::UpdateTemperature(int temp, int humid)
{
	if (!GetBaseUrl())
		return eAPIErrors::NoBaseURL;

	if (!*m_accessToken)
		return eAPIErrors::NoAccessToken;

	if (!m_curl)
		return eAPIErrors::FailedCurlInit;

	{
		char tmpBuffer[255] = { 0 };
		sprintf(tmpBuffer, "%s/api/updatetemperature", GetBaseUrl());
		curl_easy_setopt(m_curl, CURLOPT_URL, tmpBuffer);
	}

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

	json_object* json = json_object_new_object();
	json_object_object_add(json, "device_access_token", json_object_new_string(m_accessToken));
	json_object_object_add(json, "device_serial_key", json_object_new_string(m_deviceSerial));
	json_object_object_add(json, "temperature", json_object_new_int(temp));
	json_object_object_add(json, "humidity", json_object_new_int(humid));

	const char* jc = json_object_to_json_string(json);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jc);

	char* returnedJson = nullptr;
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &returnedJson);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, JsonCallback);

	CURLcode curl_res = CURLE_OK;
	long httpCode = 200;
	eAPIErrors retVal = eAPIErrors::Unknown;

	if ((curl_res = curl_easy_perform(m_curl)) == CURLE_OK)
	{
		if (json_object_put(json))
			json = nullptr;

		enum json_tokener_error jerr = json_tokener_success;

		curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
		if ((httpCode == 200) || (httpCode == 400))
		{
			if (returnedJson)
			{
				json = json_tokener_parse_verbose((const char*) returnedJson, &jerr);
				if (jerr == json_tokener_success)
				{
					json_object* token = nullptr;
					if (json_object_object_get_ex(json, "success", &token))
					{
						if (json_object_is_type(token, json_type_boolean))
						{
							bool isSuccessful = json_object_get_boolean(token);
							if (isSuccessful)
							{
								retVal = eAPIErrors::Okay;
							}
							else
							{
								token = nullptr;
								if (json_object_object_get_ex(json, "message", &token))
								{
									const char* err = json_object_get_string(token);
									printf("Error: %s\n", err);
								}

								// Nuts... no token for us :(
								retVal = eAPIErrors::Unsuccessful;
							}
						}
					}
				}
			}
		}
	}

	if (returnedJson)
	{
		free(returnedJson);
		returnedJson = nullptr;
	}

	if (json)
	{
		if (json_object_put(json))
			json = nullptr;
	}

	if (headers)
	{
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, nullptr);
		curl_slist_free_all(headers);
		headers = nullptr;
	}

	return retVal;
}

eAPIErrors webapiv1::UpdateDoorState(bool state)
{
	if (!GetBaseUrl())
		return eAPIErrors::NoBaseURL;

	if (!*m_accessToken)
		return eAPIErrors::NoAccessToken;

	if (!m_curl)
		return eAPIErrors::FailedCurlInit;

	{
		char tmpBuffer[255] = { 0 };
		sprintf(tmpBuffer, "%s/api/updatedoor", GetBaseUrl());
		curl_easy_setopt(m_curl, CURLOPT_URL, tmpBuffer);
	}

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

	json_object* json = json_object_new_object();
	json_object_object_add(json, "device_access_token", json_object_new_string(m_accessToken));
	json_object_object_add(json, "device_serial_key", json_object_new_string(m_deviceSerial));
	json_object_object_add(json, "is_open", json_object_new_boolean(state));

	const char* jc = json_object_to_json_string(json);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jc);

	char* returnedJson = nullptr;
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &returnedJson);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, JsonCallback);

	CURLcode curl_res = CURLE_OK;
	long httpCode = 200;
	eAPIErrors retVal = eAPIErrors::Unknown;

	if ((curl_res = curl_easy_perform(m_curl)) == CURLE_OK)
	{
		if (json_object_put(json))
			json = nullptr;

		enum json_tokener_error jerr = json_tokener_success;

		curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
		if ((httpCode == 200) || (httpCode == 400))
		{
			if (returnedJson)
			{
				json = json_tokener_parse_verbose((const char*) returnedJson, &jerr);
				if (jerr == json_tokener_success)
				{
					json_object* token = nullptr;
					if (json_object_object_get_ex(json, "success", &token))
					{
						if (json_object_is_type(token, json_type_boolean))
						{
							bool isSuccessful = json_object_get_boolean(token);
							if (isSuccessful)
							{
								retVal = eAPIErrors::Okay;
							}
							else
							{
								token = nullptr;
								if (json_object_object_get_ex(json, "message", &token))
								{
									const char* err = json_object_get_string(token);
									printf("Error: %s\n", err);
								}

								// Nuts... no token for us :(
								retVal = eAPIErrors::Unsuccessful;
							}
						}
					}
				}
			}
		}
	}

	if (returnedJson)
	{
		free(returnedJson);
		returnedJson = nullptr;
	}

	if (json)
	{
		if (json_object_put(json))
			json = nullptr;
	}

	if (headers)
	{
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, nullptr);
		curl_slist_free_all(headers);
		headers = nullptr;
	}

	return retVal;
}

eAPIErrors webapiv1::SetSensors(unsigned char sensors)
{
	if (!GetBaseUrl())
		return eAPIErrors::NoBaseURL;

	if (!*m_accessToken)
		return eAPIErrors::NoAccessToken;

	if (!m_curl)
		return eAPIErrors::FailedCurlInit;

	{
		char tmpBuffer[255] = { 0 };
		sprintf(tmpBuffer, "%s/api/setsensors", GetBaseUrl());
		curl_easy_setopt(m_curl, CURLOPT_URL, tmpBuffer);
	}

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

	json_object* json = json_object_new_object();
	json_object_object_add(json, "device_access_token", json_object_new_string(m_accessToken));
	json_object_object_add(json, "device_serial_key", json_object_new_string(m_deviceSerial));
	json_object_object_add(json, "has_temp_sensor", json_object_new_boolean((sensors & (1 << (unsigned char)SensorTypes::Temp)) ? true : false));
	json_object_object_add(json, "has_door_sensor", json_object_new_boolean((sensors & (1 << (unsigned char)SensorTypes::Door)) ? true : false));

	const char* jc = json_object_to_json_string(json);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jc);

	char* returnedJson = nullptr;
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &returnedJson);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, JsonCallback);

	CURLcode curl_res = CURLE_OK;
	long httpCode = 200;
	eAPIErrors retVal = eAPIErrors::Unknown;

	if ((curl_res = curl_easy_perform(m_curl)) == CURLE_OK)
	{
		if (json_object_put(json))
			json = nullptr;

		enum json_tokener_error jerr = json_tokener_success;

		curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
		if ((httpCode == 200) || (httpCode == 400))
		{
			if (returnedJson)
			{
				json = json_tokener_parse_verbose((const char*) returnedJson, &jerr);
				if (jerr == json_tokener_success)
				{
					json_object* token = nullptr;
					if (json_object_object_get_ex(json, "success", &token))
					{
						if (json_object_is_type(token, json_type_boolean))
						{
							bool isSuccessful = json_object_get_boolean(token);
							if (isSuccessful)
							{
								retVal = eAPIErrors::Okay;
							}
							else
							{
								token = nullptr;
								if (json_object_object_get_ex(json, "message", &token))
								{
									const char* err = json_object_get_string(token);
									printf("Error: %s\n", err);
								}

								// Nuts... no token for us :(
								retVal = eAPIErrors::Unsuccessful;
							}
						}
					}
				}
			}
		}
	}

	if (returnedJson)
	{
		free(returnedJson);
		returnedJson = nullptr;
	}

	if (json)
	{
		if (json_object_put(json))
			json = nullptr;
	}

	if (headers)
	{
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, nullptr);
		curl_slist_free_all(headers);
		headers = nullptr;
	}

	return retVal;
}

eAPIErrors webapiv1::Authenticate()
{
	if (!GetBaseUrl())
		return eAPIErrors::NoBaseURL;

	if (!m_deviceAPIKey)
		return eAPIErrors::NoDeviceKey;

	if (!*m_deviceSerial)
		return eAPIErrors::NoSerialNum;
	
	if (!m_curl)
		return eAPIErrors::FailedCurlInit;

	{
		char tmpBuffer[255] = { 0 };
		sprintf(tmpBuffer, "%s/api/requestaccess", GetBaseUrl());
		curl_easy_setopt(m_curl, CURLOPT_URL, tmpBuffer);
	}

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

	json_object* json = json_object_new_object();
	json_object_object_add(json, "device_api_key", json_object_new_string(m_deviceAPIKey));
	json_object_object_add(json, "device_serial_key", json_object_new_string(m_deviceSerial));

	const char* jc = json_object_to_json_string(json);
	curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jc);

	char* returnedJson = nullptr;
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &returnedJson);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, JsonCallback);

	CURLcode curl_res = CURLE_OK;
	long httpCode = 200;
	eAPIErrors retVal = eAPIErrors::Unknown;

	if ((curl_res = curl_easy_perform(m_curl)) == CURLE_OK)
	{
		if (json_object_put(json))
			json = nullptr;

		enum json_tokener_error jerr = json_tokener_success;

		curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &httpCode);
		if ((httpCode == 200) || (httpCode == 400))
		{
			if (returnedJson)
			{
				json = json_tokener_parse_verbose((const char*) returnedJson, &jerr);
				if (jerr == json_tokener_success)
				{
					json_object* token = nullptr;
					if (json_object_object_get_ex(json, "success", &token))
					{
						if (json_object_is_type(token, json_type_boolean))
						{
							bool isSuccessful = json_object_get_boolean(token);
							if (isSuccessful)
							{
								// We have an access token
								token = nullptr;
								if (json_object_object_get_ex(json, "access_token", &token))
								{
									if (json_object_is_type(token, json_type_string))
									{
										const char* accessToken = json_object_get_string(token);
										strncpy(m_accessToken, accessToken, sizeof(m_accessToken));

										printf("Access Token: %s\n", accessToken);

										retVal = eAPIErrors::Okay;
									}
									else
										printf("Token not a string.\n");
								}
								else
									printf("Cant find token.\n");
							}
							else
							{
								token = nullptr;
								if (json_object_object_get_ex(json, "message", &token))
								{
									const char* err = json_object_get_string(token);
									printf("Error: %s\n", err);
								}

								// Nuts... no token for us :(
								retVal = eAPIErrors::Unsuccessful;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		printf( "Curl Error: %s [%u]\n", curl_easy_strerror(curl_res), curl_res );
	}

	if (returnedJson)
	{
		free(returnedJson);
		returnedJson = nullptr;
	}

	if (json)
	{
		if (json_object_put(json))
			json = nullptr;
	}

	if (headers)
	{
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, nullptr);
		curl_slist_free_all(headers);
		headers = nullptr;
	}

	return retVal;
}

size_t webapiv1::JsonCallback(void *contents, size_t size, size_t nmemb, void **userp)
{
	size_t realSize = (size * nmemb);
	char* tmp = (char*)realloc(*userp, realSize + 1);
	if (tmp)
	{
		memcpy(tmp, contents, realSize);
		tmp[realSize] = 0;
		*userp = tmp;
	}

	return realSize;
}

