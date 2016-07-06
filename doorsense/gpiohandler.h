#pragma once

enum class GPIO_Direction
{
	In,
	Out
};

class gpiohandler
{
public:
	gpiohandler();
	~gpiohandler();

	void SetDirection(int pin, GPIO_Direction dir);
	void WriteGPIO(int pin, bool onOff);
	bool ReadGPIO(int pin);

public:
	static gpiohandler* getSingleton()
	{
		if (!m_singleton)
			m_singleton = new gpiohandler();

		return m_singleton;
	}
	static void RemoveSingleton()
	{
		if (m_singleton)
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}

private:
	void SetupIO();

private:
	volatile unsigned* m_gpio;
	void* m_gpioMap;

private:
	static gpiohandler* m_singleton;
};