#pragma once
class DoorStatePacket
{
private:
	static void PreparePacket( void* packer, unsigned char packetID );

public:
	static void State( bool open );
	static void SensorType(unsigned char type, bool enabled);
};

