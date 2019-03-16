
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457052185432064>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <appbase/application.hpp>
#include <fc/network/http/http_client.hpp>

namespace eosio {
   using namespace appbase;
   using fc::http_client;

   class http_client_plugin : public appbase::plugin<http_client_plugin>
   {
      public:
        http_client_plugin();
        virtual ~http_client_plugin();

        APPBASE_PLUGIN_REQUIRES()
        virtual void set_program_options(options_description&, options_description& cfg) override;

        void plugin_initialize(const variables_map& options);
        void plugin_startup();
        void plugin_shutdown();

        http_client& get_client() {
           return *my;
        }

      private:
        std::unique_ptr<http_client> my;
   };

}

