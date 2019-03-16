
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457053020098560>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>

namespace eosio {

using namespace appbase;

class producer_api_plugin : public plugin<producer_api_plugin> {
   public:
      APPBASE_PLUGIN_REQUIRES((producer_plugin) (http_plugin))

      producer_api_plugin() = default;
      producer_api_plugin(const producer_api_plugin&) = delete;
      producer_api_plugin(producer_api_plugin&&) = delete;
      producer_api_plugin& operator=(const producer_api_plugin&) = delete;
      producer_api_plugin& operator=(producer_api_plugin&&) = delete;
      virtual ~producer_api_plugin() override = default;

      virtual void set_program_options(options_description& cli, options_description& cfg) override {}
      void plugin_initialize(const variables_map& vm);
      void plugin_startup();
      void plugin_shutdown() {}

   private:
};

}

