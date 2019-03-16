
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045248053248>

#pragma once

#include "IR.h"
#include "IR/Operators.h"

#include <string>

namespace IR
{
	struct Module;
	struct FunctionDef;

	struct ValidationException
	{
		std::string message;
		ValidationException(std::string&& inMessage): message(inMessage) {}
	};

	struct CodeValidationStreamImpl;

	struct CodeValidationStream
	{
		IR_API CodeValidationStream(const Module& module,const FunctionDef& function);
		IR_API ~CodeValidationStream();
		
		IR_API void finish();

		#define VISIT_OPCODE(_,name,nameString,Imm,...) IR_API void name(Imm imm = {});
		ENUM_OPERATORS(VISIT_OPCODE)
		#undef VISIT_OPCODE

	private:
		CodeValidationStreamImpl* impl;
	};

	template<typename InnerStream>
	struct CodeValidationProxyStream
	{
		CodeValidationProxyStream(const Module& module,const FunctionDef& function,InnerStream& inInnerStream)
		: codeValidationStream(module,function)
		, innerStream(inInnerStream)
		{}
		
		void finishValidation() { codeValidationStream.finish(); }

		#define VISIT_OPCODE(_,name,nameString,Imm,...) \
			void name(Imm imm = {}) \
			{ \
				codeValidationStream.name(imm); \
				innerStream.name(imm); \
			}
		ENUM_OPERATORS(VISIT_OPCODE)
		#undef VISIT_OPCODE

	private:

		CodeValidationStream codeValidationStream;
		InnerStream& innerStream;
	};

	IR_API void validateDefinitions(const IR::Module& module);
}

