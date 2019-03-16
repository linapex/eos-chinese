
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457032560283648>

#pragma once
#include <eosiolib/privileged.hpp>

#include <vector>

namespace eosio {

   /*
    *定义活动生产者集的顺序、帐户名和签名密钥。
    *
    *@brief定义了活动生产者集的顺序、帐户名和签名密钥。
    **/

   struct producer_schedule {
      /*
       *计划的版本号。它是按顺序递增的版本号
       *
       *@计划的简要版本号
       **/

      uint32_t                     version;

      /*
       *此计划的生产商列表，包括其签名密钥
       *
       *@此计划的生产者的简要列表，包括其签名密钥
       **/

      std::vector<producer_key>    producers;
   };

///@产品类型
} ///命名空间eosio

