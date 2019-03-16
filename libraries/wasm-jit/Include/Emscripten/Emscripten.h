
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457044631490560>

#pragma once

#ifndef EMSCRIPTEN_API
	#define EMSCRIPTEN_API DLL_IMPORT
#endif

#include <vector>

namespace IR { struct Module; }
namespace Runtime { struct ModuleInstance; }

namespace Emscripten
{
	EMSCRIPTEN_API void initInstance(const IR::Module& module,Runtime::ModuleInstance* moduleInstance);
	EMSCRIPTEN_API void injectCommandArgs(const std::vector<const char*>& argStrings,std::vector<Runtime::Value>& outInvokeArgs);
}

