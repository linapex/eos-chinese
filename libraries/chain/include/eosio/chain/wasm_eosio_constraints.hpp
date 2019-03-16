
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457043087986688>

#pragma once

#include <functional>
#include <vector>

namespace IR {
   struct Module;
}

namespace eosio { namespace chain { namespace wasm_constraints {
constexpr unsigned maximum_linear_memory      = 33*1024*1024;//字节
constexpr unsigned maximum_mutable_globals    = 1024;        //字节
constexpr unsigned maximum_table_elements     = 1024;        //元素
constexpr unsigned maximum_section_elements   = 1024;        //元素
constexpr unsigned maximum_linear_memory_init = 64*1024;     //字节
constexpr unsigned maximum_func_local_bytes   = 8192;        //字节
constexpr unsigned maximum_call_depth         = 250;         //嵌套调用
   constexpr unsigned maximum_code_size          = 20*1024*1024; 

   static constexpr unsigned wasm_page_size      = 64*1024;

   static_assert(maximum_linear_memory%wasm_page_size      == 0, "maximum_linear_memory must be mulitple of wasm page size");
   static_assert(maximum_mutable_globals%4                 == 0, "maximum_mutable_globals must be mulitple of 4");
   static_assert(maximum_table_elements*8%4096             == 0, "maximum_table_elements*8 must be mulitple of 4096");
   static_assert(maximum_linear_memory_init%wasm_page_size == 0, "maximum_linear_memory_init must be mulitple of wasm page size");
   static_assert(maximum_func_local_bytes%8                == 0, "maximum_func_local_bytes must be mulitple of 8");
   static_assert(maximum_func_local_bytes>32                   , "maximum_func_local_bytes must be greater than 32");
} //命名空间wasm_约束

}} //命名空间eosio，链

