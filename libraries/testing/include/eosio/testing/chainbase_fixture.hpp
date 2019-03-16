
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457044270780416>

#include <chainbase/chainbase.hpp>
#include <fc/filesystem.hpp>

#include <memory>

namespace eosio { namespace testing {

/*
 *使用raii创建和删除临时chainbase:：数据库的实用程序类
 *
 *@tparam max_size-chainbase:：database的最大大小
 **/

template<uint64_t MAX_SIZE>
struct chainbase_fixture {
   chainbase_fixture()
   : _tempdir()
   , _db(std::make_unique<chainbase::database>(_tempdir.path(), chainbase::database::read_write, MAX_SIZE))
   {
   }

   ~chainbase_fixture()
   {
      _db.reset();
      _tempdir.remove();
   }

   fc::temp_directory                    _tempdir;
   std::unique_ptr<chainbase::database>  _db;
};

} }  //EOSIO：测试

