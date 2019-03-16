
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039308918784>

/*==--fixunsdfti.c-实现uu fixunsdfti-------------------------------
 *
 *LLVM编译器基础结构
 *
 *此文件是麻省理工学院和伊利诺伊大学公开赛的双重许可文件。
 *源许可证。有关详细信息，请参阅license.txt。
 *
 *==-------------------------------------------------------------------------------------==
 **/


#include "fp64.h"

typedef unsigned __int128 fixuint_t;

fixuint_t ___fixunsdfti(uint64_t a) {
//将A分解为符号、指数、有效位
    const rep_t aRep = a;
    const rep_t aAbs = aRep & absMask;
    const int sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const rep_t significand = (aAbs & significandMask) | implicitBit;

//如果值或指数为负，则结果为零。
    if (sign == -1 || exponent < 0)
        return 0;

//如果该值对于整数类型太大，则饱和。
    if ((unsigned)exponent >= sizeof(fixuint_t) * CHAR_BIT)
        return ~(fixuint_t)0;

//如果0<=指数<有效位，则右移以获得结果。
//否则，向左移动。
    if (exponent < significandBits)
        return significand >> (significandBits - exponent);
    else
        return (fixuint_t)significand << (exponent - significandBits);
}

