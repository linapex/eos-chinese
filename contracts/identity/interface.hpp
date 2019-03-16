
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457033709522944>

#pragma once

#include "common.hpp"

namespace identity {

   class interface : public identity_base {
      public:
         using identity_base::identity_base;

         identity_name get_claimed_identity( account_name acnt );

         account_name get_owner_for_identity( uint64_t receiver, identity_name ident );

         identity_name get_identity_for_account( uint64_t receiver, account_name acnt );
   };

}

