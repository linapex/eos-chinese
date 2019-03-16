
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457031310381056>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosiolib/types.h>

/*
 *@defgroup chaineapi链api
 *@brief定义用于查询内部链状态的API
 *@ingroup contractdev公司
 **/


/*
 *@defgroup chain c api chain c api定义组链
 *@brief定义了用于查询内部链状态的%c API
 *@ingroup链接API
 *@
 **/


extern "C" {
    /*
     *获取一组活动生产者。
     *@brief获取一组活动生产者。
     *
     *@param producers-指向帐户名缓冲区的指针
     *@param datalen-缓冲区的字节长度，当传递0时，将返回存储完整输出所需的大小。
     *
     *@return uint32_t-实际填充的字节数
     *@pre'producers'是指向至少'datalen'字节长的内存范围的指针。
     *@post传入的“producers”指针获取活动的producer数组。
     *
     *实例：
     *
     *@代码
     *帐户名生产者[21]；
     *uint32_t bytes_populated=get_active_producers（producers，sizeof（account_name）*21）；
     *@终结码
     **/


    uint32_t get_active_producers( account_name* producers, uint32_t datalen );

///@ ChanCAPI
}

