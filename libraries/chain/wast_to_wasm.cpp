
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457044044288000>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosio/chain/wast_to_wasm.hpp>
#include <Inline/BasicTypes.h>
#include <IR/Module.h>
#include <IR/Validate.h>
#include <WAST/WAST.h>
#include <WASM/WASM.h>
#include <Runtime/Runtime.h>
#include <sstream>
#include <iomanip>
#include <fc/exception/exception.hpp>
#include <eosio/chain/exceptions.hpp>

namespace eosio { namespace chain {

   std::vector<uint8_t> wast_to_wasm( const std::string& wast ) 
   { 
      std::stringstream ss;
      
      try {
      IR::Module module;
      std::vector<WAST::Error> parse_errors;
      WAST::parseModule(wast.c_str(),wast.size(),module,parse_errors);
      if(parse_errors.size())
      {
//打印任何分析错误；
         ss << "Error parsing WebAssembly text file:" << std::endl;
         for(auto& error : parse_errors)
         {
            ss << ":" << error.locus.describe() << ": " << error.message.c_str() << std::endl;
            ss << error.locus.sourceLine << std::endl;
            ss << std::setw(error.locus.column(8)) << "^" << std::endl;
         }
         EOS_ASSERT( false, wasm_exception, "error parsing wast: ${msg}", ("msg",ss.str()) );
      }

      for(auto sectionIt = module.userSections.begin();sectionIt != module.userSections.end();++sectionIt)
      {
         if(sectionIt->name == "name") { module.userSections.erase(sectionIt); break; }
      }

      try
      {
//序列化WebAssembly模块。
         Serialization::ArrayOutputStream stream;
         WASM::serialize(stream,module);
         return stream.getBytes();
      }
      catch(const Serialization::FatalSerializationException& exception)
      {
         ss << "Error serializing WebAssembly binary file:" << std::endl;
         ss << exception.message << std::endl;
         EOS_ASSERT( false, wasm_exception, "error converting to wasm: ${msg}", ("msg",ss.get()) );
      } catch(const IR::ValidationException& e) {
         ss << "Error validating WebAssembly binary file:" << std::endl;
         ss << e.message << std::endl;
         EOS_ASSERT( false, wasm_exception, "error converting to wasm: ${msg}", ("msg",ss.get()) );
      }

} FC_CAPTURE_AND_RETHROW( (wast) ) }  ///WestytotoWASM

   std::string     wasm_to_wast( const std::vector<uint8_t>& wasm, bool strip_names ) {
      return wasm_to_wast( wasm.data(), wasm.size(), strip_names );
} //WasmithtoWaster

   std::string     wasm_to_wast( const uint8_t* data, uint64_t size, bool strip_names ) 
   { try {
       IR::Module module;
       Serialization::MemoryInputStream stream((const U8*)data,size);
       WASM::serialize(stream,module);
       if(strip_names)
          module.userSections.clear();
//将模块打印到WAST。
       return WAST::print(module);
} FC_CAPTURE_AND_RETHROW() } //WasmithtoWaster


} } //EOSIO：链

