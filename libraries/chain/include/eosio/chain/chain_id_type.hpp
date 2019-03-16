
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457041473179648>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <fc/crypto/sha256.hpp>

struct hello;

namespace eosio {

   class net_plugin_impl;
   struct handshake_message;

   namespace chain_apis {
      class read_only;
   }

namespace chain {

   struct chain_id_type : public fc::sha256 {
      using fc::sha256::sha256;

      template<typename T>
      inline friend T& operator<<( T& ds, const chain_id_type& cid ) {
        ds.write( cid.data(), cid.data_size() );
        return ds;
      }

      template<typename T>
      inline friend T& operator>>( T& ds, chain_id_type& cid ) {
        ds.read( cid.data(), cid.data_size() );
        return ds;
      }

      void reflector_init()const;

      private:
         chain_id_type() = default;

//不幸的是，有些例外是必要的：
         template<typename T>
         friend T fc::variant::as()const;

         friend class eosio::chain_apis::read_only;

         friend class eosio::net_plugin_impl;
         friend struct eosio::handshake_message;

friend struct ::hello; //托多：快速黑客支持bnet_插件。需要更好的解决方案。
   };

} }  //命名空间eosio:：chain

namespace fc {
  class variant;
  void to_variant(const eosio::chain::chain_id_type& cid, fc::variant& v);
  void from_variant(const fc::variant& v, eosio::chain::chain_id_type& cid);
} //FC

