
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457052768440320>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>

namespace eosio {

using namespace appbase;

class net_api_plugin : public plugin<net_api_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((net_plugin) (http_plugin))

   net_api_plugin() = default;
   net_api_plugin(const net_api_plugin&) = delete;
   net_api_plugin(net_api_plugin&&) = delete;
   net_api_plugin& operator=(const net_api_plugin&) = delete;
   net_api_plugin& operator=(net_api_plugin&&) = delete;
   virtual ~net_api_plugin() override = default;

   virtual void set_program_options(options_description& cli, options_description& cfg) override {}
   void plugin_initialize(const variables_map& vm);
   void plugin_startup();
   void plugin_shutdown() {}

private:
};

}

