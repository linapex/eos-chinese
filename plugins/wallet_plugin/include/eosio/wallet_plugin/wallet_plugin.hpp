
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457054211280896>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <appbase/application.hpp>
#include <fc/variant.hpp>
#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/transaction.hpp>

namespace fc { class variant; }

namespace eosio {
   using namespace appbase;

   namespace wallet {
      class wallet_manager;
   }
   using namespace wallet;

class wallet_plugin : public plugin<wallet_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES()

   wallet_plugin();
   wallet_plugin(const wallet_plugin&) = delete;
   wallet_plugin(wallet_plugin&&) = delete;
   wallet_plugin& operator=(const wallet_plugin&) = delete;
   wallet_plugin& operator=(wallet_plugin&&) = delete;
   virtual ~wallet_plugin() override = default;

   virtual void set_program_options(options_description& cli, options_description& cfg) override;
   void plugin_initialize(const variables_map& options);
   void plugin_startup() {}
   void plugin_shutdown() {}

//API接口提供程序
   wallet_manager& get_wallet_manager();

private:
   std::unique_ptr<wallet_manager> wallet_manager_ptr;
};

}

