
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457049912119296>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once
#include <appbase/application.hpp>

#include <eosio/chain_plugin/chain_plugin.hpp>

namespace fc { class variant; }

namespace eosio {
   using chain::transaction_id_type;
   using std::shared_ptr;
   using namespace appbase;
   using chain::name;
   using fc::optional;
   using chain::uint128_t;

   typedef shared_ptr<class bnet_plugin_impl> bnet_ptr;
   typedef shared_ptr<const class bnet_plugin_impl> bnet_const_ptr;



/*
 *此插件跟踪与一组已配置帐户关联的所有操作和密钥。它使
 *用于为bnet查询分页的钱包。
 *
 *如果出现以下情况，将在帐户的bnet中包含一个操作：
 *接收器
 *-身份验证列表中指定的任何帐户
 *
 *如果密钥是在updateauth或newaccount的权限中引用的，则密钥将链接到帐户。
 **/

class bnet_plugin : public plugin<bnet_plugin> {
   public:
      APPBASE_PLUGIN_REQUIRES((chain_plugin))

      bnet_plugin();
      virtual ~bnet_plugin();

      virtual void set_program_options(options_description& cli, options_description& cfg) override;

      void plugin_initialize(const variables_map& options);
      void plugin_startup();
      void plugin_shutdown();

   private:
      bnet_ptr my;
};

} ///命名空间eosio



