#pragma once

template< class T >
class ISingleton
{
private:
	static T* m_singleton;

public:
	static T* GetSingleton()
	{
		if (!m_singleton)
			m_singleton = new T();

		return m_singleton;
	}

	static void CleanupSingleton()
	{
		if (m_singleton)
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}
};