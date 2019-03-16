
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457054798483456>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <libintl.h>
#include <fc/variant.hpp>

namespace eosio { namespace client { namespace localize {
   #if !defined(_)
   #define _(str) str
   #endif

   #define localized(str, ...) localized_with_variant((str), fc::mutable_variant_object() __VA_ARGS__ )

   inline auto localized_with_variant( const char* raw_fmt, const fc::variant_object& args) {
      if (raw_fmt != nullptr) {
         try {
            return fc::format_string(::gettext(raw_fmt), args);
         } catch (...) {
         }
         return std::string(raw_fmt);
      }
      return std::string();
   }
}}}

