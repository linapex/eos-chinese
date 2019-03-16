
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030500880384>

#include <eosio.sudo/eosio.sudo.hpp>
#include <eosiolib/transaction.hpp>

namespace eosio {

/*
exec函数手动分析输入数据（而不是从调度器获取已分析的参数）
因为如果包含的事务非常大，那么在分派器中分析数据会使用太多的CPU

如果我们使用Dispatcher，函数签名应该是：

void sudo:：exec（帐户名执行器，
                 事务处理trx）
**/


void sudo::exec() {
   require_auth( _self );

   constexpr size_t max_stack_buffer_size = 512;
   size_t size = action_data_size();
   char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
   read_action_data( buffer, size );

   account_name executer;

   datastream<const char*> ds( buffer, size );
   ds >> executer;

   require_auth( executer );

   size_t trx_pos = ds.tellp();
   send_deferred( (uint128_t(executer) << 64) | current_time(), executer, buffer+trx_pos, size-trx_pos );
}

} ///命名空间eosio

EOSIO_ABI( eosio::sudo, (exec) )

