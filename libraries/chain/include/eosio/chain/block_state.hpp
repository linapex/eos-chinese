
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457041305407488>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/chain/block_header_state.hpp>
#include <eosio/chain/block.hpp>
#include <eosio/chain/transaction_metadata.hpp>
#include <eosio/chain/action_receipt.hpp>

namespace eosio { namespace chain {

   struct block_state : public block_header_state {
      explicit block_state( const block_header_state& cur ):block_header_state(cur){}
      block_state( const block_header_state& prev, signed_block_ptr b, bool skip_validate_signee );
      block_state( const block_header_state& prev, block_timestamp_type when );
      block_state() = default;

///weak_ptr prev_block_state…。
      signed_block_ptr                                    block;
      bool                                                validated = false;
      bool                                                in_current_chain = false;

///此数据与存储在块中的数据是冗余的，但便于
///当我们弹出一个块时重新捕获事务
      vector<transaction_metadata_ptr>                    trxs;
   };

   using block_state_ptr = std::shared_ptr<block_state>;

} } ///namespace eosio：：链

FC_REFLECT_DERIVED( eosio::chain::block_state, (eosio::chain::block_header_state), (block)(validated)(in_current_chain) )

