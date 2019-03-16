
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:19</date>
//</624457059475132416>

/*
 *@文件
 *@eos/license中定义的版权
 **/


#include <boost/test/unit_test.hpp>
#include <eosio/chain/block_timestamp.hpp>
#include <fc/time.hpp>
#include <fc/exception/exception.hpp>

using namespace eosio;
using namespace chain;



BOOST_AUTO_TEST_SUITE(block_timestamp_tests)


BOOST_AUTO_TEST_CASE(constructor_test) {
	block_timestamp_type bt;
        BOOST_TEST( bt.slot == 0, "Default constructor gives wrong value");
        
	fc::time_point t(fc::seconds(978307200));	
	block_timestamp_type bt2(t);
	BOOST_TEST( bt2.slot == (978307200 - 946684800)*2, "Time point constructor gives wrong value");
}

BOOST_AUTO_TEST_CASE(conversion_test) {
	block_timestamp_type bt;
	fc::time_point t = (fc::time_point)bt;
	BOOST_TEST(t.time_since_epoch().to_seconds() == 946684800ll, "Time point conversion failed");

	block_timestamp_type bt1(200);
	t = (fc::time_point)bt1;
	BOOST_TEST(t.time_since_epoch().to_seconds() == 946684900ll, "Time point conversion failed");

}

BOOST_AUTO_TEST_SUITE_END()

