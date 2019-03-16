
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:17</date>
//</624457052596473856>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <appbase/application.hpp>
#include <memory>

namespace eosio {

using mongo_db_plugin_impl_ptr = std::shared_ptr<class mongo_db_plugin_impl>;

/*
 *为MongoDB提供以下持久性：
 ＊帐户
 ＊行动
 *阻塞状态
 *块
 *事务跟踪
 ＊事务
 ＊耻骨键
 *账户控制
 *
 *MongoDB模式描述见数据字典（DB模式定义-EOS API）。
 *
 *如果未指定cmake-dbuild_mongo_db_plugin=true，则不编译/包含此插件。
 **/

class mongo_db_plugin : public plugin<mongo_db_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((chain_plugin))

   mongo_db_plugin();
   virtual ~mongo_db_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   mongo_db_plugin_impl_ptr my;
};

}


