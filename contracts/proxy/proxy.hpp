
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457034368028672>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosiolib/eosio.hpp>

namespace proxy {

//ABI行动
   struct set_owner {
      account_name owner;
      uint32_t     delay;

      EOSLIB_SERIALIZE( set_owner, (owner)(delay) )
   };

//ABI表
   struct config {
      config(){}
      constexpr static uint64_t key = N(config);
      account_name        owner = 0;
      uint32_t            delay = 0;
      uint32_t            next_id = 0;
   };

} ///命名空间代理

