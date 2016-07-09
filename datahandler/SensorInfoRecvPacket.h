#pragma once

class SensorInfoRecvPacket
{
public:
	static void ParsePacket(void* unpacker, void* msg);

private:
	static void ParseTempAndHumidity(void* unpacker, void* msg);
	static void ParseDoorStatus(void* unpacker, void* msg);
};

