
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457044438552576>

#include <eosio/testing/tester_network.hpp>

namespace eosio { namespace testing {

   void tester_network::connect_blockchain(base_tester &new_blockchain) {
      if (blockchains.count(&new_blockchain))
         return;

//如果网络不是空的，请将新的区块链与旧的区块链同步。旧的已经与同步
//彼此，所以随便拿一个。旧的区块链与传播信号相连，因此
//其中的一个被同步，它也会将块传播给其他的。
      if (!blockchains.empty()) {
         blockchains.begin()->first->sync_with(new_blockchain);
      }

//新的区块链现在与任何旧的区块链同步；继续并连接传播信号。

///todo还原此
      
      /*
      区块链[&new_区块链]=new_区块链.control->applied_block.connect（
              [这个，&new_区块链]（const chain:：block_trace&bt）
                 传播区块（bt.block，新的区块链）；
              （}）；
              **/

   }

   void tester_network::disconnect_blockchain(base_tester &leaving_blockchain) {
      blockchains.erase(&leaving_blockchain);
   }

   void tester_network::disconnect_all() {
      blockchains.clear();
   }

   void tester_network::propagate_block(const signed_block &block, const base_tester &skip_blockchain) {
//用于（const auto&pair:blockchains）
//如果（pair.first=&skip_区块链）继续；
//boost:：signals2:：shared_connection_block blocker（pair.second）；
//pair.first->control->push_block（block，eosio:：chain:：validation_steps:：created_block）；
//}
   }

} } ///eosio：：测试

