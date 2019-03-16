
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457034762293248>

#include <eosiolib/eosio.hpp>
#include <eosiolib/privileged.h>
#include <eosiolib/producer_schedule.hpp>

namespace eosio {

   class testinline : public contract {
      public:
         testinline( action_name self ):contract(self){}

         void reqauth( account_name from ) {
            require_auth( from );
         }

         void forward( action_name reqauth, account_name forward_code, account_name forward_auth ) {
            require_auth( reqauth );
            INLINE_ACTION_SENDER(testinline, reqauth)( forward_code, {forward_auth,N(active)}, {forward_auth} );
//发送内联操作（testinline（forward_code），reqauth，转发，n（active），转发）；
//eosio:：dispatch_inline<account_name>（n（forward_code），n（reqauth），forward_auth，n（active），forward_auth）；
         }
   };

} ///命名空间eosio

