#pragma once

#include "ISingleton.h"

class DeviceInfo : public ISingleton<DeviceInfo>
{
public:
	DeviceInfo(void);
	~DeviceInfo(void);

	void SetName(const char* name);
	void Update(void);

private:
	char* m_name;
};
