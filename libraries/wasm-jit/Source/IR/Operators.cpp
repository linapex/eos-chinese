
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045969473536>

#include "Operators.h"

namespace IR
{
	const char* getOpcodeName(Opcode opcode)
	{
		switch(opcode)
		{
		#define VISIT_OPCODE(encoding,name,nameString,Imm,...) case Opcode::name: return nameString;
		ENUM_OPERATORS(VISIT_OPCODE)
		#undef VISIT_OPCODE
		default: return "unknown";
		};
	}
}

