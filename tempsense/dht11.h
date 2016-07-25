#pragma once

struct DHT11_Data
{
	int humidityWhole;
	int humidityFraction;

	int tempWhole;
	int tempFraction;
};

class DHT11
{
public:
	static bool ReadData(DHT11_Data* data, unsigned int pin);

};
