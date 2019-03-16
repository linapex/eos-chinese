
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042442063872>

#pragma once
#include <eosio/chain/config.hpp>
#include <eosio/chain/types.hpp>
#include <chainbase/chainbase.hpp>

namespace eosio { namespace chain {

   /*
    *作为生产商_schedule_类型的一部分，将生产商名称映射到其密钥。
    **/

   struct producer_key {
      account_name      producer_name;
      public_key_type   block_signing_key;

      friend bool operator == ( const producer_key& lhs, const producer_key& rhs ) {
         return tie( lhs.producer_name, lhs.block_signing_key ) == tie( rhs.producer_name, rhs.block_signing_key );
      }
      friend bool operator != ( const producer_key& lhs, const producer_key& rhs ) {
         return tie( lhs.producer_name, lhs.block_signing_key ) != tie( rhs.producer_name, rhs.block_signing_key );
      }
   };
   /*
    *定义活动生产者集的顺序、帐户名和签名密钥。
    **/

   struct producer_schedule_type {
uint32_t                                       version = 0; ///<按顺序递增版本号
      vector<producer_key>                           producers;

      public_key_type get_producer_key( account_name p )const {
         for( const auto& i : producers )
            if( i.producer_name == p ) 
               return i.block_signing_key;
         return public_key_type();
      }
   };

   struct shared_producer_schedule_type {
      shared_producer_schedule_type( chainbase::allocator<char> alloc )
      :producers(alloc){}

      shared_producer_schedule_type& operator=( const producer_schedule_type& a ) {
         version = a.version;
         producers.clear();
         producers.reserve( a.producers.size() );
         for( const auto& p : a.producers )
            producers.push_back(p);
         return *this;
      }

      operator producer_schedule_type()const {
         producer_schedule_type result;
         result.version = version;
         result.producers.reserve(producers.size());
         for( const auto& p : producers )
            result.producers.push_back(p);
         return result;
      }

      void clear() {
         version = 0;
         producers.clear();
      }

uint32_t                                       version = 0; ///<按顺序递增版本号
      shared_vector<producer_key>                    producers;
   };


   inline bool operator == ( const producer_schedule_type& a, const producer_schedule_type& b )
   {
      if( a.version != b.version ) return false;
      if ( a.producers.size() != b.producers.size() ) return false;
      for( uint32_t i = 0; i < a.producers.size(); ++i )
         if( a.producers[i] != b.producers[i] ) return false;
      return true;
   }
   inline bool operator != ( const producer_schedule_type& a, const producer_schedule_type& b )
   {
      return !(a==b);
   }


} } ///EOSIO：链

FC_REFLECT( eosio::chain::producer_key, (producer_name)(block_signing_key) )
FC_REFLECT( eosio::chain::producer_schedule_type, (version)(producers) )
FC_REFLECT( eosio::chain::shared_producer_schedule_type, (version)(producers) )

