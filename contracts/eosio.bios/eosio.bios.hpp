
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030249222144>

#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/privileged.hpp>

namespace eosio {

   class bios : public contract {
      public:
         bios( action_name self ):contract(self){}

         void setpriv( account_name account, uint8_t ispriv ) {
            require_auth( _self );
            set_privileged( account, ispriv );
         }

         void setalimits( account_name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
            require_auth( _self );
            set_resource_limits( account, ram_bytes, net_weight, cpu_weight );
         }

         void setglimits( uint64_t ram, uint64_t net, uint64_t cpu ) {
            (void)ram; (void)net; (void)cpu;
            require_auth( _self );
         }

         void setprods( std::vector<eosio::producer_key> schedule ) {
(void)schedule; //schedule参数只强制将操作数据反序列化为vector<producer_key>（必要的检查）
            require_auth( _self );

            constexpr size_t max_stack_buffer_size = 512;
            size_t size = action_data_size();
            char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
            read_action_data( buffer, size );
            set_proposed_producers(buffer, size);
         }

         void setparams( const eosio::blockchain_parameters& params ) {
            require_auth( _self );
            set_blockchain_parameters( params );
         }

         void reqauth( action_name from ) {
            require_auth( from );
         }

      private:
   };

} ///命名空间eosio

