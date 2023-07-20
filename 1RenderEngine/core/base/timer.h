#pragma once

#include <chrono>

/*
*  working like a stop-start clock
*/

namespace OEngine
{
	// TODO
	class Timer
	{
	public:
		Timer(bool start = false);

		Timer(const Timer&) = default;

		Timer(Timer&&) = default;

		Timer& operator=(const Timer&) = default;

		Timer& operator=(Timer&&) = default;

		virtual ~Timer() = default;

		void start();

		void stop();

		void reset();

		float duration();

	private:
		bool m_started;
		std::chrono::steady_clock::time_point m_reference;
	};

} // OEngine