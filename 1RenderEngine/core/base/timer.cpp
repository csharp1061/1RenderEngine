#include "timer.h"

namespace OEngine
{
	Timer::Timer(bool start) : m_started(start)
	{
		m_reference = std::chrono::steady_clock::now();
		if (start)
			this->start();
	}

	void Timer::start()
	{
		if (!m_started)
		{
			m_started = true;
			m_reference = std::chrono::steady_clock::now();
		}
	}

	float Timer::duration()
	{
		auto dur = std::chrono::steady_clock::now() - m_reference;
		m_reference = std::chrono::steady_clock::now();
		dur = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
		return dur.count() / 1000000000.0f;
	}
} // OEngine