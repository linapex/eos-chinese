
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457044933480448>

#pragma once

#include <chrono>

#include "Logging/Logging.h"

namespace Timing
{
//封装计时器，该计时器在构造时启动，在读取时停止。
	struct Timer
	{
		Timer(): startTime(std::chrono::high_resolution_clock::now()), isStopped(false) {}
		void stop() { endTime = std::chrono::high_resolution_clock::now(); }
		U64 getMicroseconds()
		{
			if(!isStopped) { stop(); }
			return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
		}
		F64 getMilliseconds() { return getMicroseconds() / 1000.0; }
		F64 getSeconds() { return getMicroseconds() / 1000000.0; }
	private:
		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point endTime;
		bool isStopped;
	};
	
//打印计时器的帮助程序。
	inline void logTimer(const char* context,Timer& timer) { Log::printf(Log::Category::metrics,"%s in %.2fms\n",context,timer.getMilliseconds()); }
	inline void logRatePerSecond(const char* context,Timer& timer,F64 numerator,const char* numeratorUnit)
	{
		Log::printf(Log::Category::metrics,"%s in %.2fms (%f %s/s)\n",
			context,
			timer.getMilliseconds(),
			numerator / timer.getSeconds(),
			numeratorUnit
			);
	}
}

