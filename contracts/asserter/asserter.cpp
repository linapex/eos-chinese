
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030018535425>

/*
 *@文件
 *@eos/license中定义的版权
 **/


#include <asserter/asserter.hpp> ///defines断言结构（abi）

using namespace asserter;

static int global_variable = 45;

extern "C" {
///apply方法实现将事件分派到此协定
   /*D应用（uint64_t/*接收器*/，uint64_t代码，uint64_t操作）
       需要授权（代码）；
       if（code==n（断言器））
          if（action==n（procasert））
             assertdef def=eosio:：unpack_action_data<assertdef>（）；

             //可能断言？
             eosio_assert（（uint32_t）def.condition，def.message.c_str（））；
          else if（action==n（proverereset））
             eosio_assert（global_variable==45，“global variable初始化不好”）；
             全局变量=100；
          }
       }
    }
}

