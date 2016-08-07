#pragma once

enum class PrimaryPacketIDs
{
	SensorInfo
};

enum class SensorPacketIDs
{
	TempAndHumid,
	DoorStatus,
	SensorType,
};

enum class SensorTypes
{
	Temp,
	Door,
};
