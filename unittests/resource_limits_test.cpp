
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:19</date>
//</624457060762783744>

#include <boost/test/unit_test.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/testing/chainbase_fixture.hpp>

#include <algorithm>

using namespace eosio::chain::resource_limits;
using namespace eosio::testing;
using namespace eosio::chain;



class resource_limits_fixture: private chainbase_fixture<512*1024>, public resource_limits_manager
{
   public:
      resource_limits_fixture()
      :chainbase_fixture()
      ,resource_limits_manager(*chainbase_fixture::_db)
      {
         add_indices();
         initialize_database();
      }

      ~resource_limits_fixture() {}

      chainbase::database::session start_session() {
         return chainbase_fixture::_db->start_undo_session(true);
      }
};

constexpr uint64_t expected_elastic_iterations(uint64_t from, uint64_t to, uint64_t rate_num, uint64_t rate_den ) {
   uint64_t result = 0;
   uint64_t cur = from;

   while((from < to && cur < to) || (from > to && cur > to)) {
      cur = cur * rate_num / rate_den;
      result += 1;
   }

   return result;
}


constexpr uint64_t expected_exponential_average_iterations( uint64_t from, uint64_t to, uint64_t value, uint64_t window_size ) {
   uint64_t result = 0;
   uint64_t cur = from;

   while((from < to && cur < to) || (from > to && cur > to)) {
      cur = cur * (uint64_t)(window_size - 1) / (uint64_t)(window_size);
      cur += value / (uint64_t)(window_size);
      result += 1;
   }

   return result;
}

BOOST_AUTO_TEST_SUITE(resource_limits_test)

   /*
    *测试以确保试块的弹性极限按预期放松和收缩。
    **/

   BOOST_FIXTURE_TEST_CASE(elastic_cpu_relax_contract, resource_limits_fixture) try {
      const uint64_t desired_virtual_limit = config::default_max_block_cpu_usage * 1000ULL;
      const uint64_t expected_relax_iterations = expected_elastic_iterations( config::default_max_block_cpu_usage, desired_virtual_limit, 1000, 999 );

//这足够平均值达到/超过目标（触发拥塞处理），然后迭代收缩到最小值。
//当到达/超过目标并开始处理拥塞时，对拉动双重任务的迭代减去1
      const uint64_t expected_contract_iterations =
              expected_exponential_average_iterations(0, EOS_PERCENT(config::default_max_block_cpu_usage, config::default_target_block_cpu_usage_pct), config::default_max_block_cpu_usage, config::block_cpu_usage_average_window_ms / config::block_interval_ms ) +
              expected_elastic_iterations( desired_virtual_limit, config::default_max_block_cpu_usage, 99, 100 ) - 1;

      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, -1, -1, -1);
      process_account_limit_updates();

//尽快从启动状态（拥挤）放松到空闲状态
      uint32_t iterations = 0;
      while( get_virtual_block_cpu_limit() < desired_virtual_limit && iterations <= expected_relax_iterations ) {
         add_transaction_usage({account},0,0,iterations);
         process_block_usage(iterations++);
      }

      BOOST_REQUIRE_EQUAL(iterations, expected_relax_iterations);
      BOOST_REQUIRE_EQUAL(get_virtual_block_cpu_limit(), desired_virtual_limit);

//尽可能快地将资源从空闲状态推回到拥挤状态
      while( get_virtual_block_cpu_limit() > config::default_max_block_cpu_usage
              && iterations <= expected_relax_iterations + expected_contract_iterations ) {
         add_transaction_usage({account}, config::default_max_block_cpu_usage, 0, iterations);
         process_block_usage(iterations++);
      }

      BOOST_REQUIRE_EQUAL(iterations, expected_relax_iterations + expected_contract_iterations);
      BOOST_REQUIRE_EQUAL(get_virtual_block_cpu_limit(), config::default_max_block_cpu_usage);
   } FC_LOG_AND_RETHROW();

   /*
    *测试以确保试块的弹性极限按预期放松和收缩。
    **/

   BOOST_FIXTURE_TEST_CASE(elastic_net_relax_contract, resource_limits_fixture) try {
      const uint64_t desired_virtual_limit = config::default_max_block_net_usage * 1000ULL;
      const uint64_t expected_relax_iterations = expected_elastic_iterations( config::default_max_block_net_usage, desired_virtual_limit, 1000, 999 );

//这足够平均值达到/超过目标（触发拥塞处理），然后迭代收缩到最小值。
//当到达/超过目标并开始处理拥塞时，对拉动双重任务的迭代减去1
      const uint64_t expected_contract_iterations =
              expected_exponential_average_iterations(0, EOS_PERCENT(config::default_max_block_net_usage, config::default_target_block_net_usage_pct), config::default_max_block_net_usage, config::block_size_average_window_ms / config::block_interval_ms ) +
              expected_elastic_iterations( desired_virtual_limit, config::default_max_block_net_usage, 99, 100 ) - 1;

      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, -1, -1, -1);
      process_account_limit_updates();

