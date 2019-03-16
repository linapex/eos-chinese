
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457032140853248>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <eosiolib/memory.h>
#include <eosiolib/print.hpp>

void* sbrk(size_t num_bytes);

  /*
   *@defgroup memory api内存api
   *@brief定义公共内存函数
   *@ingroup contractdev公司
   **/


  /*
   *@内存组内存CPPAPI内存C++ API
   *@brief定义公共内存函数
   *英格鲁存储器
   *
   *@
   **/


extern "C" {

/*
 *分配额外内存
 *
 *@brief分配额外内存
 *@param size-要分配的附加字节数
 *@返回void*-指向新分配内存开始的指针
 **/

void* malloc(size_t size);

/*
 *为一个**count**元素数组分配一块内存，每个元素的长度为**size**字节，并用0初始化所有位
 *
 *@brief为一个**count**元素数组分配一块内存，每个元素的长度为**size**字节，并用0初始化所有位
 *@param count-要分配的元素数
 *@param size-每个元素的大小
 *@返回void*-指向新分配内存开始的指针
 **/

void* calloc(size_t count, size_t size);

/*
 *重新分配给定的内存区域，以前由malloc（）、calloc（）或realloc（）分配。
 *
 *@brief重新分配给定的内存区域
 *@param ptr-指向要重新分配的内存区域的指针
 *@param size-内存的新大小
 *@返回void*-指向新重新分配内存的指针
 **/

void* realloc(void* ptr, size_t size);

/*
 *
 *释放先前由malloc（）、calloc（）或realloc（）分配的给定内存区域
 *@brief取消分配给定的内存区域
 *
 *@param ptr-指向要释放的内存的指针
 **/

void free(void* ptr);

}

///@内存PPAPI

