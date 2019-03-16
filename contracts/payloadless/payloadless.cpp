
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457034187673600>

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
using namespace eosio;

class payloadless : public eosio::contract {
  public:
      using contract::contract;

      void doit() {
         print( "Im a payloadless action" );
      }
};

EOSIO_ABI( payloadless, (doit) )

