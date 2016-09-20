#pragma once

#include "ISingleton.h"
#include "strutils.h"

class DeviceInfo : public ISingleton<DeviceInfo>
{
public:
	DeviceInfo(void);
	~DeviceInfo(void);

	void SetName(const char* name);
	void Update(void);

private:
	void RetrieveGetaway(char* gateway, size_t len);
	char* m_name;
};
