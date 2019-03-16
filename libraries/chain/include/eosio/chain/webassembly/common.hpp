
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457043327062016>

#pragma once

#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>

#define EOSIO_INJECTED_MODULE_NAME "eosio_injection"

using namespace fc;

namespace eosio { namespace chain { 

   class apply_context;
   class transaction_context;

   template<typename T>
   struct class_from_wasm {
      /*
       *默认情况下，这只是构造一个对象
       *@param wasm-要使用的wasm_接口
       *@返回
       **/

      static auto value(apply_context& ctx) {
         return T(ctx);
      }
   };
   
   template<>
   struct class_from_wasm<transaction_context> {
      /*
       *默认情况下，这只是构造一个对象
       *@param wasm-要使用的wasm_接口
       *@返回
       **/

      template <typename ApplyCtx>
      static auto &value(ApplyCtx& ctx) {
         return ctx.trx_context;
      }
   };

   template<>
   struct class_from_wasm<apply_context> {
      /*
       *不要构造新的应用上下文，只需返回对现有ONT的引用
       *@帕拉姆WASM
       *@返回
       **/

      static auto &value(apply_context& ctx) {
         return ctx;
      }
   };

   /*
    *表示一个in-wasm内存数组的类
    *提示转录器下一个参数将
    *是一个大小（数据字节长度），并且对是一起验证的
    *这将触发内部调用程序的模板专门化
    *@ tPARAMT
    **/

   template<typename T>
   struct array_ptr {
      explicit array_ptr (T * value) : value(value) {}

      typename std::add_lvalue_reference<T>::type operator*() const {
         return *value;
      }

      T *operator->() const noexcept {
         return value;
      }

      template<typename U>
      operator U *() const {
         return static_cast<U *>(value);
      }

      T *value;
   }; 

   /*
    *表示必须以空结尾的WASM内存字符数组的类
    **/

   struct null_terminated_ptr {
      explicit null_terminated_ptr(char* value) : value(value) {}

      typename std::add_lvalue_reference<char>::type operator*() const {
         return *value;
      }

      char *operator->() const noexcept {
         return value;
      }

      template<typename U>
      operator U *() const {
         return static_cast<U *>(value);
      }

      char *value;
   };

} } //EOSIO：链

