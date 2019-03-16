
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457039996784640>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosio/chain/block.hpp>
#include <eosio/chain/merkle.hpp>
#include <fc/io/raw.hpp>
#include <fc/bitutil.hpp>
#include <algorithm>

namespace eosio { namespace chain {
   digest_type block_header::digest()const
   {
      return digest_type::hash(*this);
   }

   uint32_t block_header::num_from_id(const block_id_type& id)
   {
      return fc::endian_reverse_u32(id._hash[0]);
   }

   block_id_type block_header::id()const
   {
//不要在ID中包含已签名的_块_头属性，特别是排除生产者_签名。
block_id_type result = digest(); //fc:：sha256:：hash（*static_cast<const block_header*>（this））；
      result._hash[0] &= 0xffffffff00000000;
result._hash[0] += fc::endian_reverse_u32(block_num()); //将块号存储在id中，160位足够散列
      return result;
   }


} }

