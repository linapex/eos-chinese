
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457056316821504>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <cstdlib>
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <fc/log/logger.hpp>
#include <eosio/chain/exceptions.hpp>

//extern uint32_t eos_testing_genesis_timestamp；

void translate_fc_exception(const fc::exception &e) {
   std::cerr << "\033[33m" <<  e.to_detail_string() << "\033[0m" << std::endl;
   BOOST_TEST_FAIL("Caught Unexpected Exception");
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[]) {
//如果没有，则关闭区块链日志记录--不添加详细参数
//要启用verbose，请调用“tests/chain_test---verbose”
   bool is_verbose = false;
   std::string verbose_arg = "--verbose";
   for (int i = 0; i < argc; i++) {
      if (verbose_arg == argv[i]) {
         is_verbose = true;
         break;
      }
   }
   if(!is_verbose) fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::off);

//注册fc：：异常转换器
   boost::unit_test::unit_test_monitor.register_exception_translator<fc::exception>(&translate_fc_exception);

   std::srand(time(NULL));
   std::cout << "Random number generator seeded to " << time(NULL) << std::endl;
   /*
   const char*genesis_timestamp_str=getenv（“eos_testing_genesis_timestamp”）；
   如果（创世记时间戳！= null pPTR）
   {
      eos_testing_genesis_timestamp=std:：stoul（genesis_timestamp_str）；
   }
   std:：cout<“eos_testing_genesis_timestamp is”<<eos_testing_genesis_timestamp<<std:：endl；
   **/

   return nullptr;
}

