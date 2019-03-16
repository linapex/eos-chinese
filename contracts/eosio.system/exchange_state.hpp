
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030849007616>

#pragma once

#include <eosiolib/asset.hpp>

namespace eosiosystem {
   using eosio::asset;
   using eosio::symbol_type;

   typedef double real_type;

   /*
    *使用Bancor Math在两种资产类型之间创建50/50中继。的状态
    *Bancor Exchange完全包含在此结构中。没有外部
    *与使用此API相关的副作用。
    **/

   struct exchange_state {
      asset    supply;

      struct connector {
         asset balance;
         double weight = .5;

         EOSLIB_SERIALIZE( connector, (balance)(weight) )
      };

      connector base;
      connector quote;

      uint64_t primary_key()const { return supply.symbol; }

      asset convert_to_exchange( connector& c, asset in ); 
      asset convert_from_exchange( connector& c, asset in );
      asset convert( asset from, symbol_type to );

      EOSLIB_SERIALIZE( exchange_state, (supply)(base)(quote) )
   };

   typedef eosio::multi_index<N(rammarket), exchange_state> rammarket;

} ///namespace eosios系统

