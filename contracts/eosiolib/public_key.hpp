
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457032606420992>

#pragma once 
#include <eosiolib/varint.hpp>
#include <eosiolib/serialize.hpp>

namespace eosio {

   /*
   *@defgroup public key type公钥类型
   *@ingroup类型
   *@brief指定公钥类型
   *
   *@
   **/

   
   /*
    *eosio公钥
    *@brief eosio公钥
    **/

   struct public_key {
      /*
       *公钥的类型，可以是k1或r1
       *@brief公钥类型
       **/

      unsigned_int        type;

      /*
       *公钥的字节数
       *
       *@公钥的简短字节数
       **/

      std::array<char,33> data;

      friend bool operator == ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) == std::tie(b.type,b.data);
      }
      friend bool operator != ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) != std::tie(b.type,b.data);
      }
      EOSLIB_SERIALIZE( public_key, (type)(data) )
   };
   
}

///@公开密钥类型

