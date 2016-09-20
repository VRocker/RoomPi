#include "DeviceInfo.h"
#include <ifaddrs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include "webapiv1.h"

template<>
DeviceInfo* ISingleton<DeviceInfo>::m_singleton = nullptr;

DeviceInfo::DeviceInfo(void)
{
	m_name = nullptr;
}

DeviceInfo::~DeviceInfo(void)
{
	if (m_name != nullptr)
	{
		free(m_name);
		m_name = nullptr;
	}
}

void DeviceInfo::SetName(const char* name)
{
	if (m_name != nullptr) free(m_name);

	m_name = strdup(name);
}

void DeviceInfo::Update(void)
{
	struct ifaddrs *ifaddr, *ifa;
	char address[NI_MAXHOST];
	char netmask[NI_MAXHOST];
	char hostname[128];
	char gateway[18];

	if (m_name == nullptr) return;

	if (getifaddrs(&ifaddr) != 0) return;

	for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == nullptr || ifa->ifa_netmask == nullptr)
			continue;

		if (ifa->ifa_addr->sa_family != AF_INET)
			continue;

		if (strcmp(ifa->ifa_name, m_name) != 0)
			continue;

		if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), address, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) != 0)
			continue;

		if (getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in), netmask, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) != 0)
			continue;

		RetrieveGetaway(gateway, sizeof(gateway));

		gethostname(hostname, sizeof(hostname));

		webapiv1::GetSingleton()->SetDeviceInfo(address, netmask, gateway, hostname);

		//printf("[%s] IP: %s - Netmask: %s - Gateway: %s\n", hostname, address, netmask, gateway);

		break;
	}

	freeifaddrs(ifaddr);
}

void DeviceInfo::RetrieveGetaway(char* gateway, size_t len)
{
	FILE* net = popen("route -n | grep 'UG[ \t]' | awk '{ print $2 }'", "r");
	if (!net)
		return;

	char buffer[18];
	if (fgets(buffer, sizeof(buffer), net))
	{
		str_cpy(gateway, buffer, len);
		gateway[strlen(gateway) - 1] = 0;
	}
	pclose(net);
}
