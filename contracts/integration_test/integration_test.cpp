
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457033881489408>

#include <eosiolib/eosio.hpp>
using namespace eosio;

struct integration_test : public eosio::contract {
      using contract::contract;

      struct payload {
         uint64_t            key;
         vector<uint64_t>    data;

         uint64_t primary_key()const { return key; }
      };
      typedef eosio::multi_index<N(payloads), payload> payloads;

///ABI行动
      void store( account_name from,
                  account_name to,
                  uint64_t     num ) {
         require_auth( from );
         eosio_assert( is_account( to ), "to account does not exist");
         eosio_assert( num < std::numeric_limits<size_t>::max(), "num to large");
         payloads data ( _self, from );
         uint64_t key = 0;
         const uint64_t num_keys = 5;
         while (data.find( key ) != data.end()) {
            key += num_keys;
         }
         for (size_t i = 0; i < num_keys; ++i) {
            data.emplace(from, [&]( auto& g ) {
               g.key = key + i;
               g.data = vector<uint64_t>( static_cast<size_t>(num), 5);
            });
         }
      }
};

EOSIO_ABI( integration_test, (store) )

