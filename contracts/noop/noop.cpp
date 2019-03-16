
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457034107981824>

/*
 *@文件
 *@eos/license中定义的版权
 **/


#include <eosiolib/eosio.hpp>

namespace eosio {

   class noop: public contract {
      public:
         noop( account_name self ): contract( self ) { }
         void anyaction( account_name from,
                         /*ST标准：：字符串和/*类型*/，
                         const std：：字符串&/*日期*/ )

         {
            require_auth( from );
         }
   };

   EOSIO_ABI( noop, ( anyaction ) )

} ///EOSIO

