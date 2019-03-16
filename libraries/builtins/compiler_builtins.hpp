
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039120175104>

#pragma once
#include <cstdint>
#include <softfloat.hpp>

extern "C" {
   __int128 ___fixdfti(uint64_t);
   __int128 ___fixsfti(uint32_t);
   __int128 ___fixtfti( float128_t);
   unsigned __int128 ___fixunsdfti(uint64_t);
   unsigned __int128 ___fixunssfti(uint32_t);
   unsigned __int128 ___fixunstfti(float128_t);
   double ___floattidf(__int128);
   double ___floatuntidf(unsigned __int128);
}

