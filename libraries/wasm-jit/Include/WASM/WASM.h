
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045591986176>

#pragma once

#ifndef WEBASSEMBLY_API
	#define WEBASSEMBLY_API DLL_IMPORT
#endif

#include "Inline/BasicTypes.h"

namespace IR { struct Module; struct DisassemblyNames; }
namespace Serialization { struct InputStream; struct OutputStream; }

namespace WASM
{
	WEBASSEMBLY_API void serialize(Serialization::InputStream& stream,IR::Module& module);
	WEBASSEMBLY_API void serialize(Serialization::OutputStream& stream,const IR::Module& module);
}

