
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457052877492224>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <appbase/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/net_plugin/protocol.hpp>

namespace eosio {
   using namespace appbase;

   struct connection_status {
      string            peer;
      bool              connecting = false;
      bool              syncing    = false;
      handshake_message last_handshake;
   };

   class net_plugin : public appbase::plugin<net_plugin>
   {
      public:
        net_plugin();
        virtual ~net_plugin();

        APPBASE_PLUGIN_REQUIRES((chain_plugin))
        virtual void set_program_options(options_description& cli, options_description& cfg) override;

        void plugin_initialize(const variables_map& options);
        void plugin_startup();
        void plugin_shutdown();

        void   broadcast_block(const chain::signed_block &sb);

        string                       connect( const string& endpoint );
        string                       disconnect( const string& endpoint );
        optional<connection_status>  status( const string& endpoint )const;
        vector<connection_status>    connections()const;

        size_t num_peers() const;
      private:
        std::unique_ptr<class net_plugin_impl> my;
   };

}

FC_REFLECT( eosio::connection_status, (peer)(connecting)(syncing)(last_handshake) )
