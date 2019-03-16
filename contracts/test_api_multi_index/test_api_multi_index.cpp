
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:14</date>
//</624457037429870592>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosiolib/eosio.hpp>
#include "../test_api/test_api.hpp"

#include "test_multi_index.cpp"

extern "C" {

   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
      require_auth(code);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_general);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_store_only);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_check_without_storing);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_require_find_fail);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_require_find_fail_with_msg);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_require_find_sk_fail);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_require_find_sk_fail_with_msg);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_general);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_store_only);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_check_without_storing);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_autoincrement_test);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_autoincrement_test_part1);
      WASM_TEST_HANDLER_EX(test_multi_index, idx128_autoincrement_test_part2);
      WASM_TEST_HANDLER_EX(test_multi_index, idx256_general);
      WASM_TEST_HANDLER_EX(test_multi_index, idx_double_general);
      WASM_TEST_HANDLER_EX(test_multi_index, idx_long_double_general);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pk_iterator_exceed_end);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_sk_iterator_exceed_end);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pk_iterator_exceed_begin);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_sk_iterator_exceed_begin);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_pk_ref_to_other_table);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_sk_ref_to_other_table);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_pk_end_itr_to_iterator_to);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_pk_end_itr_to_modify);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_pk_end_itr_to_erase);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_sk_end_itr_to_iterator_to);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_sk_end_itr_to_modify);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pass_sk_end_itr_to_erase);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_modify_primary_key);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_run_out_of_avl_pk);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_sk_cache_pk_lookup);
      WASM_TEST_HANDLER_EX(test_multi_index, idx64_pk_cache_sk_lookup);

//未处理的测试调用
      eosio_assert(false, "Unknown Test");
   }

}

