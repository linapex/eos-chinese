
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457041783558144>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>

namespace eosio { namespace chain {

   class apply_context;

   /*
    *@defgroup native_action_处理程序native action处理程序
    **/

///@
   void apply_eosio_newaccount(apply_context&);
   void apply_eosio_updateauth(apply_context&);
   void apply_eosio_deleteauth(apply_context&);
   void apply_eosio_linkauth(apply_context&);
   void apply_eosio_unlinkauth(apply_context&);

   /*
   void应用“eosio”后恢复（应用“context&）；
   void apply-eosio-passrecovery（apply-context&）；
   void apply-eosio-vetorecovery（apply-context&）；
   **/


   void apply_eosio_setcode(apply_context&);
   void apply_eosio_setabi(apply_context&);

   void apply_eosio_canceldelay(apply_context&);
///@结束操作处理程序

} } ///namespace eosio：：链

