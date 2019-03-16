
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039262781440>

/*==--fixtfti.c-实现uu fixtfti------------------------------------
 *
 *LLVM编译器基础结构
 *
 *此文件是麻省理工学院和伊利诺伊大学公开赛的双重许可文件。
 *源许可证。有关详细信息，请参阅license.txt。
 *
 *==-------------------------------------------------------------------------------------==
 **/


#include "fp128.h"

__int128 ___fixtfti( float128_t a) {
    const __int128 fixint_max = (__int128)((~(unsigned __int128)0) / 2);
    const __int128 fixint_min = -fixint_max - 1;
//将A分解为符号、指数、有效位
    const __int128 aRep = toRep(a);
    const __int128 aAbs = aRep & absMask;
    const __int128 sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const __int128 significand = (aAbs & significandMask) | implicitBit;

//如果指数为负，则结果为零。
    if (exponent < 0)
        return 0;

//如果该值对于整数类型太大，则饱和。
    if ((unsigned)exponent >= sizeof(__int128) * CHAR_BIT)
        return sign == 1 ? fixint_max : fixint_min;

//如果0<=指数<有效位，则右移以获得结果。
//否则，向左移动。
    if (exponent < significandBits)
        return sign * (significand >> (significandBits - exponent));
    else
        return sign * ((__int128)significand << (exponent - significandBits));
}

