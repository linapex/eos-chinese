
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042765025280>

/*
 *@文件
 *@eos/license.txt中定义的版权
 **/

#pragma once

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <future>
#include <memory>

namespace eosio { namespace chain {

//异步线程池并返回未来
   template<typename F>
   auto async_thread_pool( boost::asio::thread_pool& thread_pool, F&& f ) {
      auto task = std::make_shared<std::packaged_task<decltype( f() )()>>( std::forward<F>( f ) );
      boost::asio::post( thread_pool, [task]() { (*task)(); } );
      return task->get_future();
   }

} } //EOSIO：链



