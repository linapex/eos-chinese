
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457053535997952>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosio/template_plugin/template_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _template_plugin = app().register_plugin<template_plugin>();

class template_plugin_impl {
   public:
};

template_plugin::template_plugin():my(new template_plugin_impl()){}
template_plugin::~template_plugin(){}

void template_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void template_plugin::plugin_initialize(const variables_map& options) {
   try {
      if( options.count( "option-name" )) {
//处理选项
      }
   }
   FC_LOG_AND_RETHROW()
}

void template_plugin::plugin_startup() {
//让魔法发生
}

void template_plugin::plugin_shutdown() {
//好吧，这足够神奇了
}

}

