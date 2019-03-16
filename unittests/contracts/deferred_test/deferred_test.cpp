
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:19</date>
//</624457059693236224>

/*
 *@文件
 *@eos/license中定义的版权
 **/


#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/dispatcher.hpp>

using namespace eosio;

class deferred_test : public eosio::contract {
   public:
      using contract::contract;

      struct deferfunc_args {
         uint64_t payload;
      };

//ABI行动
      void defercall( account_name payer, uint64_t sender_id, account_name contract, uint64_t payload ) {
         print( "defercall called on ", name{_self}, "\n" );
         require_auth( payer );

         print( "deferred send of deferfunc action to ", name{contract}, " by ", name{payer}, " with sender id ", sender_id );
         transaction trx;
         deferfunc_args a = {.payload = payload};
         trx.actions.emplace_back(permission_level{_self, N(active)}, contract, N(deferfunc), a);
         trx.send( (static_cast<uint128_t>(payer) << 64) | sender_id, payer);
      }

//ABI行动
      void deferfunc( uint64_t payload ) {
         print("deferfunc called on ", name{_self}, " with payload = ", payload, "\n");
         eosio_assert( payload != 13, "value 13 not allowed in payload" );
      }

//ABI行动
      void inlinecall( account_name contract, account_name authorizer, uint64_t payload ) {
         action a( {permission_level{authorizer, N(active)}}, contract, N(deferfunc), payload );
         a.send();
      }

   private:
};

void apply_onerror(uint64_t receiver, const onerror& error ) {
   print("onerror called on ", name{receiver}, "\n");
}

extern "C" {
///apply方法实现将事件分派到此协定
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
      if( code == N(eosio) && action == N(onerror) ) {
         apply_onerror( receiver, onerror::from_current_action() );
      } else if( code == receiver ) {
         deferred_test thiscontract(receiver);
         if( action == N(defercall) ) {
            execute_action( &thiscontract, &deferred_test::defercall );
         } else if( action == N(deferfunc) ) {
            execute_action( &thiscontract, &deferred_test::deferfunc );
         } else if( action == N(inlinecall) ) {
            execute_action( &thiscontract, &deferred_test::inlinecall );
         }
      }
   }
}

//eosio-abi（延迟测试，（延迟调用）（延迟函数）

