
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457032321208320>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosiolib/permission.h>
#include <eosiolib/transaction.hpp>

#include <set>
#include <limits>

namespace eosio {

   /*
    *@brief检查事务是否由提供的一组密钥和权限授权
    *
    *@param trx-检查授权的事务
    *@param provided_permissions-授权事务的权限集（空权限名用作通配符）
    *@param提供了\u个密钥-授权该事务的一组公钥
    *
    *@返回交易是否由提供的密钥和权限授权
    **/

   bool
   check_transaction_authorization( const transaction&                 trx,
                                    const std::set<permission_level>&  provided_permissions ,
                                    const std::set<public_key>&        provided_keys = std::set<public_key>()
                                  )
   {
      bytes packed_trx = pack(trx);

      bytes packed_keys;
      auto nkeys = provided_keys.size();
      if( nkeys > 0 ) {
         packed_keys = pack(provided_keys);
      }

      bytes packed_perms;
      auto nperms = provided_permissions.size();
      if( nperms > 0 ) {
         packed_perms = pack(provided_permissions);
      }

      auto res = ::check_transaction_authorization( packed_trx.data(),
                                                    packed_trx.size(),
                                                    (nkeys > 0)  ? packed_keys.data()  : (const char*)0,
                                                    (nkeys > 0)  ? packed_keys.size()  : 0,
                                                    (nperms > 0) ? packed_perms.data() : (const char*)0,
                                                    (nperms > 0) ? packed_perms.size() : 0
                                                  );

      return (res > 0);
   }

   /*
    *@简要检查权限是否由提供的延迟和提供的密钥和权限集授权
    *
    *@param account-权限的帐户所有者
    *@param permission-检查授权的权限名称
    *@param提供了\u个密钥-授权该事务的一组公钥
    *@param provided_permissions-授权事务的权限集（空权限名用作通配符）
    *@param provided_delay_us-以微秒为单位提供的延迟（不能超过Int64_max）
    *
    *@返回权限是否由提供的延迟、密钥和权限授权
    **/

   bool
   check_permission_authorization( account_name                       account,
                                   permission_name                    permission,
                                   const std::set<public_key>&        provided_keys,
                                   const std::set<permission_level>&  provided_permissions = std::set<permission_level>(),
                                   uint64_t                           provided_delay_us = static_cast<uint64_t>(std::numeric_limits<int64_t>::max())
                                 )
   {
      bytes packed_keys;
      auto nkeys = provided_keys.size();
      if( nkeys > 0 ) {
         packed_keys = pack(provided_keys);
      }

      bytes packed_perms;
      auto nperms = provided_permissions.size();
      if( nperms > 0 ) {
         packed_perms = pack(provided_permissions);
      }

      auto res = ::check_permission_authorization( account,
                                                   permission,
                                                   (nkeys > 0)  ? packed_keys.data()  : (const char*)0,
                                                   (nkeys > 0)  ? packed_keys.size()  : 0,
                                                   (nperms > 0) ? packed_perms.data() : (const char*)0,
                                                   (nperms > 0) ? packed_perms.size() : 0,
                                                   provided_delay_us
                                                 );

      return (res > 0);
   }

}

