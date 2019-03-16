
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045671677952>

#pragma once

#ifndef WAST_API
	#define WAST_API DLL_IMPORT
#endif

#include "Inline/BasicTypes.h"
#include "Runtime/Runtime.h"
#include "WASM/WASM.h"

namespace WAST
{
//文本文件中的位置。
	struct TextFileLocus
	{
		std::string sourceLine;
		U32 newlines;
		U32 tabs;
		U32 characters;

		TextFileLocus(): newlines(0), tabs(0), characters(0) {}

		U32 lineNumber() const { return newlines + 1; }
		U32 column(U32 spacesPerTab = 4) const { return tabs * spacesPerTab + characters + 1; }

		std::string describe(U32 spacesPerTab = 4) const
		{
			return std::to_string(lineNumber()) + ":" + std::to_string(column(spacesPerTab));
		}
	};

//WAST分析错误。
	struct Error
	{
		TextFileLocus locus;
		std::string message;
	};

//从字符串分析模块。如果成功，则返回true，并将模块写入outmodule。
//如果失败，则返回false并将错误列表追加到outErrors。
	WAST_API bool parseModule(const char* string,Uptr stringLength,IR::Module& outModule,std::vector<Error>& outErrors);

//以WAST格式打印模块。
	WAST_API std::string print(const IR::Module& module);
}

