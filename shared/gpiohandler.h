#pragma once

#include "ISingleton.h"

enum class GPIO_Direction
{
	In,
	Out
};

class gpiohandler : public ISingleton< gpiohandler >
{
public:
	gpiohandler();
	~gpiohandler();

	void SetDirection(int pin, GPIO_Direction dir);
	void WriteGPIO(int pin, bool onOff);
	bool ReadGPIO(int pin);

private:
	void SetupIO();

private:
	volatile unsigned* m_gpio;
	void* m_gpioMap;
};