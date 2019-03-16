
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457031486541824>

#pragma once

namespace eosio {

/*
 *@defgroup contract type合同类型
 *@ingroup类型
 *@brief定义的合同类型是每个eosio合同的%基类
 *
 *@
 *
 **/


/*
 *@brief%EOSIO合同的基类。
 *@details%EOSIO合同的基类。%一个新合同应该从这个类派生，这样它就可以使用eosio-abi宏。
 **/

class contract {
   public:
      /*
       *根据合同名称建立新合同
       *
       *@brief构造一个新的契约对象。
       *@param n-此合同的名称
       **/

      contract( account_name n ):_self(n){}
      
      /*
       *
       *获取此合同名称
       *
       *@brief获取此合同名称。
       *@return account\u name-本合同名称
       **/

      inline account_name get_self()const { return _self; }

   protected:
      /*
       *本合同名称
       *
       *@简要说明本合同的名称。
       **/

      account_name _self;
};

///@合同类型
} ///命名空间eosio

