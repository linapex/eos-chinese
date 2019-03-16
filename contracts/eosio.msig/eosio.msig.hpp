
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030370856960>

#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>

namespace eosio {

   class multisig : public contract {
      public:
         multisig( account_name self ):contract(self){}

         void propose();
         void approve( account_name proposer, name proposal_name, permission_level level );
         void unapprove( account_name proposer, name proposal_name, permission_level level );
         void cancel( account_name proposer, name proposal_name, account_name canceler );
         void exec( account_name proposer, name proposal_name, account_name executer );

      private:
         struct proposal {
            name                       proposal_name;
            vector<char>               packed_transaction;

            auto primary_key()const { return proposal_name.value; }
         };
         typedef eosio::multi_index<N(proposal),proposal> proposals;

         struct approvals_info {
            name                       proposal_name;
            vector<permission_level>   requested_approvals;
            vector<permission_level>   provided_approvals;

            auto primary_key()const { return proposal_name.value; }
         };
         typedef eosio::multi_index<N(approvals),approvals_info> approvals;
   };

} ///命名空间eosio

