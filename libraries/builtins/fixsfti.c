
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039212449792>

/*==--fixsfti.c-实现uu fixsfti------------------------------------
 *
 *LLVM编译器基础结构
 *
 *此文件是麻省理工学院和伊利诺伊大学公开赛的双重许可文件。
 *源许可证。有关详细信息，请参阅license.txt。
 *
 *==-------------------------------------------------------------------------------------==
 **/


#include "fp32.h"

typedef __int128 fixint_t;
typedef unsigned __int128 fixuint_t;

fixint_t ___fixsfti(uint32_t a) {
    const fixint_t fixint_max = (fixint_t)((~(fixuint_t)0) / 2);
    const fixint_t fixint_min = -fixint_max - 1;
//将A分解为符号、指数、有效位
    const rep_t aRep = a;
    const rep_t aAbs = aRep & absMask;
    const fixint_t sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const rep_t significand = (aAbs & significandMask) | implicitBit;

//如果指数为负，则结果为零。
    if (exponent < 0)
        return 0;

//如果该值对于整数类型太大，则饱和。
    if ((unsigned)exponent >= sizeof(fixint_t) * CHAR_BIT)
        return sign == 1 ? fixint_max : fixint_min;

//如果0<=指数<有效位，则右移以获得结果。
//否则，向左移动。
    if (exponent < significandBits)
        return sign * (significand >> (significandBits - exponent));
    else
        return sign * ((fixint_t)significand << (exponent - significandBits));
}

