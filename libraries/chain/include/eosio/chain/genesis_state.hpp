
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042010050560>


/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/chain/chain_config.hpp>
#include <eosio/chain/types.hpp>

#include <fc/crypto/sha256.hpp>

#include <string>
#include <vector>

namespace eosio { namespace chain {

struct genesis_state {
   genesis_state();

   static const string eosio_root_key;

   chain_config   initial_configuration = {
      .max_block_net_usage                  = config::default_max_block_net_usage,
      .target_block_net_usage_pct           = config::default_target_block_net_usage_pct,
      .max_transaction_net_usage            = config::default_max_transaction_net_usage,
      .base_per_transaction_net_usage       = config::default_base_per_transaction_net_usage,
      .net_usage_leeway                     = config::default_net_usage_leeway,
      .context_free_discount_net_usage_num  = config::default_context_free_discount_net_usage_num,
      .context_free_discount_net_usage_den  = config::default_context_free_discount_net_usage_den,

      .max_block_cpu_usage                  = config::default_max_block_cpu_usage,
      .target_block_cpu_usage_pct           = config::default_target_block_cpu_usage_pct,
      .max_transaction_cpu_usage            = config::default_max_transaction_cpu_usage,
      .min_transaction_cpu_usage            = config::default_min_transaction_cpu_usage,

      .max_transaction_lifetime             = config::default_max_trx_lifetime,
      .deferred_trx_expiration_window       = config::default_deferred_trx_expiration_window,
      .max_transaction_delay                = config::default_max_trx_delay,
      .max_inline_action_size               = config::default_max_inline_action_size,
      .max_inline_action_depth              = config::default_max_inline_action_depth,
      .max_authority_depth                  = config::default_max_auth_depth,
   };

   time_point                               initial_timestamp;
   public_key_type                          initial_key;

   /*
    *获取与这个起源状态相对应的链ID。
    *
    *这是创世纪之州的Sha256系列。
    **/

   chain_id_type compute_chain_id() const;

   friend inline bool operator==( const genesis_state& lhs, const genesis_state& rhs ) {
      return std::tie( lhs.initial_configuration, lhs.initial_timestamp, lhs.initial_key )
               == std::tie( rhs.initial_configuration, rhs.initial_timestamp, rhs.initial_key );
   };

   friend inline bool operator!=( const genesis_state& lhs, const genesis_state& rhs ) { return !(lhs == rhs); }

};

} } //命名空间eosio:：chain


FC_REFLECT(eosio::chain::genesis_state,
           (initial_timestamp)(initial_key)(initial_configuration))

