#pragma once

class TempSensorPacket
{
private:
	static void PreparePacket( void* packer, unsigned char packetID );

public:
	static void TempAndHumidity( int temp, int humidity );
	static void SensorType(unsigned char type, bool enabled);
};

