
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457053959622656>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/wallet_plugin/wallet_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>

namespace eosio {

using namespace appbase;

class wallet_api_plugin : public plugin<wallet_api_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((wallet_plugin) (http_plugin))

   wallet_api_plugin() = default;
   wallet_api_plugin(const wallet_api_plugin&) = delete;
   wallet_api_plugin(wallet_api_plugin&&) = delete;
   wallet_api_plugin& operator=(const wallet_api_plugin&) = delete;
   wallet_api_plugin& operator=(wallet_api_plugin&&) = delete;
   virtual ~wallet_api_plugin() override = default;

   virtual void set_program_options(options_description& cli, options_description& cfg) override {}
   void plugin_initialize(const variables_map& vm);
   void plugin_startup();
   void plugin_shutdown() {}

private:
};

}