//尽快从启动状态（拥挤）放松到空闲状态
      uint32_t iterations = 0;
      while( get_virtual_block_net_limit() < desired_virtual_limit && iterations <= expected_relax_iterations ) {
         add_transaction_usage({account},0,0,iterations);
         process_block_usage(iterations++);
      }

      BOOST_REQUIRE_EQUAL(iterations, expected_relax_iterations);
      BOOST_REQUIRE_EQUAL(get_virtual_block_net_limit(), desired_virtual_limit);

//尽可能快地将资源从空闲状态推回到拥挤状态
      while( get_virtual_block_net_limit() > config::default_max_block_net_usage
              && iterations <= expected_relax_iterations + expected_contract_iterations ) {
         add_transaction_usage({account},0, config::default_max_block_net_usage, iterations);
         process_block_usage(iterations++);
      }

      BOOST_REQUIRE_EQUAL(iterations, expected_relax_iterations + expected_contract_iterations);
      BOOST_REQUIRE_EQUAL(get_virtual_block_net_limit(), config::default_max_block_net_usage);
   } FC_LOG_AND_RETHROW();

   /*
    *创建5个具有不同权重的帐户，验证容量是否如预期，并且使用是否适当地强制执行它们。
    **/



//TODO:还原加权容量CPU测试
#if 0
   BOOST_FIXTURE_TEST_CASE(weighted_capacity_cpu, resource_limits_fixture) try {
      const vector<int64_t> weights = { 234, 511, 672, 800, 1213 };
      const int64_t total = std::accumulate(std::begin(weights), std::end(weights), 0LL);
      vector<int64_t> expected_limits;
      std::transform(std::begin(weights), std::end(weights), std::back_inserter(expected_limits), [total](const auto& v){ return v * config::default_max_block_cpu_usage / total; });

      for (int64_t idx = 0; idx < weights.size(); idx++) {
         const account_name account(idx + 100);
         initialize_account(account);
         set_account_limits(account, -1, -1, weights.at(idx));
      }

      process_account_limit_updates();

      for (int64_t idx = 0; idx < weights.size(); idx++) {
         const account_name account(idx + 100);
         BOOST_CHECK_EQUAL(get_account_cpu_limit(account), expected_limits.at(idx));

{  //使用预期限制，应成功…卷土重来
            auto s = start_session();
            add_transaction_usage({account}, expected_limits.at(idx), 0, 0);
            s.undo();
         }

//使用过多，预计会失败；
         BOOST_REQUIRE_THROW(add_transaction_usage({account}, expected_limits.at(idx) + 1, 0, 0), tx_cpu_usage_exceeded);
      }
   } FC_LOG_AND_RETHROW();

   /*
    *创建5个具有不同权重的帐户，验证容量是否如预期，并且使用是否适当地强制执行它们。
    **/

   BOOST_FIXTURE_TEST_CASE(weighted_capacity_net, resource_limits_fixture) try {
      const vector<int64_t> weights = { 234, 511, 672, 800, 1213 };
      const int64_t total = std::accumulate(std::begin(weights), std::end(weights), 0LL);
      vector<int64_t> expected_limits;
      std::transform(std::begin(weights), std::end(weights), std::back_inserter(expected_limits), [total](const auto& v){ return v * config::default_max_block_net_usage / total; });

      for (int64_t idx = 0; idx < weights.size(); idx++) {
         const account_name account(idx + 100);
         initialize_account(account);
         set_account_limits(account, -1, weights.at(idx), -1 );
      }

      process_account_limit_updates();

      for (int64_t idx = 0; idx < weights.size(); idx++) {
         const account_name account(idx + 100);
         BOOST_CHECK_EQUAL(get_account_net_limit(account), expected_limits.at(idx));

{  //使用预期限制，应成功…卷土重来
            auto s = start_session();
            add_transaction_usage({account}, 0, expected_limits.at(idx), 0);
            s.undo();
         }

//使用过多，预计会失败；
         BOOST_REQUIRE_THROW(add_transaction_usage({account}, 0, expected_limits.at(idx) + 1, 0), tx_net_usage_exceeded);
      }
   } FC_LOG_AND_RETHROW();
