
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457040810479616>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/chain/types.hpp>

namespace eosio { namespace chain {

   /*
    *对于发送的每个操作，都会生成此收据。
    **/

   struct action_receipt {
      account_name                    receiver;
      digest_type                     act_digest;
uint64_t                        global_sequence = 0; ///<Genesis以来调度的操作总数
uint64_t                        recv_sequence   = 0; ///<自Genesis以来此接收器的操作总数
      flat_map<account_name,uint64_t> auth_sequence;
fc::unsigned_int                code_sequence = 0; ///<设置代码总数
fc::unsigned_int                abi_sequence  = 0; ///<setabis总数

      digest_type digest()const { return digest_type::hash(*this); }
   };

} }  ///namespace eosio：：链

FC_REFLECT( eosio::chain::action_receipt, (receiver)(act_digest)(global_sequence)(recv_sequence)(auth_sequence)(code_sequence)(abi_sequence) )

