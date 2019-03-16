
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457044803457024>

#pragma once

#include <stdarg.h>
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace Errors
{
//致命错误处理。
	[[noreturn]] inline void fatalf(const char* messageFormat,...)
	{
		va_list varArgs;
		va_start(varArgs,messageFormat);
		std::vfprintf(stderr,messageFormat,varArgs);
		std::fflush(stderr);
		va_end(varArgs);
		std::abort();
	}
	[[noreturn]] inline void fatal(const char* message) { fatalf("%s\n",message); }
	[[noreturn]] inline void unreachable() { fatalf("reached unreachable code\n"); }
	[[noreturn]] inline void unimplemented(const char* context) { fatalf("unimplemented: %s\n",context); }
}

//与断言类似，但在任何生成配置中都不会删除。
#define errorUnless(condition) if(!(condition)) { Errors::fatalf("errorUnless(%s) failed\n",#condition); }

#define WAVM_ASSERT_THROW(cond) ({ if( !(cond) ) throw std::runtime_error{"wavm assert: " #cond}; })

#define WAVM_ASSERT_TERMINATE(cond) ({ if( !(cond) ) { fprintf(stderr, "wavm assert in destructor: %s", #cond); std::terminate(); } })