#endif

   BOOST_FIXTURE_TEST_CASE(enforce_block_limits_cpu, resource_limits_fixture) try {
      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, -1, -1, -1 );
      process_account_limit_updates();

      const uint64_t increment = 1000;
      const uint64_t expected_iterations = config::default_max_block_cpu_usage / increment;

      for (int idx = 0; idx < expected_iterations; idx++) {
         add_transaction_usage({account}, increment, 0, 0);
      }

      BOOST_REQUIRE_THROW(add_transaction_usage({account}, increment, 0, 0), block_resource_exhausted);

   } FC_LOG_AND_RETHROW();

   BOOST_FIXTURE_TEST_CASE(enforce_block_limits_net, resource_limits_fixture) try {
      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, -1, -1, -1 );
      process_account_limit_updates();

      const uint64_t increment = 1000;
      const uint64_t expected_iterations = config::default_max_block_net_usage / increment;

      for (int idx = 0; idx < expected_iterations; idx++) {
         add_transaction_usage({account}, 0, increment, 0);
      }

      BOOST_REQUIRE_THROW(add_transaction_usage({account}, 0, increment, 0), block_resource_exhausted);

   } FC_LOG_AND_RETHROW();

   BOOST_FIXTURE_TEST_CASE(enforce_account_ram_limit, resource_limits_fixture) try {
      const uint64_t limit = 1000;
      const uint64_t increment = 77;
      const uint64_t expected_iterations = (limit + increment - 1 ) / increment;


      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, limit, -1, -1 );
      process_account_limit_updates();

      for (int idx = 0; idx < expected_iterations - 1; idx++) {
         add_pending_ram_usage(account, increment);
         verify_account_ram_usage(account);
      }

      add_pending_ram_usage(account, increment);
      BOOST_REQUIRE_THROW(verify_account_ram_usage(account), ram_usage_exceeded);
   } FC_LOG_AND_RETHROW();

   BOOST_FIXTURE_TEST_CASE(enforce_account_ram_limit_underflow, resource_limits_fixture) try {
      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, 100, -1, -1 );
      verify_account_ram_usage(account);
      process_account_limit_updates();
      BOOST_REQUIRE_THROW(add_pending_ram_usage(account, -101), transaction_exception);

   } FC_LOG_AND_RETHROW();

   BOOST_FIXTURE_TEST_CASE(enforce_account_ram_limit_overflow, resource_limits_fixture) try {
      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, UINT64_MAX, -1, -1 );
      verify_account_ram_usage(account);
      process_account_limit_updates();
      add_pending_ram_usage(account, UINT64_MAX/2);
      verify_account_ram_usage(account);
      add_pending_ram_usage(account, UINT64_MAX/2);
      verify_account_ram_usage(account);
      BOOST_REQUIRE_THROW(add_pending_ram_usage(account, 2), transaction_exception);

   } FC_LOG_AND_RETHROW();

   BOOST_FIXTURE_TEST_CASE(enforce_account_ram_commitment, resource_limits_fixture) try {
      const int64_t limit = 1000;
      const int64_t commit = 600;
      const int64_t increment = 77;
      const int64_t expected_iterations = (limit - commit + increment - 1 ) / increment;


      const account_name account(1);
      initialize_account(account);
      set_account_limits(account, limit, -1, -1 );
      process_account_limit_updates();
      add_pending_ram_usage(account, commit);
      verify_account_ram_usage(account);

      for (int idx = 0; idx < expected_iterations - 1; idx++) {
         set_account_limits(account, limit - increment * idx, -1, -1);
         verify_account_ram_usage(account);
         process_account_limit_updates();
      }

      set_account_limits(account, limit - increment * expected_iterations, -1, -1);
      BOOST_REQUIRE_THROW(verify_account_ram_usage(account), ram_usage_exceeded);
   } FC_LOG_AND_RETHROW();


   BOOST_FIXTURE_TEST_CASE(sanity_check, resource_limits_fixture) try {
      double total_staked_tokens = 1'000'000'000'0000.;
      double user_stake = 1'0000.;
double max_block_cpu = 100000.; //我们；
      double blocks_per_day = 2*60*60*23;
      double total_cpu_per_period = max_block_cpu * blocks_per_day * 3;

      double congested_cpu_time_per_period = total_cpu_per_period * user_stake / total_staked_tokens;
      wdump((congested_cpu_time_per_period));
      double uncongested_cpu_time_per_period = (1000*total_cpu_per_period) * user_stake / total_staked_tokens;
      wdump((uncongested_cpu_time_per_period));


      initialize_account( N(dan) );
      initialize_account( N(everyone) );
      set_account_limits( N(dan), 0, 0, 10000 );
      set_account_limits( N(everyone), 0, 0, 10000000000000ll );
      process_account_limit_updates();

add_transaction_usage( {N(dan)}, 10, 0, 1 ); ///dan应该可以每3天做10次



   } FC_LOG_AND_RETHROW() 

BOOST_AUTO_TEST_SUITE_END()

