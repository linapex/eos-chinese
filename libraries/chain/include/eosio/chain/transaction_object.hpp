
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042974740480>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <fc/io/raw.hpp>

#include <eosio/chain/transaction.hpp>
#include <fc/uint128.hpp>

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include "multi_index_includes.hpp"

namespace eosio { namespace chain {
   using boost::multi_index_container;
   using namespace boost::multi_index;
   /*
    *此对象的目的是启用重复事务的检测。包括交易时
    *在块中添加事务对象。在块处理结束时，所有具有
    *过期可以从索引中删除。
    **/

   class transaction_object : public chainbase::object<transaction_object_type, transaction_object>
   {
         OBJECT_CTOR(transaction_object)

         id_type             id;
         time_point_sec      expiration;
         transaction_id_type trx_id;
   };

   struct by_expiration;
   struct by_trx_id;
   using transaction_multi_index = chainbase::shared_multi_index_container<
      transaction_object,
      indexed_by<
         ordered_unique< tag<by_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, transaction_object::id_type, id)>,
         ordered_unique< tag<by_trx_id>, BOOST_MULTI_INDEX_MEMBER(transaction_object, transaction_id_type, trx_id)>,
         ordered_unique< tag<by_expiration>,
            composite_key< transaction_object,
               BOOST_MULTI_INDEX_MEMBER( transaction_object, time_point_sec, expiration ),
               BOOST_MULTI_INDEX_MEMBER( transaction_object, transaction_object::id_type, id)
            >
         >
      >
   >;

   typedef chainbase::generic_index<transaction_multi_index> transaction_index;
} }

CHAINBASE_SET_INDEX_TYPE(eosio::chain::transaction_object, eosio::chain::transaction_multi_index)

FC_REFLECT(eosio::chain::transaction_object, (expiration)(trx_id))

