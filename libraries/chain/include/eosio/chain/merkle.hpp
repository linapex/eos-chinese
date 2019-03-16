
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042156851200>

#pragma once
#include <eosio/chain/types.hpp>

namespace eosio { namespace chain {

   digest_type make_canonical_left(const digest_type& val);
   digest_type make_canonical_right(const digest_type& val);

   bool is_canonical_left(const digest_type& val);
   bool is_canonical_right(const digest_type& val);


   inline auto make_canonical_pair(const digest_type& l, const digest_type& r) {
      return make_pair(make_canonical_left(l), make_canonical_right(r));
   };

   /*
    *计算一组摘要的merkle根，如果id是奇数，它将复制最后一个id。
    **/

   digest_type merkle( vector<digest_type> ids );

} } ///EOSIO：链

