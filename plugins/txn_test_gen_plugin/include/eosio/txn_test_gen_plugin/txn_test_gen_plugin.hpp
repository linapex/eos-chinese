
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457053829599232>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <appbase/application.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

namespace eosio {

using namespace appbase;

class txn_test_gen_plugin : public appbase::plugin<txn_test_gen_plugin> {
public:
   txn_test_gen_plugin();
   ~txn_test_gen_plugin();

   APPBASE_PLUGIN_REQUIRES((http_plugin)(chain_plugin))
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   std::unique_ptr<struct txn_test_gen_plugin_impl> my;
};

}

