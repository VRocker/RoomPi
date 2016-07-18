#pragma once

class SensorInfoRecvPacket
{
public:
	static void ParsePacket(void* unpacker);

private:
	static void ParseTempAndHumidity(void* unpacker);
	static void ParseDoorStatus(void* unpacker);
};

