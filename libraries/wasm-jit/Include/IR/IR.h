
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045017366528>

#pragma once

#include "Platform/Platform.h"

#ifndef IR_API
	#define IR_API DLL_IMPORT
#endif

namespace IR
{
	enum { maxMemoryPages = (Uptr)65536 };
	enum { numBytesPerPage = (Uptr)65536 };
	enum { numBytesPerPageLog2 = (Uptr)16 };

	enum { requireSharedFlagForAtomicOperators = false };
}

