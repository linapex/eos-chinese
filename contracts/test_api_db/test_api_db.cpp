
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457036049944576>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosiolib/eosio.hpp>
#include "../test_api/test_api.hpp"

#include "test_db.cpp"

extern "C" {
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
      require_auth(code);
      WASM_TEST_HANDLER_EX(test_db, primary_i64_general);
      WASM_TEST_HANDLER_EX(test_db, primary_i64_lowerbound);
      WASM_TEST_HANDLER_EX(test_db, primary_i64_upperbound);
      WASM_TEST_HANDLER_EX(test_db, idx64_general);
      WASM_TEST_HANDLER_EX(test_db, idx64_lowerbound);
      WASM_TEST_HANDLER_EX(test_db, idx64_upperbound);
      WASM_TEST_HANDLER_EX(test_db, test_invalid_access);
      WASM_TEST_HANDLER_EX(test_db, idx_double_nan_create_fail);
      WASM_TEST_HANDLER_EX(test_db, idx_double_nan_modify_fail);
      WASM_TEST_HANDLER_EX(test_db, idx_double_nan_lookup_fail);
      WASM_TEST_HANDLER_EX(test_db, misaligned_secondary_key256_tests);

//未处理的测试调用
      eosio_assert(false, "Unknown Test");
   }

}

