
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045298384896>

#pragma once

#ifndef LOGGING_API
	#define LOGGING_API DLL_IMPORT
#endif

#include "Inline/BasicTypes.h"
#include "Platform/Platform.h"

//调试日志记录。
namespace Log
{
//允许按类别筛选日志记录。
	enum class Category
	{
		error,
		debug,
		metrics,
		num
	};
	LOGGING_API void setCategoryEnabled(Category category,bool enable);
	LOGGING_API bool isCategoryEnabled(Category category);

//打印一些已分类、格式化的字符串，并刷新输出。不包括换行符。
	LOGGING_API void printf(Category category,const char* format,...);
};

