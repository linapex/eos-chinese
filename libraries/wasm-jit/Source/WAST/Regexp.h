
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457049589157888>

#pragma once

#include "Inline/BasicTypes.h"
#include "NFA.h"

namespace Regexp
{
//解析字符串中的正则表达式，并将其识别器添加到给定的NFA中
//识别器将从InitialState开始，并在正则表达式完全匹配后以finalState结束。
	void addToNFA(const char* regexpString,NFA::Builder* nfaBuilder,NFA::StateIndex initialState,NFA::StateIndex finalState);
};

