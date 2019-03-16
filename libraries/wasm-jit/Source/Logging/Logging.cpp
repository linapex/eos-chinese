
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457046133051392>

#include "Logging.h"
#include "Platform/Platform.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

namespace Log
{
	static Platform::Mutex* categoryEnabledMutex = Platform::createMutex();
	static bool categoryEnabled[(Uptr)Category::num] =
	{
true, //错误
#ifdef _DEBUG //调试
			true,
		#else
			false,
		#endif
WAVM_METRICS_OUTPUT != 0 //韵律学
	};
	void setCategoryEnabled(Category category,bool enable)
	{
		Platform::Lock lock(categoryEnabledMutex);
		WAVM_ASSERT_THROW(category < Category::num);
		categoryEnabled[(Uptr)category] = enable;
	}
	bool isCategoryEnabled(Category category)
	{
		Platform::Lock lock(categoryEnabledMutex);
		WAVM_ASSERT_THROW(category < Category::num);
		return categoryEnabled[(Uptr)category];
	}
	void printf(Category category,const char* format,...)
	{
		Platform::Lock lock(categoryEnabledMutex);
		if(categoryEnabled[(Uptr)category])
		{
			va_list varArgs;
			va_start(varArgs,format);
			vfprintf(stdout,format,varArgs);
			fflush(stdout);
			va_end(varArgs);
		}
	}
}

