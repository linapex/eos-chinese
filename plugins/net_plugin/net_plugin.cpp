
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:18</date>
//</624457052948795392>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#include <eosio/chain/types.hpp>

#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/net_plugin/protocol.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/block.hpp>
#include <eosio/chain/plugin_interface.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/chain/contract_types.hpp>

#include <fc/network/message_buffer.hpp>
#include <fc/network/ip.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/appender.hpp>
#include <fc/reflect/variant.hpp>
#include <fc/crypto/rand.hpp>
#include <fc/exception/exception.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/steady_timer.hpp>

using namespace eosio::chain::plugin_interface::compat;

namespace fc {
   extern std::unordered_map<std::string,logger>& get_logger_map();
}

namespace eosio {
   static appbase::abstract_plugin& _net_plugin = app().register_plugin<net_plugin>();

   using std::vector;

   using boost::asio::ip::tcp;
   using boost::asio::ip::address_v4;
   using boost::asio::ip::host_name;
   using boost::multi_index_container;

   using fc::time_point;
   using fc::time_point_sec;
   using eosio::chain::transaction_id_type;

   class connection;

   class sync_manager;
   class dispatch_manager;

   using connection_ptr = std::shared_ptr<connection>;
   using connection_wptr = std::weak_ptr<connection>;

   using socket_ptr = std::shared_ptr<tcp::socket>;

   struct node_transaction_state {
      transaction_id_type id;
time_point_sec  expires;  ///time之后可以清除。
uint32_t        block_num = 0; ///block事务包含在
std::shared_ptr<vector<char>>   serialized_txn; ///收到的原始包
   };

   struct by_expiry;
   struct by_block_num;

   struct sha256_less {
      bool operator()( const sha256& lhs, const sha256& rhs ) const {
       return
             std::tie(lhs._hash[0], lhs._hash[1], lhs._hash[2], lhs._hash[3]) <
             std::tie(rhs._hash[0], rhs._hash[1], rhs._hash[2], rhs._hash[3]);
      }
   };

   typedef multi_index_container<
      node_transaction_state,
      indexed_by<
         ordered_unique<
            tag< by_id >,
            member < node_transaction_state,
                     transaction_id_type,
                     &node_transaction_state::id >,
            sha256_less >,
         ordered_non_unique<
            tag< by_expiry >,
            member< node_transaction_state,
                    fc::time_point_sec,
                    &node_transaction_state::expires >
            >,
         ordered_non_unique<
            tag<by_block_num>,
            member< node_transaction_state,
                    uint32_t,
                    &node_transaction_state::block_num > >
         >
      >
   node_transaction_index;

   class net_plugin_impl {
   public:
      unique_ptr<tcp::acceptor>        acceptor;
      tcp::endpoint                    listen_endpoint;
      string                           p2p_address;
      uint32_t                         max_client_count = 0;
      uint32_t                         max_nodes_per_host = 1;
      uint32_t                         num_clients = 0;

      vector<string>                   supplied_peers;
vector<chain::public_key_type>   allowed_peers; ///<允许连接的对等密钥
      std::map<chain::public_key_type,
chain::private_key_type> private_keys; ///<与生产者密钥重叠，也对非生产节点进行身份验证

      enum possible_connections : char {
         None = 0,
            Producers = 1 << 0,
            Specified = 1 << 1,
            Any = 1 << 2
            };
      possible_connections             allowed_connections{None};

      connection_ptr find_connection(const string& host)const;

      std::set< connection_ptr >       connections;
      bool                             done = false;
      unique_ptr< sync_manager >       sync_master;
      unique_ptr< dispatch_manager >   dispatcher;

      unique_ptr<boost::asio::steady_timer> connector_check;
      unique_ptr<boost::asio::steady_timer> transaction_check;
      unique_ptr<boost::asio::steady_timer> keepalive_timer;
      boost::asio::steady_timer::duration   connector_period;
      boost::asio::steady_timer::duration   txn_exp_period;
      boost::asio::steady_timer::duration   resp_expected_period;
      boost::asio::steady_timer::duration   keepalive_interval{std::chrono::seconds{32}};
      int                           max_cleanup_time_ms = 0;

const std::chrono::system_clock::duration peer_authentication_interval{std::chrono::seconds{1}}; ///<对等时钟与我们的时钟的偏差可能不超过1秒，包括网络延迟。

      bool                          network_version_match = false;
      chain_id_type                 chain_id;
      fc::sha256                    node_id;

      string                        user_agent_name;
      chain_plugin*                 chain_plug = nullptr;
      producer_plugin*              producer_plug = nullptr;
      int                           started_sessions = 0;

      node_transaction_index        local_txns;

      shared_ptr<tcp::resolver>     resolver;

      bool                          use_socket_read_watermark = false;

      channels::transaction_ack::channel_type::handle  incoming_transaction_ack_subscription;

      void connect(const connection_ptr& c);
      void connect(const connection_ptr& c, tcp::resolver::iterator endpoint_itr);
      bool start_session(const connection_ptr& c);
      void start_listen_loop();
      void start_read_message(const connection_ptr& c);

      void close(const connection_ptr& c);
      size_t count_open_sockets() const;

      template<typename VerifierFunc>
      void send_all( const std::shared_ptr<std::vector<char>>& send_buffer, VerifierFunc verify );

      void accepted_block(const block_state_ptr&);
      void transaction_ack(const std::pair<fc::exception_ptr, transaction_metadata_ptr>&);

      bool is_valid( const handshake_message &msg);

      void handle_message(const connection_ptr& c, const handshake_message& msg);
      void handle_message(const connection_ptr& c, const chain_size_message& msg);
      void handle_message(const connection_ptr& c, const go_away_message& msg );
      /*\name对等时间戳
       *时间信息处理
       *@
       **/

      /*\简要处理时间\u消息
       *
       *计算偏移、延迟和分散。注意
       *隐含处理。一阶差已完成
       *直接采用64位运算，然后对结果进行转换
       *浮动双精度。所有进一步的处理都在进行中
       *由硬件完成的带舍入的浮动双精度运算。
       *为了避免溢出和保持精度，这是必要的。
       **/

      void handle_message(const connection_ptr& c, const time_message& msg);
      /*@ }*/
      void handle_message(const connection_ptr& c, const notice_message& msg);
      void handle_message(const connection_ptr& c, const request_message& msg);
      void handle_message(const connection_ptr& c, const sync_request_message& msg);
void handle_message(const connection_ptr& c, const signed_block& msg) = delete; //改用有符号块指针重载
      void handle_message(const connection_ptr& c, const signed_block_ptr& msg);
void handle_message(const connection_ptr& c, const packed_transaction& msg) = delete; //使用packed_transaction_ptr重载
      void handle_message(const connection_ptr& c, const packed_transaction_ptr& msg);

      void start_conn_timer(boost::asio::steady_timer::duration du, std::weak_ptr<connection> from_connection);
      void start_txn_timer();
      void start_monitors();

      void expire_txns();
      void expire_local_txns();
      void connection_monitor(std::weak_ptr<connection> from_connection);
      /*\name对等时间戳
       *时间信息处理
       *@
       **/

      /*\brief peer heartbeat ticker。
       **/

      void ticker();
      /*@ }*/
      /*\brief确定是否允许对等机连接。
       *
       *检查当前连接模式和密钥验证。
       *
       *\如果对等端不应连接，则返回false，否则返回true。
       **/

      bool authenticate_peer(const handshake_message& msg) const;
      /*\brief检索用于与对等方进行身份验证的公钥。
       *
       *查找用于身份验证的密钥。如果此节点是生产者，请使用
       *生产者关键地图的前面。如果节点不是生产者，但具有
       *配置的私钥，使用它。如果节点既不是生产者也不是
       *私钥，返回空密钥。
       *
       *\注意，在配置了多个私钥的节点上，该密钥
       *始终使用数字较小的字节。
       **/

      chain::public_key_type get_authentication_key() const;
      /*\brief使用签名者的相应私钥返回摘要的签名。
       *
       *如果没有配置的私钥，则返回空签名。
       **/

      chain::signature_type sign_compact(const chain::public_key_type& signer, const fc::sha256& digest) const;

      uint16_t to_protocol_version(uint16_t v);
   };

   const fc::string logger_name("net_plugin_impl");
   fc::logger logger;
   std::string peer_log_format;

#define peer_dlog( PEER, FORMAT, ... ) \
  FC_MULTILINE_MACRO_BEGIN \
   if( logger.is_enabled( fc::log_level::debug ) ) \
      logger.log( FC_LOG_MESSAGE( debug, peer_log_format + FORMAT, __VA_ARGS__ (PEER->get_logger_variant()) ) ); \
  FC_MULTILINE_MACRO_END

#define peer_ilog( PEER, FORMAT, ... ) \
  FC_MULTILINE_MACRO_BEGIN \
   if( logger.is_enabled( fc::log_level::info ) ) \
      logger.log( FC_LOG_MESSAGE( info, peer_log_format + FORMAT, __VA_ARGS__ (PEER->get_logger_variant()) ) ); \
  FC_MULTILINE_MACRO_END

#define peer_wlog( PEER, FORMAT, ... ) \
  FC_MULTILINE_MACRO_BEGIN \
   if( logger.is_enabled( fc::log_level::warn ) ) \
      logger.log( FC_LOG_MESSAGE( warn, peer_log_format + FORMAT, __VA_ARGS__ (PEER->get_logger_variant()) ) ); \
  FC_MULTILINE_MACRO_END

#define peer_elog( PEER, FORMAT, ... ) \
  FC_MULTILINE_MACRO_BEGIN \
   if( logger.is_enabled( fc::log_level::error ) ) \
      logger.log( FC_LOG_MESSAGE( error, peer_log_format + FORMAT, __VA_ARGS__ (PEER->get_logger_variant())) ); \
  FC_MULTILINE_MACRO_END


   template<class enum_type, class=typename std::enable_if<std::is_enum<enum_type>::value>::type>
   inline enum_type& operator|=(enum_type& lhs, const enum_type& rhs)
   {
      using T = std::underlying_type_t <enum_type>;
      return lhs = static_cast<enum_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
   }

   static net_plugin_impl *my_impl;

   /*
    *默认值初始值设定项
    **/

   constexpr auto     def_send_buffer_size_mb = 4;
   constexpr auto     def_send_buffer_size = 1024*1024*def_send_buffer_size_mb;
constexpr auto     def_max_clients = 25; //0代表无限客户
   constexpr auto     def_max_nodes_per_host = 1;
   constexpr auto     def_conn_retry_wait = 30;
   constexpr auto     def_txn_expire_wait = std::chrono::seconds(3);
   constexpr auto     def_resp_expected_wait = std::chrono::seconds(5);
   constexpr auto     def_sync_fetch_span = 100;

   constexpr auto     message_header_size = 4;

   /*
    *有一段时间，网络版本是一个16位值，等于第二组16位。
    *当前版本的git commit id。我们现在将其替换为整数协议。
    *标识符。基于对所有Git提交标识符的历史分析，存在较大的差距
    *在Ajacent提交ID值之间显示如下。
    *在主分支上通过以下命令找到这些号码：
    *
    *Git日志grep“^提交”awk“打印子目录（$2,5,4）”sort-u>sorted.txt
    *rm-f gap.txt；prev=0；对于in$（cat sorted.txt）；do echo$prev$（（0x$A-0x$prev））$A>>gap.txt；prev=$A；done；sort-k2-n gap.txt_tail
    *
    *请勿编辑网络版或网络版范围！
    **/

   constexpr uint16_t net_version_base = 0x04b5;
   constexpr uint16_t net_version_range = 106;
   /*
    *如果网络协议或行为发生变化，请增加网络版本以识别
    *对兼容性挂钩的需求
    **/

   constexpr uint16_t proto_base = 0;
   constexpr uint16_t proto_explicit_sync = 1;

   constexpr uint16_t net_version = proto_explicit_sync;

   struct transaction_state {
      transaction_id_type id;
uint32_t            block_num = 0; ///<包含该事务的块编号
      time_point_sec      expires;
   };

   typedef multi_index_container<
      transaction_state,
      indexed_by<
         ordered_unique< tag<by_id>, member<transaction_state, transaction_id_type, &transaction_state::id >, sha256_less >,
         ordered_non_unique< tag< by_expiry >, member< transaction_state,fc::time_point_sec,&transaction_state::expires >>,
         ordered_non_unique<
            tag<by_block_num>,
            member< transaction_state,
                    uint32_t,
                    &transaction_state::block_num > >
         >

      > transaction_state_index;

   /*
    *
    **/

   struct peer_block_state {
      block_id_type id;
      uint32_t      block_num;
      bool          is_known;
      bool          is_noticed;
      time_point    requested_time;
   };

   struct update_request_time {
      void operator() (struct eosio::peer_block_state &bs) {
         bs.requested_time = time_point::now();
      }
   } set_request_time;

   typedef multi_index_container<
      eosio::peer_block_state,
      indexed_by<
         ordered_unique< tag<by_id>, member<eosio::peer_block_state, block_id_type, &eosio::peer_block_state::id >, sha256_less >,
         ordered_unique< tag<by_block_num>, member<eosio::peer_block_state, uint32_t, &eosio::peer_block_state::block_num > >
         >
      > peer_block_state_index;


   struct update_known_by_peer {
      void operator() (eosio::peer_block_state& bs) {
         bs.is_known = true;
      }
   } set_is_known;


   struct update_block_num {
      uint32_t new_bnum;
      update_block_num(uint32_t bnum) : new_bnum(bnum) {}
      void operator() (node_transaction_state& nts) {
         nts.block_num = new_bnum;
      }
      void operator() (transaction_state& ts) {
         ts.block_num = new_bnum;
      }
      void operator() (peer_block_state& pbs) {
         pbs.block_num = new_bnum;
      }
   };

   /*
    *按起始块编号索引
    **/

   struct sync_state {
      sync_state(uint32_t start = 0, uint32_t end = 0, uint32_t last_acted = 0)
         :start_block( start ), end_block( end ), last( last_acted ),
          start_time(time_point::now())
      {}
      uint32_t     start_block;
      uint32_t     end_block;
uint32_t     last; ///<上次发送或接收
time_point   start_time; ///<发出或接收请求的时间
   };

   struct handshake_initializer {
      static void populate(handshake_message &hello);
   };

   class connection : public std::enable_shared_from_this<connection> {
   public:
      explicit connection( string endpoint );

      explicit connection( socket_ptr s );
      ~connection();
      void initialize();

      peer_block_state_index  blk_state;
      transaction_state_index trx_state;
optional<sync_state>    peer_requested;  //这个同伴正在向我们请求信息
      socket_ptr              socket;

      fc::message_buffer<1024*1024>    pending_message_buffer;
      fc::optional<std::size_t>        outstanding_read_bytes;

      struct queued_write {
         std::shared_ptr<vector<char>> buff;
         std::function<void(boost::system::error_code, std::size_t)> callback;
      };
      deque<queued_write>     write_queue;
      deque<queued_write>     out_queue;
      fc::sha256              node_id;
      handshake_message       last_handshake_recv;
      handshake_message       last_handshake_sent;
      int16_t                 sent_handshake_count = 0;
      bool                    connecting = false;
      bool                    syncing = false;
      uint16_t                protocol_version  = 0;
      string                  peer_addr;
      unique_ptr<boost::asio::steady_timer> response_expected;
      optional<request_message> pending_fetch;
      go_away_reason         no_retry = no_reason;
      block_id_type          fork_head;
      uint32_t               fork_head_num = 0;
      optional<request_message> last_req;

      connection_status get_status()const {
         connection_status stat;
         stat.peer = peer_addr;
         stat.connecting = connecting;
         stat.syncing = syncing;
         stat.last_handshake = last_handshake_recv;
         return stat;
      }

      /*\name对等时间戳
       *时间信息处理
       *@
       **/

//从网络数据设置的成员
tstamp                         org{0};          //！<起始时间戳
tstamp                         rec{0};          //！<接收时间戳
tstamp                         dst{0};          //！<目标时间戳
tstamp                         xmt{0};          //！<传输时间戳

//计算数据
double                         offset{0};       //！<对等偏移

      static const size_t            ts_buffer_size{32};
char                           ts[ts_buffer_size];          //！<使人类可读时间戳的工作缓冲区
      /*@ }*/

      bool connected();
      bool current();
      void reset();
      void close();
      void send_handshake();

      /*\name对等时间戳
       *时间信息处理
       **/

      /*\简短的填充和排队时间\u消息
       **/

      void send_time();
      /*\brief立即使用传入时间消息填充和排队时间消息
       **/

      void send_time(const time_message& msg);
      /*\brief读取系统时间并转换为64位整数。
       *
       *程序中此例程只有两个调用。一
       *当一个数据包从网络到达另一个数据包时，
       *数据包放在发送队列上。调用的内核时间
       *day例程并转换为（至少）64位整数。
       **/

      tstamp get_time()
      {
         return std::chrono::system_clock::now().time_since_epoch().count();
      }
      /*@ }*/

      const string peer_name();

      void txn_send_pending(const vector<transaction_id_type>& ids);
      void txn_send(const vector<transaction_id_type>& txn_lis);

      void blk_send_branch();
      void blk_send(const vector<block_id_type> &txn_lis);
      void stop_send();

      void enqueue( const net_message &msg, bool trigger_send = true );
      void enqueue_block( const signed_block_ptr& sb, bool trigger_send = true );
      void enqueue_buffer( const std::shared_ptr<std::vector<char>>& send_buffer, bool trigger_send, go_away_reason close_after_send );
      void cancel_sync(go_away_reason);
      void flush_queues();
      bool enqueue_sync_block();
      void request_sync_blocks(uint32_t start, uint32_t end);

      void cancel_wait();
      void sync_wait();
      void fetch_wait();
      void sync_timeout(boost::system::error_code ec);
      void fetch_timeout(boost::system::error_code ec);

      void queue_write(const std::shared_ptr<vector<char>>& buff,
                       bool trigger_send,
                       std::function<void(boost::system::error_code, std::size_t)> callback);
      void do_queue_write();

      /*\brief处理来自挂起消息缓冲区的下一条消息
       *
       *处理来自待处理消息缓冲区的下一条消息。
       *消息长度是已经确定的数据长度
       *网络插件实现中的部分消息和impl
       *这将处理消息。
       *返回true成功。如果错误为
       *解包或处理邮件时遇到问题。
       **/

      bool process_next_message(net_plugin_impl& impl, uint32_t message_length);

      bool add_peer_block(const peer_block_state &pbs);

      fc::optional<fc::variant_object> _logger_variant;
      const fc::variant_object& get_logger_variant()  {
         if (!_logger_variant) {
            boost::system::error_code ec;
            auto rep = socket->remote_endpoint(ec);
            string ip = ec ? "<unknown>" : rep.address().to_string();
            string port = ec ? "<unknown>" : std::to_string(rep.port());

            auto lep = socket->local_endpoint(ec);
            string lip = ec ? "<unknown>" : lep.address().to_string();
            string lport = ec ? "<unknown>" : std::to_string(lep.port());

            _logger_variant.emplace(fc::mutable_variant_object()
               ("_name", peer_name())
               ("_id", node_id)
               ("_sid", ((string)node_id).substr(0, 7))
               ("_ip", ip)
               ("_port", port)
               ("_lip", lip)
               ("_lport", lport)
            );
         }
         return *_logger_variant;
      }
   };

   struct msg_handler : public fc::visitor<void> {
      net_plugin_impl &impl;
      connection_ptr c;
      msg_handler( net_plugin_impl &imp, const connection_ptr& conn) : impl(imp), c(conn) {}

      void operator()( const signed_block& msg ) const {
         EOS_ASSERT( false, plugin_config_exception, "operator()(signed_block&&) should be called" );
      }
      void operator()( signed_block& msg ) const {
         EOS_ASSERT( false, plugin_config_exception, "operator()(signed_block&&) should be called" );
      }
      void operator()( const packed_transaction& msg ) const {
         EOS_ASSERT( false, plugin_config_exception, "operator()(packed_transaction&&) should be called" );
      }
      void operator()( packed_transaction& msg ) const {
         EOS_ASSERT( false, plugin_config_exception, "operator()(packed_transaction&&) should be called" );
      }

      void operator()( signed_block&& msg ) const {
         impl.handle_message( c, std::make_shared<signed_block>( std::move( msg ) ) );
      }
      void operator()( packed_transaction&& msg ) const {
         impl.handle_message( c, std::make_shared<packed_transaction>( std::move( msg ) ) );
      }

      template <typename T>
      void operator()( T&& msg ) const
      {
         impl.handle_message( c, std::forward<T>(msg) );
      }
   };

   class sync_manager {
   private:
      enum stages {
         lib_catchup,
         head_catchup,
         in_sync
      };

      uint32_t       sync_known_lib_num;
      uint32_t       sync_last_requested_num;
      uint32_t       sync_next_expected_num;
      uint32_t       sync_req_span;
      connection_ptr source;
      stages         state;

      chain_plugin* chain_plug = nullptr;

      constexpr auto stage_str(stages s );

   public:
      explicit sync_manager(uint32_t span);
      void set_state(stages s);
      bool sync_required();
      void send_handshakes();
      bool is_active(const connection_ptr& conn);
      void reset_lib_num(const connection_ptr& conn);
      void request_next_chunk(const connection_ptr& conn = connection_ptr());
      void start_sync(const connection_ptr& c, uint32_t target);
      void reassign_fetch(const connection_ptr& c, go_away_reason reason);
      void verify_catchup(const connection_ptr& c, uint32_t num, const block_id_type& id);
      void rejected_block(const connection_ptr& c, uint32_t blk_num);
      void recv_block(const connection_ptr& c, const block_id_type& blk_id, uint32_t blk_num);
      void recv_handshake(const connection_ptr& c, const handshake_message& msg);
      void recv_notice(const connection_ptr& c, const notice_message& msg);
   };

   class dispatch_manager {
   public:
      std::multimap<block_id_type, connection_ptr, sha256_less> received_blocks;
      std::multimap<transaction_id_type, connection_ptr, sha256_less> received_transactions;

      void bcast_transaction(const transaction_metadata_ptr& trx);
      void rejected_transaction(const transaction_id_type& msg);
      void bcast_block(const block_state_ptr& bs);
      void rejected_block(const block_id_type& id);

      void recv_block(const connection_ptr& conn, const block_id_type& msg, uint32_t bnum);
      void recv_transaction(const connection_ptr& conn, const transaction_id_type& id);
      void recv_notice(const connection_ptr& conn, const notice_message& msg, bool generated);

      void retry_fetch(const connection_ptr& conn);
   };

//————————————————————————————————————————————————————————————————————

   connection::connection( string endpoint )
      : blk_state(),
        trx_state(),
        peer_requested(),
        socket( std::make_shared<tcp::socket>( std::ref( app().get_io_service() ))),
        node_id(),
        last_handshake_recv(),
        last_handshake_sent(),
        sent_handshake_count(0),
        connecting(false),
        syncing(false),
        protocol_version(0),
        peer_addr(endpoint),
        response_expected(),
        pending_fetch(),
        no_retry(no_reason),
        fork_head(),
        fork_head_num(0),
        last_req()
   {
      wlog( "created connection to ${n}", ("n", endpoint) );
      initialize();
   }

   connection::connection( socket_ptr s )
      : blk_state(),
        trx_state(),
        peer_requested(),
        socket( s ),
        node_id(),
        last_handshake_recv(),
        last_handshake_sent(),
        sent_handshake_count(0),
        connecting(true),
        syncing(false),
        protocol_version(0),
        peer_addr(),
        response_expected(),
        pending_fetch(),
        no_retry(no_reason),
        fork_head(),
        fork_head_num(0),
        last_req()
   {
      wlog( "accepted network connection" );
      initialize();
   }

   connection::~connection() {}

   void connection::initialize() {
      auto *rnd = node_id.data();
      rnd[0] = 0;
      response_expected.reset(new boost::asio::steady_timer(app().get_io_service()));
   }

   bool connection::connected() {
      return (socket && socket->is_open() && !connecting);
   }

   bool connection::current() {
      return (connected() && !syncing);
   }

   void connection::reset() {
      peer_requested.reset();
      blk_state.clear();
      trx_state.clear();
   }

   void connection::flush_queues() {
      write_queue.clear();
   }

   void connection::close() {
      if(socket) {
         socket->close();
      }
      else {
         wlog("no socket to close!");
      }
      flush_queues();
      connecting = false;
      syncing = false;
      if( last_req ) {
         my_impl->dispatcher->retry_fetch(shared_from_this());
      }
      reset();
      sent_handshake_count = 0;
      last_handshake_recv = handshake_message();
      last_handshake_sent = handshake_message();
      my_impl->sync_master->reset_lib_num(shared_from_this());
      fc_dlog(logger, "canceling wait on ${p}", ("p",peer_name()));
      cancel_wait();
      pending_message_buffer.reset();
   }

   void connection::txn_send_pending(const vector<transaction_id_type>& ids) {
      const std::set<transaction_id_type, sha256_less> known_ids(ids.cbegin(), ids.cend());
      my_impl->expire_local_txns();
      for(auto tx = my_impl->local_txns.begin(); tx != my_impl->local_txns.end(); ++tx ){
         const bool found = known_ids.find( tx->id ) != known_ids.cend();
         if( !found ) {
            queue_write( tx->serialized_txn, true, []( boost::system::error_code ec, std::size_t ) {} );
         }
      }
   }

   void connection::txn_send(const vector<transaction_id_type>& ids) {
      for(const auto& t : ids) {
         auto tx = my_impl->local_txns.get<by_id>().find(t);
         if( tx != my_impl->local_txns.end() ) {
            queue_write( tx->serialized_txn, true, []( boost::system::error_code ec, std::size_t ) {} );
         }
      }
   }

   void connection::blk_send_branch() {
      controller& cc = my_impl->chain_plug->chain();
      uint32_t head_num = cc.fork_db_head_block_num();
      notice_message note;
      note.known_blocks.mode = normal;
      note.known_blocks.pending = 0;
      fc_dlog(logger, "head_num = ${h}",("h",head_num));
      if(head_num == 0) {
         enqueue(note);
         return;
      }
      block_id_type head_id;
      block_id_type lib_id;
      block_id_type remote_head_id;
      uint32_t remote_head_num = 0;
      try {
         if (last_handshake_recv.generation >= 1) {
            remote_head_id = last_handshake_recv.head_id;
            remote_head_num = block_header::num_from_id(remote_head_id);
            fc_dlog(logger, "maybe truncating branch at  = ${h}:${id}",("h",remote_head_num)("id",remote_head_id));
         }

//根据上次握手的分支，我们发送的是对等机，而不是当前的
//当数据包在飞行中时，lib可以及时向前移动。
         if (last_handshake_sent.generation >= 1) {
            lib_id = last_handshake_sent.last_irreversible_block_id;
         } else {
            lib_id = cc.last_irreversible_block_id();
         }
         head_id = cc.fork_db_head_block_id();
      }
      catch (const assert_exception& ex) {
         elog( "unable to retrieve block info: ${n} for ${p}",("n",ex.to_string())("p",peer_name()));
         enqueue(note);
         return;
      }
      catch (const fc::exception& ex) {
      }
      catch (...) {
      }

      vector<signed_block_ptr> bstack;
      block_id_type null_id;
      for (auto bid = head_id; bid != null_id && bid != lib_id; ) {
         try {

//如果收到的最后一次握手表明我们正在追赶一个叉子
//对等方已经部分意识到的，不需要重新发送块
            if (remote_head_id == bid) {
               break;
            }

            signed_block_ptr b = cc.fetch_block_by_id(bid);
            if ( b ) {
               bid = b->previous;
               bstack.push_back(b);
            }
            else {
               break;
            }
         } catch (...) {
            break;
         }
      }
      size_t count = 0;
      if (!bstack.empty()) {
         if (bstack.back()->previous == lib_id || bstack.back()->previous == remote_head_id) {
            count = bstack.size();
            while (bstack.size()) {
               enqueue_block( bstack.back() );
               bstack.pop_back();
            }
         }
         fc_ilog(logger, "Sent ${n} blocks on my fork",("n",count));
      } else {
         fc_ilog(logger, "Nothing to send on fork request");
      }

      syncing = false;
   }

   void connection::blk_send(const vector<block_id_type> &ids) {
      controller &cc = my_impl->chain_plug->chain();
      int count = 0;
      for(auto &blkid : ids) {
         ++count;
         try {
            signed_block_ptr b = cc.fetch_block_by_id(blkid);
            if(b) {
               fc_dlog(logger,"found block for id at num ${n}",("n",b->block_num()));
               enqueue_block( b );
            }
            else {
               ilog("fetch block by id returned null, id ${id} on block ${c} of ${s} for ${p}",
                     ("id",blkid)("c",count)("s",ids.size())("p",peer_name()));
               break;
            }
         }
         catch (const assert_exception &ex) {
            elog( "caught assert on fetch_block_by_id, ${ex}, id ${id} on block ${c} of ${s} for ${p}",
                  ("ex",ex.to_string())("id",blkid)("c",count)("s",ids.size())("p",peer_name()));
            break;
         }
         catch (...) {
            elog( "caught othser exception fetching block id ${id} on block ${c} of ${s} for ${p}",
                  ("id",blkid)("c",count)("s",ids.size())("p",peer_name()));
            break;
         }
      }

   }

   void connection::stop_send() {
      syncing = false;
   }

   void connection::send_handshake() {
      handshake_initializer::populate(last_handshake_sent);
      last_handshake_sent.generation = ++sent_handshake_count;
      fc_dlog(logger, "Sending handshake generation ${g} to ${ep}",
              ("g",last_handshake_sent.generation)("ep", peer_name()));
      enqueue(last_handshake_sent);
   }

   void connection::send_time() {
      time_message xpkt;
      xpkt.org = rec;
      xpkt.rec = dst;
      xpkt.xmt = get_time();
      org = xpkt.xmt;
      enqueue(xpkt);
   }

   void connection::send_time(const time_message& msg) {
      time_message xpkt;
      xpkt.org = msg.xmt;
      xpkt.rec = msg.dst;
      xpkt.xmt = get_time();
      enqueue(xpkt);
   }

   void connection::queue_write(const std::shared_ptr<vector<char>>& buff,
                                bool trigger_send,
                                std::function<void(boost::system::error_code, std::size_t)> callback) {
      write_queue.push_back({buff, callback});
      if(out_queue.empty() && trigger_send)
         do_queue_write();
   }

   void connection::do_queue_write() {
      if(write_queue.empty() || !out_queue.empty())
         return;
      connection_wptr c(shared_from_this());
      if(!socket->is_open()) {
         fc_elog(logger,"socket not open to ${p}",("p",peer_name()));
         my_impl->close(c.lock());
         return;
      }
      std::vector<boost::asio::const_buffer> bufs;
      while (write_queue.size() > 0) {
         auto& m = write_queue.front();
         bufs.push_back(boost::asio::buffer(*m.buff));
         out_queue.push_back(m);
         write_queue.pop_front();
      }
      boost::asio::async_write(*socket, bufs, [c](boost::system::error_code ec, std::size_t w) {
            try {
               auto conn = c.lock();
               if(!conn)
                  return;

               for (auto& m: conn->out_queue) {
                  m.callback(ec, w);
               }

               if(ec) {
                  string pname = conn ? conn->peer_name() : "no connection name";
                  if( ec.value() != boost::asio::error::eof) {
                     elog("Error sending to peer ${p}: ${i}", ("p",pname)("i", ec.message()));
                  }
                  else {
                     ilog("connection closure detected on write to ${p}",("p",pname));
                  }
                  my_impl->close(conn);
                  return;
               }
               while (conn->out_queue.size() > 0) {
                  conn->out_queue.pop_front();
               }
               conn->enqueue_sync_block();
               conn->do_queue_write();
            }
            catch(const std::exception &ex) {
               auto conn = c.lock();
               string pname = conn ? conn->peer_name() : "no connection name";
               elog("Exception in do_queue_write to ${p} ${s}", ("p",pname)("s",ex.what()));
            }
            catch(const fc::exception &ex) {
               auto conn = c.lock();
               string pname = conn ? conn->peer_name() : "no connection name";
               elog("Exception in do_queue_write to ${p} ${s}", ("p",pname)("s",ex.to_string()));
            }
            catch(...) {
               auto conn = c.lock();
               string pname = conn ? conn->peer_name() : "no connection name";
               elog("Exception in do_queue_write to ${p}", ("p",pname) );
            }
         });
   }

   void connection::cancel_sync(go_away_reason reason) {
      fc_dlog(logger,"cancel sync reason = ${m}, write queue size ${o} peer ${p}",
              ("m",reason_str(reason)) ("o", write_queue.size())("p", peer_name()));
      cancel_wait();
      flush_queues();
      switch (reason) {
      case validation :
      case fatal_other : {
         no_retry = reason;
         enqueue( go_away_message( reason ));
         break;
      }
      default:
         fc_dlog(logger, "sending empty request but not calling sync wait on ${p}", ("p",peer_name()));
         enqueue( ( sync_request_message ) {0,0} );
      }
   }

   bool connection::enqueue_sync_block() {
      if (!peer_requested)
         return false;
      uint32_t num = ++peer_requested->last;
      bool trigger_send = num == peer_requested->start_block;
      if(num == peer_requested->end_block) {
         peer_requested.reset();
      }
      try {
         controller& cc = my_impl->chain_plug->chain();
         signed_block_ptr sb = cc.fetch_block_by_number(num);
         if(sb) {
            enqueue_block( sb, trigger_send);
            return true;
         }
      } catch ( ... ) {
         wlog( "write loop exception" );
      }
      return false;
   }

   void connection::enqueue( const net_message& m, bool trigger_send ) {
      go_away_reason close_after_send = no_reason;
      if (m.contains<go_away_message>()) {
         close_after_send = m.get<go_away_message>().reason;
      }

      uint32_t payload_size = fc::raw::pack_size( m );

      char* header = reinterpret_cast<char*>(&payload_size);
      size_t header_size = sizeof(payload_size);
      size_t buffer_size = header_size + payload_size;

      auto send_buffer = std::make_shared<vector<char>>(buffer_size);
      fc::datastream<char*> ds( send_buffer->data(), buffer_size);
      ds.write( header, header_size );
      fc::raw::pack( ds, m );

      enqueue_buffer( send_buffer, trigger_send, close_after_send );
   }

   void connection::enqueue_block( const signed_block_ptr& sb, bool trigger_send ) {
//此实现是为了避免将已签名的\块复制到网络\消息
int which = 7; //匹配已签名\块的网络\消息

      uint32_t which_size = fc::raw::pack_size( unsigned_int( which ));
      uint32_t payload_size = which_size + fc::raw::pack_size( *sb );

      char* header = reinterpret_cast<char*>(&payload_size);
      size_t header_size = sizeof(payload_size);
      size_t buffer_size = header_size + payload_size;

      auto send_buffer = std::make_shared<vector<char>>(buffer_size);
      fc::datastream<char*> ds( send_buffer->data(), buffer_size);
      ds.write( header, header_size );
      fc::raw::pack( ds, unsigned_int( which ));
      fc::raw::pack( ds, *sb );

      enqueue_buffer( send_buffer, trigger_send, no_reason );
   }

   void connection::enqueue_buffer( const std::shared_ptr<std::vector<char>>& send_buffer, bool trigger_send, go_away_reason close_after_send ) {
      connection_wptr weak_this = shared_from_this();
      queue_write(send_buffer,trigger_send,
                  [weak_this, close_after_send](boost::system::error_code ec, std::size_t ) {
                     connection_ptr conn = weak_this.lock();
                     if (conn) {
                        if (close_after_send != no_reason) {
                           elog ("sent a go away message: ${r}, closing connection to ${p}",("r", reason_str(close_after_send))("p", conn->peer_name()));
                           my_impl->close(conn);
                           return;
                        }
                     } else {
                        fc_wlog(logger, "connection expired before enqueued net_message called callback!");
                     }
                  });
   }

   void connection::cancel_wait() {
      if (response_expected)
         response_expected->cancel();
   }

   void connection::sync_wait() {
      response_expected->expires_from_now( my_impl->resp_expected_period);
      connection_wptr c(shared_from_this());
      response_expected->async_wait( [c]( boost::system::error_code ec){
            connection_ptr conn = c.lock();
            if (!conn) {
//在传递此lambda之前，连接被破坏
               return;
            }

            conn->sync_timeout(ec);
         } );
   }

   void connection::fetch_wait() {
      response_expected->expires_from_now( my_impl->resp_expected_period);
      connection_wptr c(shared_from_this());
      response_expected->async_wait( [c]( boost::system::error_code ec){
            connection_ptr conn = c.lock();
            if (!conn) {
//在传递此lambda之前，连接被破坏
               return;
            }

            conn->fetch_timeout(ec);
         } );
   }

   void connection::sync_timeout( boost::system::error_code ec ) {
      if( !ec ) {
         my_impl->sync_master->reassign_fetch(shared_from_this(), benign_other);
      }
      else if( ec == boost::asio::error::operation_aborted) {
      }
      else {
         elog("setting timer for sync request got error ${ec}",("ec", ec.message()));
      }
   }

   const string connection::peer_name() {
      if( !last_handshake_recv.p2p_address.empty() ) {
         return last_handshake_recv.p2p_address;
      }
      if( !peer_addr.empty() ) {
         return peer_addr;
      }
      return "connecting client";
   }

   void connection::fetch_timeout( boost::system::error_code ec ) {
      if( !ec ) {
         if( pending_fetch.valid() && !( pending_fetch->req_trx.empty() || pending_fetch->req_blocks.empty() ) ) {
            my_impl->dispatcher->retry_fetch(shared_from_this());
         }
      }
      else if( ec == boost::asio::error::operation_aborted ) {
         if( !connected() ) {
            fc_dlog(logger, "fetch timeout was cancelled due to dead connection");
         }
      }
      else {
         elog( "setting timer for fetch request got error ${ec}", ("ec", ec.message() ) );
      }
   }

   void connection::request_sync_blocks(uint32_t start, uint32_t end) {
      sync_request_message srm = {start,end};
      enqueue( net_message(srm));
      sync_wait();
   }

   bool connection::process_next_message(net_plugin_impl& impl, uint32_t message_length) {
      try {
         auto ds = pending_message_buffer.create_datastream();
         net_message msg;
         fc::raw::unpack(ds, msg);
         msg_handler m(impl, shared_from_this() );
         if( msg.contains<signed_block>() ) {
            m( std::move( msg.get<signed_block>() ) );
         } else if( msg.contains<packed_transaction>() ) {
            m( std::move( msg.get<packed_transaction>() ) );
         } else {
            msg.visit( m );
         }
      } catch(  const fc::exception& e ) {
         edump((e.to_detail_string() ));
         impl.close( shared_from_this() );
         return false;
      }
      return true;
   }

   bool connection::add_peer_block(const peer_block_state &entry) {
      auto bptr = blk_state.get<by_id>().find(entry.id);
      bool added = (bptr == blk_state.end());
      if (added){
         blk_state.insert(entry);
      }
      else {
         blk_state.modify(bptr,set_is_known);
         if (entry.block_num == 0) {
            blk_state.modify(bptr,update_block_num(entry.block_num));
         }
         else {
            blk_state.modify(bptr,set_request_time);
         }
      }
      return added;
   }

//——————————————————————————————————————————————————————————————————

    sync_manager::sync_manager( uint32_t req_span )
      :sync_known_lib_num( 0 )
      ,sync_last_requested_num( 0 )
      ,sync_next_expected_num( 1 )
      ,sync_req_span( req_span )
      ,source()
      ,state(in_sync)
   {
      chain_plug = app().find_plugin<chain_plugin>();
      EOS_ASSERT( chain_plug, chain::missing_chain_plugin_exception, ""  );
   }

   constexpr auto sync_manager::stage_str(stages s) {
    switch (s) {
    case in_sync : return "in sync";
    case lib_catchup: return "lib catchup";
    case head_catchup : return "head catchup";
    default : return "unkown";
    }
  }

   void sync_manager::set_state(stages newstate) {
      if (state == newstate) {
         return;
      }
      fc_dlog(logger, "old state ${os} becoming ${ns}",("os",stage_str(state))("ns",stage_str(newstate)));
      state = newstate;
   }

   bool sync_manager::is_active(const connection_ptr& c) {
      if (state == head_catchup && c) {
         bool fhset = c->fork_head != block_id_type();
         fc_dlog(logger, "fork_head_num = ${fn} fork_head set = ${s}",
                 ("fn", c->fork_head_num)("s", fhset));
            return c->fork_head != block_id_type() && c->fork_head_num < chain_plug->chain().fork_db_head_block_num();
      }
      return state != in_sync;
   }

   void sync_manager::reset_lib_num(const connection_ptr& c) {
      if(state == in_sync) {
         source.reset();
      }
      if( c->current() ) {
         if( c->last_handshake_recv.last_irreversible_block_num > sync_known_lib_num) {
            sync_known_lib_num =c->last_handshake_recv.last_irreversible_block_num;
         }
      } else if( c == source ) {
         sync_last_requested_num = 0;
         request_next_chunk();
      }
   }

   bool sync_manager::sync_required() {
      fc_dlog(logger, "last req = ${req}, last recv = ${recv} known = ${known} our head = ${head}",
              ("req",sync_last_requested_num)("recv",sync_next_expected_num)("known",sync_known_lib_num)("head",chain_plug->chain().fork_db_head_block_num()));

      return( sync_last_requested_num < sync_known_lib_num ||
              chain_plug->chain().fork_db_head_block_num() < sync_last_requested_num );
   }

   void sync_manager::request_next_chunk( const connection_ptr& conn ) {
      uint32_t head_block = chain_plug->chain().fork_db_head_block_num();

      if (head_block < sync_last_requested_num && source && source->current()) {
         fc_ilog(logger, "ignoring request, head is ${h} last req = ${r} source is ${p}",
                  ("h",head_block)("r",sync_last_requested_num)("p",source->peer_name()));
         return;
      }

      /*----------
       *下一个块提供程序选择条件
       *提供并能够使用供应商，请使用它。
       *否则，从列表中选择下一个可用的循环样式。
       **/


      if (conn && conn->current() ) {
         source = conn;
      }
      else {
         if (my_impl->connections.size() == 1) {
            if (!source) {
               source = *my_impl->connections.begin();
            }
         }
         else {
//初始化到线性数组搜索
            auto cptr = my_impl->connections.begin();
            auto cend = my_impl->connections.end();
//我们还记得以前的资料吗？
            if (source) {
//尝试在列表中找到它
               cptr = my_impl->connections.find(source);
               cend = cptr;
               if (cptr == my_impl->connections.end()) {
//不在那里-一定是关门了！cend现在是connections.end，所以只需将环展平即可。
                  source.reset();
                  cptr = my_impl->connections.begin();
               } else {
//找到了-前进到下一个。CEND是旧的来源。
                  if (++cptr == my_impl->connections.end() && cend != my_impl->connections.end() ) {
                     cptr = my_impl->connections.begin();
                  }
               }
            }

//扫描对等机列表，寻找能够提供同步块的其他对等机。
            auto cstart_it = cptr;
            do {
//选择当前的第一个并断开。
               if((*cptr)->current()) {
                  source = *cptr;
                  break;
               }
               if(++cptr == my_impl->connections.end())
                     cptr = my_impl->connections.begin();
            } while(cptr != cstart_it);
//无需检查结果，无论是源高级还是整个列表都已检查，旧源将被重用。
         }
      }

//验证是否有可用的源
      if (!source || !source->current() ) {
         elog("Unable to continue syncing at this time");
         sync_known_lib_num = chain_plug->chain().last_irreversible_block_num();
         sync_last_requested_num = 0;
set_state(in_sync); //可能没有，但我们不能做其他的事
         return;
      }

      if( sync_last_requested_num != sync_known_lib_num ) {
         uint32_t start = sync_next_expected_num;
         uint32_t end = start + sync_req_span - 1;
         if( end > sync_known_lib_num )
            end = sync_known_lib_num;
         if( end > 0 && end >= start ) {
            fc_ilog(logger, "requesting range ${s} to ${e}, from ${n}",
                    ("n",source->peer_name())("s",start)("e",end));
            source->request_sync_blocks(start, end);
            sync_last_requested_num = end;
         }
      }
   }

   void sync_manager::send_handshakes()
   {
      for( auto &ci : my_impl->connections) {
         if( ci->current()) {
            ci->send_handshake();
         }
      }
   }

   void sync_manager::start_sync(const connection_ptr& c, uint32_t target) {
      if( target > sync_known_lib_num) {
         sync_known_lib_num = target;
      }

      if (!sync_required()) {
         uint32_t bnum = chain_plug->chain().last_irreversible_block_num();
         uint32_t hnum = chain_plug->chain().fork_db_head_block_num();
         fc_dlog( logger, "We are already caught up, my irr = ${b}, head = ${h}, target = ${t}",
                  ("b",bnum)("h",hnum)("t",target));
         return;
      }

      if (state == in_sync) {
         set_state(lib_catchup);
         sync_next_expected_num = chain_plug->chain().last_irreversible_block_num() + 1;
      }

      fc_ilog(logger, "Catching up with chain, our last req is ${cc}, theirs is ${t} peer ${p}",
              ( "cc",sync_last_requested_num)("t",target)("p",c->peer_name()));

      request_next_chunk(c);
   }

   void sync_manager::reassign_fetch(const connection_ptr& c, go_away_reason reason) {
      fc_ilog(logger, "reassign_fetch, our last req is ${cc}, next expected is ${ne} peer ${p}",
              ( "cc",sync_last_requested_num)("ne",sync_next_expected_num)("p",c->peer_name()));

      if (c == source) {
         c->cancel_sync(reason);
         sync_last_requested_num = 0;
         request_next_chunk();
      }
   }

   void sync_manager::recv_handshake(const connection_ptr& c, const handshake_message& msg) {
      controller& cc = chain_plug->chain();
      uint32_t lib_num = cc.last_irreversible_block_num();
      uint32_t peer_lib = msg.last_irreversible_block_num;
      reset_lib_num(c);
      c->syncing = false;

//----------------------
//同步需要检查；（lib==最后一个不可逆块）
//
//0。我的头块id==peer head id意味着我们都是按块抓到的
//1。my head block num<peer lib-本地启动同步
//2。my lib>peer head num-如果不是第一代，则发送最后一个“内部收益率捕获”通知
//
//3 my head block num<=peer head block num-更新同步状态并发送捕获请求
//4 my head block num>peer block num如果这不是第一代，则发送通知catchup
//
//----------------------

      uint32_t head = cc.fork_db_head_block_num();
      block_id_type head_id = cc.fork_db_head_block_id();
      if (head_id == msg.head_id) {
         fc_dlog(logger, "sync check state 0");
//通知对等方我们的未决交易
         notice_message note;
         note.known_blocks.mode = none;
         note.known_trx.mode = catch_up;
         note.known_trx.pending = my_impl->local_txns.size();
         c->enqueue( note );
         return;
      }
      if (head < peer_lib) {
         fc_dlog(logger, "sync check state 1");
//在启动同步之前等待收到通知消息
         if (c->protocol_version < proto_explicit_sync) {
            start_sync( c, peer_lib);
         }
         return;
      }
      if (lib_num > msg.head_num ) {
         fc_dlog(logger, "sync check state 2");
         if (msg.generation > 1 || c->protocol_version > proto_base) {
            notice_message note;
            note.known_trx.pending = lib_num;
            note.known_trx.mode = last_irr_catch_up;
            note.known_blocks.mode = last_irr_catch_up;
            note.known_blocks.pending = head;
            c->enqueue( note );
         }
         c->syncing = true;
         return;
      }

      if (head <= msg.head_num ) {
         fc_dlog(logger, "sync check state 3");
         verify_catchup(c, msg.head_num, msg.head_id);
         return;
      }
      else {
         fc_dlog(logger, "sync check state 4");
         if (msg.generation > 1 ||  c->protocol_version > proto_base) {
            notice_message note;
            note.known_trx.mode = none;
            note.known_blocks.mode = catch_up;
            note.known_blocks.pending = head;
            note.known_blocks.ids.push_back(head_id);
            c->enqueue( note );
         }
         c->syncing = true;
         return;
      }
      elog("sync check failed to resolve status");
   }

   void sync_manager::verify_catchup(const connection_ptr& c, uint32_t num, const block_id_type& id) {
      request_message req;
      req.req_blocks.mode = catch_up;
      for (const auto& cc : my_impl->connections) {
         if (cc->fork_head == id ||
             cc->fork_head_num > num) {
            req.req_blocks.mode = none;
            break;
         }
      }
      if( req.req_blocks.mode == catch_up ) {
         c->fork_head = id;
         c->fork_head_num = num;
         ilog("got a catch_up notice while in ${s}, fork head num = ${fhn} target LIB = ${lib} next_expected = ${ne}",
               ("s",stage_str(state))("fhn",num)("lib",sync_known_lib_num)("ne", sync_next_expected_num));
         if (state == lib_catchup)
            return;
         set_state(head_catchup);
      }
      else {
         c->fork_head = block_id_type();
         c->fork_head_num = 0;
      }
      req.req_trx.mode = none;
      c->enqueue( req );
   }

   void sync_manager::recv_notice(const connection_ptr& c, const notice_message& msg) {
      fc_ilog(logger, "sync_manager got ${m} block notice",("m",modes_str(msg.known_blocks.mode)));
      if (msg.known_blocks.mode == catch_up) {
         if (msg.known_blocks.ids.size() == 0) {
            elog("got a catch up with ids size = 0");
         }
         else {
            verify_catchup(c, msg.known_blocks.pending, msg.known_blocks.ids.back());
         }
      }
      else {
         c->last_handshake_recv.last_irreversible_block_num = msg.known_trx.pending;
         reset_lib_num(c);
         start_sync(c, msg.known_trx.pending);
      }
   }

   void sync_manager::rejected_block(const connection_ptr& c, uint32_t blk_num) {
      if (state != in_sync ) {
         fc_ilog(logger, "block ${bn} not accepted from ${p}",("bn",blk_num)("p",c->peer_name()));
         sync_last_requested_num = 0;
         source.reset();
         my_impl->close(c);
         set_state(in_sync);
         send_handshakes();
      }
   }
   void sync_manager::recv_block(const connection_ptr& c, const block_id_type& blk_id, uint32_t blk_num) {
      fc_dlog(logger," got block ${bn} from ${p}",("bn",blk_num)("p",c->peer_name()));
      if (state == lib_catchup) {
         if (blk_num != sync_next_expected_num) {
            fc_ilog(logger, "expected block ${ne} but got ${bn}",("ne",sync_next_expected_num)("bn",blk_num));
            my_impl->close(c);
            return;
         }
         sync_next_expected_num = blk_num + 1;
      }
      if (state == head_catchup) {
         fc_dlog(logger, "sync_manager in head_catchup state");
         set_state(in_sync);
         source.reset();

         block_id_type null_id;
         for (const auto& cp : my_impl->connections) {
            if (cp->fork_head == null_id) {
               continue;
            }
            if (cp->fork_head == blk_id || cp->fork_head_num < blk_num) {
               c->fork_head = null_id;
               c->fork_head_num = 0;
            }
            else {
               set_state(head_catchup);
            }
         }
      }
      else if (state == lib_catchup) {
         if( blk_num == sync_known_lib_num ) {
            fc_dlog( logger, "All caught up with last known last irreversible block resending handshake");
            set_state(in_sync);
            send_handshakes();
         }
         else if (blk_num == sync_last_requested_num) {
            request_next_chunk();
         }
         else {
            fc_dlog(logger,"calling sync_wait on connection ${p}",("p",c->peer_name()));
            c->sync_wait();
         }
      }
   }

//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

   void dispatch_manager::bcast_block(const block_state_ptr& bs) {
      std::set<connection_ptr> skips;
      auto range = received_blocks.equal_range(bs->id);
      for (auto org = range.first; org != range.second; ++org) {
         skips.insert(org->second);
      }
      received_blocks.erase(range.first, range.second);

      block_id_type bid = bs->id;
      uint32_t bnum = bs->block_num;
      peer_block_state pbstate = {bid, bnum, false, true, time_point()};

      pbstate.is_known = true;
      for( auto& cp : my_impl->connections ) {
         if( skips.find( cp ) != skips.end() || !cp->current() ) {
            continue;
         }
         bool has_block = cp->last_handshake_recv.last_irreversible_block_num >= bnum;
         if( !has_block ) {
            fc_dlog(logger, "bcast block ${b} to ${p}", ("b", bnum)("p", cp->peer_name()));
            cp->add_peer_block( pbstate );
            cp->enqueue_block( bs->block );
         }
      }

   }

   void dispatch_manager::recv_block(const connection_ptr& c, const block_id_type& id, uint32_t bnum) {
      received_blocks.insert(std::make_pair(id, c));
      if (c &&
          c->last_req &&
          c->last_req->req_blocks.mode != none &&
          !c->last_req->req_blocks.ids.empty() &&
          c->last_req->req_blocks.ids.back() == id) {
         c->last_req.reset();
      }
      c->add_peer_block({id, bnum, false,true,time_point()});

      fc_dlog(logger, "canceling wait on ${p}", ("p",c->peer_name()));
      c->cancel_wait();
   }

   void dispatch_manager::rejected_block(const block_id_type& id) {
      fc_dlog(logger,"not sending rejected transaction ${tid}",("tid",id));
      auto range = received_blocks.equal_range(id);
      received_blocks.erase(range.first, range.second);
   }

   void dispatch_manager::bcast_transaction(const transaction_metadata_ptr& ptrx) {
      std::set<connection_ptr> skips;
      const auto& id = ptrx->id;

      auto range = received_transactions.equal_range(id);
      for (auto org = range.first; org != range.second; ++org) {
         skips.insert(org->second);
      }
      received_transactions.erase(range.first, range.second);

if( my_impl->local_txns.get<by_id>().find( id ) != my_impl->local_txns.end() ) { //建立
         fc_dlog(logger, "found trxid in local_trxs" );
         return;
      }

      time_point_sec trx_expiration = ptrx->packed_trx->expiration();
      const packed_transaction& trx = *ptrx->packed_trx;

//此实现是为了避免将打包的事务复制到net消息
int which = 8; //匹配打包事务的网络消息

      uint32_t which_size = fc::raw::pack_size( unsigned_int( which ));
      uint32_t payload_size = which_size + fc::raw::pack_size( trx );

      char* header = reinterpret_cast<char*>(&payload_size);
      size_t header_size = sizeof(payload_size);
      size_t buffer_size = header_size + payload_size;

      auto buff = std::make_shared<vector<char>>(buffer_size);
      fc::datastream<char*> ds( buff->data(), buffer_size);
      ds.write( header, header_size );
      fc::raw::pack( ds, unsigned_int( which ));
      fc::raw::pack( ds, trx );

      node_transaction_state nts = {id, trx_expiration, 0, buff};
      my_impl->local_txns.insert(std::move(nts));

      my_impl->send_all( buff, [&id, &skips, trx_expiration](const connection_ptr& c) -> bool {
         if( skips.find(c) != skips.end() || c->syncing ) {
            return false;
          }
          const auto& bs = c->trx_state.find(id);
          bool unknown = bs == c->trx_state.end();
          if( unknown ) {
             c->trx_state.insert(transaction_state({id,0,trx_expiration}));
             fc_dlog(logger, "sending trx to ${n}", ("n",c->peer_name() ) );
          }
          return unknown;
      });

   }

   void dispatch_manager::recv_transaction(const connection_ptr& c, const transaction_id_type& id) {
      received_transactions.insert(std::make_pair(id, c));
      if (c &&
          c->last_req &&
          c->last_req->req_trx.mode != none &&
          !c->last_req->req_trx.ids.empty() &&
          c->last_req->req_trx.ids.back() == id) {
         c->last_req.reset();
      }

      fc_dlog(logger, "canceling wait on ${p}", ("p",c->peer_name()));
      c->cancel_wait();
   }

   void dispatch_manager::rejected_transaction(const transaction_id_type& id) {
      fc_dlog(logger,"not sending rejected transaction ${tid}",("tid",id));
      auto range = received_transactions.equal_range(id);
      received_transactions.erase(range.first, range.second);
   }

   void dispatch_manager::recv_notice(const connection_ptr& c, const notice_message& msg, bool generated) {
      request_message req;
      req.req_trx.mode = none;
      req.req_blocks.mode = none;
      bool send_req = false;
      if (msg.known_trx.mode == normal) {
         req.req_trx.mode = normal;
         req.req_trx.pending = 0;
         send_req = false;
      }
      else if (msg.known_trx.mode != none) {
         elog("passed a notice_message with something other than a normal on none known_trx");
         return;
      }
      if (msg.known_blocks.mode == normal) {
         req.req_blocks.mode = normal;
         controller& cc = my_impl->chain_plug->chain();
         for( const auto& blkid : msg.known_blocks.ids) {
            signed_block_ptr b;
            peer_block_state entry = {blkid,0,true,true,fc::time_point()};
            try {
               b = cc.fetch_block_by_id(blkid);
               if(b)
                  entry.block_num = b->block_num();
            } catch (const assert_exception &ex) {
               ilog( "caught assert on fetch_block_by_id, ${ex}",("ex",ex.what()));
//继续，客户可以询问其他同行
            } catch (...) {
               elog( "failed to retrieve block for id");
            }
            if (!b) {
               send_req = true;
               req.req_blocks.ids.push_back( blkid );
               entry.requested_time = fc::time_point::now();
            }
            c->add_peer_block(entry);
         }
      }
      else if (msg.known_blocks.mode != none) {
         elog("passed a notice_message with something other than a normal on none known_blocks");
         return;
      }
      fc_dlog( logger, "send req = ${sr}", ("sr",send_req));
      if( send_req) {
         c->enqueue(req);
         c->fetch_wait();
         c->last_req = std::move(req);
      }
   }

   void dispatch_manager::retry_fetch(const connection_ptr& c) {
      if (!c->last_req) {
         return;
      }
      fc_wlog( logger, "failed to fetch from ${p}",("p",c->peer_name()));
      transaction_id_type tid;
      block_id_type bid;
      bool is_txn = false;
      if( c->last_req->req_trx.mode == normal && !c->last_req->req_trx.ids.empty() ) {
         is_txn = true;
         tid = c->last_req->req_trx.ids.back();
      }
      else if( c->last_req->req_blocks.mode == normal && !c->last_req->req_blocks.ids.empty() ) {
         bid = c->last_req->req_blocks.ids.back();
      }
      else {
         fc_wlog( logger,"no retry, block mpde = ${b} trx mode = ${t}",
                  ("b",modes_str(c->last_req->req_blocks.mode))("t",modes_str(c->last_req->req_trx.mode)));
         return;
      }
      for (auto& conn : my_impl->connections) {
         if (conn == c || conn->last_req) {
            continue;
         }
         bool sendit = false;
         if (is_txn) {
            auto trx = conn->trx_state.get<by_id>().find(tid);
            sendit = trx != conn->trx_state.end();
         }
         else {
            auto blk = conn->blk_state.get<by_id>().find(bid);
            sendit = blk != conn->blk_state.end() && blk->is_known;
         }
         if (sendit) {
            conn->enqueue(*c->last_req);
            conn->fetch_wait();
            conn->last_req = c->last_req;
            return;
         }
      }

//在这一点上没有其他同行拥有它，重新请求或什么都不做？
      if( c->connected() ) {
         c->enqueue(*c->last_req);
         c->fetch_wait();
      }
   }

//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

   void net_plugin_impl::connect(const connection_ptr& c) {
      if( c->no_retry != go_away_reason::no_reason) {
         fc_dlog( logger, "Skipping connect due to go_away reason ${r}",("r", reason_str( c->no_retry )));
         return;
      }

      auto colon = c->peer_addr.find(':');

      if (colon == std::string::npos || colon == 0) {
         elog("Invalid peer address. must be \"host:port\": ${p}", ("p",c->peer_addr));
         for ( auto itr : connections ) {
            if((*itr).peer_addr == c->peer_addr) {
               (*itr).reset();
               close(itr);
               connections.erase(itr);
               break;
            }
         }
         return;
      }

      auto host = c->peer_addr.substr( 0, colon );
      auto port = c->peer_addr.substr( colon + 1);
      idump((host)(port));
      tcp::resolver::query query( tcp::v4(), host.c_str(), port.c_str() );
      connection_wptr weak_conn = c;
//注意：还需要添加对IPv6的支持

      resolver->async_resolve( query,
                               [weak_conn, this]( const boost::system::error_code& err,
                                          tcp::resolver::iterator endpoint_itr ){
                                  auto c = weak_conn.lock();
                                  if (!c) return;
                                  if( !err ) {
                                     connect( c, endpoint_itr );
                                  } else {
                                     elog( "Unable to resolve ${peer_addr}: ${error}",
                                           (  "peer_addr", c->peer_name() )("error", err.message() ) );
                                  }
                               });
   }

   void net_plugin_impl::connect(const connection_ptr& c, tcp::resolver::iterator endpoint_itr) {
      if( c->no_retry != go_away_reason::no_reason) {
         string rsn = reason_str(c->no_retry);
         return;
      }
      auto current_endpoint = *endpoint_itr;
      ++endpoint_itr;
      c->connecting = true;
      connection_wptr weak_conn = c;
      c->socket->async_connect( current_endpoint, [weak_conn, endpoint_itr, this] ( const boost::system::error_code& err ) {
            auto c = weak_conn.lock();
            if (!c) return;
            if( !err && c->socket->is_open() ) {
               if (start_session( c )) {
                  c->send_handshake();
               }
            } else {
               if( endpoint_itr != tcp::resolver::iterator() ) {
                  close(c);
                  connect( c, endpoint_itr );
               }
               else {
                  elog( "connection failed to ${peer}: ${error}",
                        ( "peer", c->peer_name())("error",err.message()));
                  c->connecting = false;
                  my_impl->close(c);
               }
            }
         } );
   }

   bool net_plugin_impl::start_session(const connection_ptr& con) {
      boost::asio::ip::tcp::no_delay nodelay( true );
      boost::system::error_code ec;
      con->socket->set_option( nodelay, ec );
      if (ec) {
         elog( "connection failed to ${peer}: ${error}",
               ( "peer", con->peer_name())("error",ec.message()));
         con->connecting = false;
         close(con);
         return false;
      }
      else {
         start_read_message( con );
         ++started_sessions;
         return true;
//现在，我们可以只使用应用程序主循环。
//con->read loop_complete=bf:：async（[=]（）read_loop（con）；）；
//con->writeLoop_complete=bf:：async（[=]（）write_loop con）；）；
      }
   }


   void net_plugin_impl::start_listen_loop() {
      auto socket = std::make_shared<tcp::socket>( std::ref( app().get_io_service() ) );
      acceptor->async_accept( *socket, [socket,this]( boost::system::error_code ec ) {
            if( !ec ) {
               uint32_t visitors = 0;
               uint32_t from_addr = 0;
               auto paddr = socket->remote_endpoint(ec).address();
               if (ec) {
                  fc_elog(logger,"Error getting remote endpoint: ${m}",("m", ec.message()));
               }
               else {
                  for (auto &conn : connections) {
                     if(conn->socket->is_open()) {
                        if (conn->peer_addr.empty()) {
                           visitors++;
                           boost::system::error_code ec;
                           if (paddr == conn->socket->remote_endpoint(ec).address()) {
                              from_addr++;
                           }
                        }
                     }
                  }
                  if (num_clients != visitors) {
                     ilog("checking max client, visitors = ${v} num clients ${n}",("v",visitors)("n",num_clients));
                     num_clients = visitors;
                  }
                  if( from_addr < max_nodes_per_host && (max_client_count == 0 || num_clients < max_client_count )) {
                     ++num_clients;
                     connection_ptr c = std::make_shared<connection>( socket );
                     connections.insert( c );
                     start_session( c );

                  }
                  else {
                     if (from_addr >= max_nodes_per_host) {
                        fc_elog(logger, "Number of connections (${n}) from ${ra} exceeds limit",
                                ("n", from_addr+1)("ra",paddr.to_string()));
                     }
                     else {
                        fc_elog(logger, "Error max_client_count ${m} exceeded",
                                ( "m", max_client_count) );
                     }
                     socket->close();
                  }
               }
            } else {
               elog( "Error accepting connection: ${m}",( "m", ec.message() ) );
//对于下面列出的错误代码，调用start_listen_loop（）。
               switch (ec.value()) {
                  case ECONNABORTED:
                  case EMFILE:
                  case ENFILE:
                  case ENOBUFS:
                  case ENOMEM:
                  case EPROTO:
                     break;
                  default:
                     return;
               }
            }
            start_listen_loop();
         });
   }

   void net_plugin_impl::start_read_message(const connection_ptr& conn) {

      try {
         if(!conn->socket) {
            return;
         }
         connection_wptr weak_conn = conn;

         std::size_t minimum_read = conn->outstanding_read_bytes ? *conn->outstanding_read_bytes : message_header_size;

         if (use_socket_read_watermark) {
            const size_t max_socket_read_watermark = 4096;
            std::size_t socket_read_watermark = std::min<std::size_t>(minimum_read, max_socket_read_watermark);
            boost::asio::socket_base::receive_low_watermark read_watermark_opt(socket_read_watermark);
            conn->socket->set_option(read_watermark_opt);
         }

         auto completion_handler = [minimum_read](boost::system::error_code ec, std::size_t bytes_transferred) -> std::size_t {
            if (ec || bytes_transferred >= minimum_read ) {
               return 0;
            } else {
               return minimum_read - bytes_transferred;
            }
         };

         boost::asio::async_read(*conn->socket,
            conn->pending_message_buffer.get_buffer_sequence_for_boost_async_read(), completion_handler,
            [this,weak_conn]( boost::system::error_code ec, std::size_t bytes_transferred ) {
               auto conn = weak_conn.lock();
               if (!conn) {
                  return;
               }

               conn->outstanding_read_bytes.reset();

               try {
                  if( !ec ) {
                     if (bytes_transferred > conn->pending_message_buffer.bytes_to_write()) {
                        elog("async_read_some callback: bytes_transfered = ${bt}, buffer.bytes_to_write = ${btw}",
                             ("bt",bytes_transferred)("btw",conn->pending_message_buffer.bytes_to_write()));
                     }
                     EOS_ASSERT(bytes_transferred <= conn->pending_message_buffer.bytes_to_write(), plugin_exception, "");
                     conn->pending_message_buffer.advance_write_ptr(bytes_transferred);
                     while (conn->pending_message_buffer.bytes_to_read() > 0) {
                        uint32_t bytes_in_buffer = conn->pending_message_buffer.bytes_to_read();

                        if (bytes_in_buffer < message_header_size) {
                           conn->outstanding_read_bytes.emplace(message_header_size - bytes_in_buffer);
                           break;
                        } else {
                           uint32_t message_length;
                           auto index = conn->pending_message_buffer.read_index();
                           conn->pending_message_buffer.peek(&message_length, sizeof(message_length), index);
                           if(message_length > def_send_buffer_size*2 || message_length == 0) {
                              boost::system::error_code ec;
                              elog("incoming message length unexpected (${i}), from ${p}",
                                    ("i", message_length)("p",boost::lexical_cast<std::string>(conn->socket->remote_endpoint(ec))));
                              close(conn);
                              return;
                           }

                           auto total_message_bytes = message_length + message_header_size;

                           if (bytes_in_buffer >= total_message_bytes) {
                              conn->pending_message_buffer.advance_read_ptr(message_header_size);
                              if (!conn->process_next_message(*this, message_length)) {
                                 return;
                              }
                           } else {
                              auto outstanding_message_bytes = total_message_bytes - bytes_in_buffer;
                              auto available_buffer_bytes = conn->pending_message_buffer.bytes_to_write();
                              if (outstanding_message_bytes > available_buffer_bytes) {
                                 conn->pending_message_buffer.add_space( outstanding_message_bytes - available_buffer_bytes );
                              }

                              conn->outstanding_read_bytes.emplace(outstanding_message_bytes);
                              break;
                           }
                        }
                     }
                     start_read_message(conn);
                  } else {
                     auto pname = conn->peer_name();
                     if (ec.value() != boost::asio::error::eof) {
                        elog( "Error reading message from ${p}: ${m}",("p",pname)( "m", ec.message() ) );
                     } else {
                        ilog( "Peer ${p} closed connection",("p",pname) );
                     }
                     close( conn );
                  }
               }
               catch(const std::exception &ex) {
                  string pname = conn ? conn->peer_name() : "no connection name";
                  elog("Exception in handling read data from ${p} ${s}",("p",pname)("s",ex.what()));
                  close( conn );
               }
               catch(const fc::exception &ex) {
                  string pname = conn ? conn->peer_name() : "no connection name";
                  elog("Exception in handling read data ${s}", ("p",pname)("s",ex.to_string()));
                  close( conn );
               }
               catch (...) {
                  string pname = conn ? conn->peer_name() : "no connection name";
                  elog( "Undefined exception hanlding the read data from connection ${p}",( "p",pname));
                  close( conn );
               }
            } );
      } catch (...) {
         string pname = conn ? conn->peer_name() : "no connection name";
         elog( "Undefined exception handling reading ${p}",("p",pname) );
         close( conn );
      }
   }

   size_t net_plugin_impl::count_open_sockets() const
   {
      size_t count = 0;
      for( auto &c : connections) {
         if(c->socket->is_open())
            ++count;
      }
      return count;
   }


   template<typename VerifierFunc>
   void net_plugin_impl::send_all(const std::shared_ptr<std::vector<char>>& send_buffer, VerifierFunc verify) {
      for( auto &c : connections) {
         if( c->current() && verify( c )) {
            c->enqueue_buffer( send_buffer, true, no_reason );
         }
      }
   }

   bool net_plugin_impl::is_valid(const handshake_message& msg) {
//对传入的握手消息做一些基本的验证，所以
//不需要握手的信息可以很快被丢弃
//影响状态。
      bool valid = true;
      if (msg.last_irreversible_block_num > msg.head_num) {
         wlog("Handshake message validation: last irreversible block (${i}) is greater than head block (${h})",
              ("i", msg.last_irreversible_block_num)("h", msg.head_num));
         valid = false;
      }
      if (msg.p2p_address.empty()) {
         wlog("Handshake message validation: p2p_address is null string");
         valid = false;
      }
      if (msg.os.empty()) {
         wlog("Handshake message validation: os field is null string");
         valid = false;
      }
      if ((msg.sig != chain::signature_type() || msg.token != sha256()) && (msg.token != fc::sha256::hash(msg.time))) {
         wlog("Handshake message validation: token field invalid");
         valid = false;
      }
      return valid;
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const chain_size_message& msg) {
      peer_ilog(c, "received chain_size_message");
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const handshake_message& msg) {
      peer_ilog(c, "received handshake_message");
      if (!is_valid(msg)) {
         peer_elog( c, "bad handshake message");
         c->enqueue( go_away_message( fatal_other ));
         return;
      }
      controller& cc = chain_plug->chain();
      uint32_t lib_num = cc.last_irreversible_block_num();
      uint32_t peer_lib = msg.last_irreversible_block_num;
      if( c->connecting ) {
         c->connecting = false;
      }
      if (msg.generation == 1) {
         if( msg.node_id == node_id) {
            elog( "Self connection detected. Closing connection");
            c->enqueue( go_away_message( self ) );
            return;
         }

         if( c->peer_addr.empty() || c->last_handshake_recv.node_id == fc::sha256()) {
            fc_dlog(logger, "checking for duplicate" );
            for(const auto &check : connections) {
               if(check == c)
                  continue;
               if(check->connected() && check->peer_name() == msg.p2p_address) {
//两个同龄人可能同时到达这里，所以
//我们需要避免这样的情况，他们会告诉另一个连接离开。
//使用两个连接的初始握手时间之和，我们将
//任意地（但始终在两个同龄人之间）保留其中一个。
                  if (msg.time + c->last_handshake_sent.time <= check->last_handshake_sent.time + check->last_handshake_recv.time)
                     continue;

                  fc_dlog( logger, "sending go_away duplicate to ${ep}", ("ep",msg.p2p_address) );
                  go_away_message gam(duplicate);
                  gam.node_id = node_id;
                  c->enqueue(gam);
                  c->no_retry = duplicate;
                  return;
               }
            }
         }
         else {
            fc_dlog(logger, "skipping duplicate check, addr == ${pa}, id = ${ni}",("pa",c->peer_addr)("ni",c->last_handshake_recv.node_id));
         }

         if( msg.chain_id != chain_id) {
            elog( "Peer on a different chain. Closing connection");
            c->enqueue( go_away_message(go_away_reason::wrong_chain) );
            return;
         }
         c->protocol_version = to_protocol_version(msg.network_version);
         if(c->protocol_version != net_version) {
            if (network_version_match) {
               elog("Peer network version does not match expected ${nv} but got ${mnv}",
                    ("nv", net_version)("mnv", c->protocol_version));
               c->enqueue(go_away_message(wrong_version));
               return;
            } else {
               ilog("Local network version: ${nv} Remote version: ${mnv}",
                    ("nv", net_version)("mnv", c->protocol_version));
            }
         }

         if(  c->node_id != msg.node_id) {
            c->node_id = msg.node_id;
         }

         if(!authenticate_peer(msg)) {
            elog("Peer not authenticated.  Closing connection.");
            c->enqueue(go_away_message(authentication));
            return;
         }

         bool on_fork = false;
         fc_dlog(logger, "lib_num = ${ln} peer_lib = ${pl}",("ln",lib_num)("pl",peer_lib));

         if( peer_lib <= lib_num && peer_lib > 0) {
            try {
               block_id_type peer_lib_id =  cc.get_block_id_for_num( peer_lib);
               on_fork =( msg.last_irreversible_block_id != peer_lib_id);
            }
            catch( const unknown_block_exception &ex) {
               wlog( "peer last irreversible block ${pl} is unknown", ("pl", peer_lib));
               on_fork = true;
            }
            catch( ...) {
               wlog( "caught an exception getting block id for ${pl}",("pl",peer_lib));
               on_fork = true;
            }
            if( on_fork) {
               elog( "Peer chain is forked");
               c->enqueue( go_away_message( forked ));
               return;
            }
         }

         if (c->sent_handshake_count == 0) {
            c->send_handshake();
         }
      }

      c->last_handshake_recv = msg;
      c->_logger_variant.reset();
      sync_master->recv_handshake(c,msg);
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const go_away_message& msg) {
      string rsn = reason_str( msg.reason );
      peer_ilog(c, "received go_away_message");
      ilog( "received a go away message from ${p}, reason = ${r}",
            ("p", c->peer_name())("r",rsn));
      c->no_retry = msg.reason;
      if(msg.reason == duplicate ) {
         c->node_id = msg.node_id;
      }
      c->flush_queues();
      close(c);
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const time_message& msg) {
      peer_ilog(c, "received time_message");
      /*我们已经损失了多少微秒
       *信息，但无法帮助。
       **/

      msg.dst = c->get_time();

//如果传输时间戳为零，则对等机将严重损坏。
      if(msg.xmt == 0)
         /*urn；/*时间戳无效*/

      如果（msg.xmt==c->xmt）
         返回；/*重复数据包*/


      c->xmt = msg.xmt;
      c->rec = msg.rec;
      c->dst = msg.dst;

      if(msg.org == 0)
         {
            c->send_time(msg);
return;  //我们还没有足够的数据来执行计算。
         }

      c->offset = (double(c->rec - c->org) + double(msg.xmt - c->dst)) / 2;
      double NsecPerUsec{1000};

      if(logger.is_enabled(fc::log_level::all))
         logger.log(FC_LOG_MESSAGE(all, "Clock offset is ${o}ns (${us}us)", ("o", c->offset)("us", c->offset/NsecPerUsec)));
      c->org = 0;
      c->rec = 0;
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const notice_message& msg) {
//Peer告诉我们一个或多个块或TXN。完成同步后，继续
//以前未知块或txn的通知，
//
      peer_ilog(c, "received notice_message");
      c->connecting = false;
      request_message req;
      bool send_req = false;
      if (msg.known_trx.mode != none) {
         fc_dlog(logger,"this is a ${m} notice with ${n} transactions", ("m",modes_str(msg.known_trx.mode))("n",msg.known_trx.pending));
      }
      switch (msg.known_trx.mode) {
      case none:
         break;
      case last_irr_catch_up: {
         c->last_handshake_recv.head_num = msg.known_trx.pending;
         req.req_trx.mode = none;
         break;
      }
      case catch_up : {
         if( msg.known_trx.pending > 0) {
//除了我们已经知道的以外，计划获取所有信息。
            req.req_trx.mode = catch_up;
            send_req = true;
            size_t known_sum = local_txns.size();
            if( known_sum ) {
               for( const auto& t : local_txns.get<by_id>() ) {
                  req.req_trx.ids.push_back( t.id );
               }
            }
         }
         break;
      }
      case normal: {
         dispatcher->recv_notice(c, msg, false);
      }
      }

      if (msg.known_blocks.mode != none) {
         fc_dlog(logger,"this is a ${m} notice with ${n} blocks", ("m",modes_str(msg.known_blocks.mode))("n",msg.known_blocks.pending));
      }
      switch (msg.known_blocks.mode) {
      case none : {
         break;
      }
      case last_irr_catch_up:
      case catch_up: {
         sync_master->recv_notice(c,msg);
         break;
      }
      case normal : {
         dispatcher->recv_notice(c, msg, false);
         break;
      }
      default: {
         peer_elog(c, "bad notice_message : invalid known_blocks.mode ${m}",("m",static_cast<uint32_t>(msg.known_blocks.mode)));
      }
      }
      fc_dlog(logger, "send req = ${sr}", ("sr",send_req));
      if( send_req) {
         c->enqueue(req);
      }
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const request_message& msg) {
      switch (msg.req_blocks.mode) {
      case catch_up :
         peer_ilog(c,  "received request_message:catch_up");
         c->blk_send_branch();
         break;
      case normal :
         peer_ilog(c, "received request_message:normal");
         c->blk_send(msg.req_blocks.ids);
         break;
      default:;
      }


      switch (msg.req_trx.mode) {
      case catch_up :
         c->txn_send_pending(msg.req_trx.ids);
         break;
      case normal :
         c->txn_send(msg.req_trx.ids);
         break;
      case none :
         if(msg.req_blocks.mode == none)
            c->stop_send();
         break;
      default:;
      }

   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const sync_request_message& msg) {
      if( msg.end_block == 0) {
         c->peer_requested.reset();
         c->flush_queues();
      } else {
         c->peer_requested = sync_state( msg.start_block,msg.end_block,msg.start_block-1);
         c->enqueue_sync_block();
      }
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const packed_transaction_ptr& trx) {
      fc_dlog(logger, "got a packed transaction, cancel wait");
      peer_ilog(c, "received packed_transaction");
      controller& cc = my_impl->chain_plug->chain();
      if( cc.get_read_mode() == eosio::db_read_mode::READ_ONLY ) {
         fc_dlog(logger, "got a txn in read-only mode - dropping");
         return;
      }
      if( sync_master->is_active(c) ) {
         fc_dlog(logger, "got a txn during sync - dropping");
         return;
      }

      auto ptrx = std::make_shared<transaction_metadata>( trx );
      const auto& tid = ptrx->id;

      c->cancel_wait();
      if(local_txns.get<by_id>().find(tid) != local_txns.end()) {
         fc_dlog(logger, "got a duplicate transaction - dropping");
         return;
      }
      dispatcher->recv_transaction(c, tid);
      chain_plug->accept_transaction(ptrx, [c, this, ptrx](const static_variant<fc::exception_ptr, transaction_trace_ptr>& result) {
         if (result.contains<fc::exception_ptr>()) {
            peer_dlog(c, "bad packed_transaction : ${m}", ("m",result.get<fc::exception_ptr>()->what()));
         } else {
            auto trace = result.get<transaction_trace_ptr>();
            if (!trace->except) {
               fc_dlog(logger, "chain accepted transaction");
               this->dispatcher->bcast_transaction(ptrx);
               return;
            }

            peer_elog(c, "bad packed_transaction : ${m}", ("m",trace->except->what()));
         }

         dispatcher->rejected_transaction(ptrx->id);
      });
   }

   void net_plugin_impl::handle_message(const connection_ptr& c, const signed_block_ptr& msg) {
      controller &cc = chain_plug->chain();
      block_id_type blk_id = msg->id();
      uint32_t blk_num = msg->block_num();
      fc_dlog(logger, "canceling wait on ${p}", ("p",c->peer_name()));
      c->cancel_wait();

      try {
         if( cc.fetch_block_by_id(blk_id)) {
            sync_master->recv_block(c, blk_id, blk_num);
            return;
         }
      } catch( ...) {
//这个应该被抓住吗？
         elog("Caught an unknown exception trying to recall blockID");
      }

      dispatcher->recv_block(c, blk_id, blk_num);
      fc::microseconds age( fc::time_point::now() - msg->timestamp);
      peer_ilog(c, "received signed_block : #${n} block age in secs = ${age}",
              ("n",blk_num)("age",age.to_seconds()));

      go_away_reason reason = fatal_other;
      try {
chain_plug->accept_block(msg); //，sync_master->is_active（c））；
         reason = no_reason;
      } catch( const unlinkable_block_exception &ex) {
         peer_elog(c, "bad signed_block : ${m}", ("m",ex.what()));
         reason = unlinkable;
      } catch( const block_validate_exception &ex) {
         peer_elog(c, "bad signed_block : ${m}", ("m",ex.what()));
         elog( "block_validate_exception accept block #${n} syncing from ${p}",("n",blk_num)("p",c->peer_name()));
         reason = validation;
      } catch( const assert_exception &ex) {
         peer_elog(c, "bad signed_block : ${m}", ("m",ex.what()));
         elog( "unable to accept block on assert exception ${n} from ${p}",("n",ex.to_string())("p",c->peer_name()));
      } catch( const fc::exception &ex) {
         peer_elog(c, "bad signed_block : ${m}", ("m",ex.what()));
         elog( "accept_block threw a non-assert exception ${x} from ${p}",( "x",ex.to_string())("p",c->peer_name()));
         reason = no_reason;
      } catch( ...) {
         peer_elog(c, "bad signed_block : unknown exception");
         elog( "handle sync block caught something else from ${p}",("num",blk_num)("p",c->peer_name()));
      }

      update_block_num ubn(blk_num);
      if( reason == no_reason ) {
         for (const auto &recpt : msg->transactions) {
            auto id = (recpt.trx.which() == 0) ? recpt.trx.get<transaction_id_type>() : recpt.trx.get<packed_transaction>().id();
            auto ltx = local_txns.get<by_id>().find(id);
            if( ltx != local_txns.end()) {
               local_txns.modify( ltx, ubn );
            }
            auto ctx = c->trx_state.get<by_id>().find(id);
            if( ctx != c->trx_state.end()) {
               c->trx_state.modify( ctx, ubn );
            }
         }
         sync_master->recv_block(c, blk_id, blk_num);
      }
      else {
         sync_master->rejected_block(c, blk_num);
      }
   }

   void net_plugin_impl::start_conn_timer(boost::asio::steady_timer::duration du, std::weak_ptr<connection> from_connection) {
      connector_check->expires_from_now( du);
      connector_check->async_wait( [this, from_connection](boost::system::error_code ec) {
            if( !ec) {
               connection_monitor(from_connection);
            }
            else {
               elog( "Error from connection check monitor: ${m}",( "m", ec.message()));
               start_conn_timer( connector_period, std::weak_ptr<connection>());
            }
         });
   }

   void net_plugin_impl::start_txn_timer() {
      transaction_check->expires_from_now( txn_exp_period);
      transaction_check->async_wait( [this](boost::system::error_code ec) {
            if( !ec) {
               expire_txns();
            }
            else {
               elog( "Error from transaction check monitor: ${m}",( "m", ec.message()));
               start_txn_timer();
            }
         });
   }

   void net_plugin_impl::ticker() {
      keepalive_timer->expires_from_now(keepalive_interval);
      keepalive_timer->async_wait([this](boost::system::error_code ec) {
            ticker();
            if (ec) {
               wlog("Peer keepalive ticked sooner than expected: ${m}", ("m", ec.message()));
            }
            for (auto &c : connections ) {
               if (c->socket->is_open()) {
                  c->send_time();
               }
            }
         });
   }

   void net_plugin_impl::start_monitors() {
      connector_check.reset(new boost::asio::steady_timer( app().get_io_service()));
      transaction_check.reset(new boost::asio::steady_timer( app().get_io_service()));
      start_conn_timer(connector_period, std::weak_ptr<connection>());
      start_txn_timer();
   }

   void net_plugin_impl::expire_txns() {
      start_txn_timer();

      auto now = time_point::now();
      auto start_size = local_txns.size();

      expire_local_txns();

      controller& cc = chain_plug->chain();
      uint32_t lib = cc.last_irreversible_block_num();
      for ( auto &c : connections ) {
         auto &stale_txn = c->trx_state.get<by_block_num>();
         stale_txn.erase( stale_txn.lower_bound(1), stale_txn.upper_bound(lib) );
         auto &stale_txn_e = c->trx_state.get<by_expiry>();
         stale_txn_e.erase(stale_txn_e.lower_bound(time_point_sec()), stale_txn_e.upper_bound(time_point::now()));
         auto &stale_blk = c->blk_state.get<by_block_num>();
         stale_blk.erase( stale_blk.lower_bound(1), stale_blk.upper_bound(lib) );
      }
      fc_dlog(logger, "expire_txns ${n}us size ${s} removed ${r}",
            ("n", time_point::now() - now)("s", start_size)("r", start_size - local_txns.size()) );
   }

   void net_plugin_impl::expire_local_txns() {
      auto& old = local_txns.get<by_expiry>();
      auto ex_lo = old.lower_bound( fc::time_point_sec(0) );
      auto ex_up = old.upper_bound( time_point::now() );
      old.erase( ex_lo, ex_up );

      auto& stale = local_txns.get<by_block_num>();
      controller& cc = chain_plug->chain();
      uint32_t lib = cc.last_irreversible_block_num();
      stale.erase( stale.lower_bound(1), stale.upper_bound(lib) );
   }

   void net_plugin_impl::connection_monitor(std::weak_ptr<connection> from_connection) {
      auto max_time = fc::time_point::now();
      max_time += fc::milliseconds(max_cleanup_time_ms);
      auto from = from_connection.lock();
      auto it = (from ? connections.find(from) : connections.begin());
      if (it == connections.end()) it = connections.begin();
      while (it != connections.end()) {
         if (fc::time_point::now() >= max_time) {
start_conn_timer(std::chrono::milliseconds(1), *it); //避免疲劳
            return;
         }
         if( !(*it)->socket->is_open() && !(*it)->connecting) {
            if( (*it)->peer_addr.length() > 0) {
               connect(*it);
            }
            else {
               it = connections.erase(it);
               continue;
            }
         }
         ++it;
      }
      start_conn_timer(connector_period, std::weak_ptr<connection>());
   }

   void net_plugin_impl::close(const connection_ptr& c) {
      if( c->peer_addr.empty() && c->socket->is_open() ) {
         if (num_clients == 0) {
            fc_wlog( logger, "num_clients already at 0");
         }
         else {
            --num_clients;
         }
      }
      c->close();
   }

   void net_plugin_impl::accepted_block(const block_state_ptr& block) {
      fc_dlog(logger,"signaled, id = ${id}",("id", block->id));
      dispatcher->bcast_block(block);
   }

   void net_plugin_impl::transaction_ack(const std::pair<fc::exception_ptr, transaction_metadata_ptr>& results) {
      const auto& id = results.second->id;
      if (results.first) {
         fc_ilog(logger,"signaled NACK, trx-id = ${id} : ${why}",("id", id)("why", results.first->to_detail_string()));
         dispatcher->rejected_transaction(id);
      } else {
         fc_ilog(logger,"signaled ACK, trx-id = ${id}",("id", id));
         dispatcher->bcast_transaction(results.second);
      }
   }

   bool net_plugin_impl::authenticate_peer(const handshake_message& msg) const {
      if(allowed_connections == None)
         return false;

      if(allowed_connections == Any)
         return true;

      if(allowed_connections & (Producers | Specified)) {
         auto allowed_it = std::find(allowed_peers.begin(), allowed_peers.end(), msg.key);
         auto private_it = private_keys.find(msg.key);
         bool found_producer_key = false;
         if(producer_plug != nullptr)
            found_producer_key = producer_plug->is_producer_key(msg.key);
         if( allowed_it == allowed_peers.end() && private_it == private_keys.end() && !found_producer_key) {
            elog( "Peer ${peer} sent a handshake with an unauthorized key: ${key}.",
                  ("peer", msg.p2p_address)("key", msg.key));
            return false;
         }
      }

      namespace sc = std::chrono;
      sc::system_clock::duration msg_time(msg.time);
      auto time = sc::system_clock::now().time_since_epoch();
      if(time - msg_time > peer_authentication_interval) {
         elog( "Peer ${peer} sent a handshake with a timestamp skewed by more than ${time}.",
("peer", msg.p2p_address)("time", "1 second")); //TODO为std:：chrono:：system_clock:：duration添加到_变量
         return false;
      }

      if(msg.sig != chain::signature_type() && msg.token != sha256()) {
         sha256 hash = fc::sha256::hash(msg.time);
         if(hash != msg.token) {
            elog( "Peer ${peer} sent a handshake with an invalid token.",
                  ("peer", msg.p2p_address));
            return false;
         }
         chain::public_key_type peer_key;
         try {
            peer_key = crypto::public_key(msg.sig, msg.token, true);
         }
         /*ch（fc:：exception&/*e*/）
            Elog（“peer$peer发送了一个带有不可恢复密钥的握手。”，
                  （peer，msg.p2p_address））；
            返回错误；
         }
         如果（（允许的_连接&（指定的生产者）&&peer_键！= MSG.Key）{
            ELOG（“对等机$对等机使用未经身份验证的密钥发送握手。”，
                  （peer，msg.p2p_address））；
            返回错误；
         }
      }
      否则，如果（允许的连接和（指定的生产商）
         dlog（“对等端发送了一个签名和令牌为空的握手，但此节点只接受经过身份验证的连接。”）；
         返回错误；
      }
      回归真实；
   }

   chain:：public_key_type net_plugin_impl:：get_authentication_key（）const_
      如果（！）private_keys.empty（））
         首先返回private_keys.begin（）->
      /*producer_plugin*pp=app（）。查找_plugin<producer_plugin>（）；
      如果（PP）！=nullptr&&pp->get_state（）==abstract_plugin:：started）
         返回pp->first_producer_public_key（）。*/

      return chain::public_key_type();
   }

   chain::signature_type net_plugin_impl::sign_compact(const chain::public_key_type& signer, const fc::sha256& digest) const
   {
      auto private_key_itr = private_keys.find(signer);
      if(private_key_itr != private_keys.end())
         return private_key_itr->second.sign(digest);
      if(producer_plug != nullptr && producer_plug->get_state() == abstract_plugin::started)
         return producer_plug->sign_compact(signer, digest);
      return chain::signature_type();
   }

   void
   handshake_initializer::populate( handshake_message &hello) {
      hello.network_version = net_version_base + net_version;
      hello.chain_id = my_impl->chain_id;
      hello.node_id = my_impl->node_id;
      hello.key = my_impl->get_authentication_key();
      hello.time = std::chrono::system_clock::now().time_since_epoch().count();
      hello.token = fc::sha256::hash(hello.time);
      hello.sig = my_impl->sign_compact(hello.key, hello.token);
//如果我们不能签名，就不要发送令牌。
      if(hello.sig == chain::signature_type())
         hello.token = sha256();
      hello.p2p_address = my_impl->p2p_address + " - " + hello.node_id.str().substr(0,7);
#if defined( __APPLE__ )
      hello.os = "osx";
#elif defined( __linux__ )
      hello.os = "linux";
#elif defined( _MSC_VER )
      hello.os = "win32";
#else
      hello.os = "other";
#endif
      hello.agent = my_impl->user_agent_name;


      controller& cc = my_impl->chain_plug->chain();
      hello.head_id = fc::sha256();
      hello.last_irreversible_block_id = fc::sha256();
      hello.head_num = cc.fork_db_head_block_num();
      hello.last_irreversible_block_num = cc.last_irreversible_block_num();
      if( hello.last_irreversible_block_num ) {
         try {
            hello.last_irreversible_block_id = cc.get_block_id_for_num(hello.last_irreversible_block_num);
         }
         catch( const unknown_block_exception &ex) {
            ilog("caught unkown_block");
            hello.last_irreversible_block_num = 0;
         }
      }
      if( hello.head_num ) {
         try {
            hello.head_id = cc.get_block_id_for_num( hello.head_num );
         }
         catch( const unknown_block_exception &ex) {
           hello.head_num = 0;
         }
      }
   }

   net_plugin::net_plugin()
      :my( new net_plugin_impl ) {
      my_impl = my.get();
   }

   net_plugin::~net_plugin() {
   }

   /*d net_plugin:：set_program_options（选项_description&/*cli*/，选项_description&cfg）
   {
      添加选项（）
         （“p2p侦听端点”，bpo:：value<string>（）->默认值（“0.0.0.0:9876”），“实际主机：用于侦听传入的p2p连接的端口。”）
         （“p2p服务器地址”，bpo:：value<string>（），“用于标识此节点的外部可访问主机：端口。”默认为p2p侦听终结点。“）
         （“p2p peer address”，bpo:：value<vector<string>>（）->compositing（），”要连接到的对等节点的公共端点。根据需要使用多个P2P对等地址选项组成网络。”）
         （p2p max nodes per host，bpo:：value<int>（）->default_value（def_max_nodes_per_host），“来自任何单个IP地址的最大客户端节点数”）。
         （“代理名称”，bpo:：value<string>（）->默认值（“\”eos test agent\“”“），“提供的用于在对等节点中标识此节点的名称。”）
         （“allowed connection”，bpo:：value<vector<string>>（）->multioken（）->defaultvalue（“any”，“any”），“可以是'any'或'producers'或'specified'或'none'。如果“指定”，则必须至少指定一次对等密钥。如果只有“生产者”，则不需要对等密钥。“生产者”和“指定的”可以组合。“）
         （“peer key”，bpo:：value<vector<string>>（）->compositing（）->multioken（），”允许连接的peer的可选公钥。可多次使用。”）
         （“peer private key”，boost:：programou options:：value<vector<string>>（）->compositing（）->multioken（），
           “[publickey，wif private key]的元组（可以指定多次）”）
         （max clients，bpo:：value<int>（）->default_value（def_max_clients），“接受连接的最大客户端数，使用0表示无限制”）
         （“连接清理周期”，bpo:：value<int>（）->default_value（def_conn_retry_wait），“清理死连接前等待的秒数”）。
         （“最大清理时间毫秒”，bpo:：value<int>（）->默认值（10），“每次清理调用的最大连接清理时间（毫秒）”。
         （“网络版本匹配”，bpo:：value<bool>（）->默认值（false），
           “如果为true，则要求对等网络版本完全匹配。”）
         （“同步获取范围”，bpo:：value<uint32_t>（）->default_value（def_sync_fetch_span），“同步期间从任何单个对等机获取的块数”）。
         （“使用socket read watermark”，bpo:：value<bool>（）->默认值（false），“启用expirimental socket read watermark优化”）
         （“对等日志格式”，bpo:：value<string>（）->默认值（“[\”$\“$U port]），
           “记录有关对等点的消息时用于格式化对等点的字符串。变量用$<variable name>转义。\n“
           “可用变量：\n”
           “_name \tself reported name \n \n”
           “_id\tself reported id（64个十六进制字符）\n\n”
           “_sid \t第一个8个字符的_peer.id \n \n”
           “删除对等机的IP地址\n \n”
           “_port \t删除对等机的端口号\n \n”
           “连接到对等机的本地IP地址\n \n”
           “连接到对等机的端口号\n \n”）
        ；
   }

   模板<typename t>
   t dejsonify（const string&s）
      从_字符串返回fc:：json:：as<t>（）；
   }

   void net_plugin:：plugin_initialize（常量变量_map&options）
      ilog（“初始化网络插件”）；
      尝试{
         peer_log_format=options.at（“peer log format”）.as<string>（）；

         my->network_version_match=options.at（“network version match”）.as<bool>（）；

         my->sync_master.reset（new sync_manager（options.at（“sync fetch span”）.as<uint32_t>（））；
         my->dispatcher.reset（新的调度管理器）；

         my->connector_period=std:：chrono:：seconds（options.at（“connection cleanup period”）.as<int>（））；
         my->max_cleanup_time_ms=options.at（“max cleanup time msec”）.as<int>（）；
         my->txn_exp_period=def_txn_expire_wait；
         my->resp_expected_period=def_resp_expected_wait；
         my->max_client_count=options.at（“max clients”）.as<int>（）；
         my->max_nodes_per_host=options.at（“p2p max nodes per host”）.as<int>（）；
         my->num_clients=0；
         my->started_sessions=0；

         my->use_socket_read_watermark=options.at（“use socket read watermark”）.as<bool>（）；

         my->resolver=std:：make_shared<tcp:：resolver>（std:：ref（app（）.get_io_service（））；
         if（options.count（“p2p listen endpoint”）&&options.at（“p2p listen endpoint”）.as<string>（）.length（））
            my->p2p_address=options.at（“p2p listen endpoint”）.as<string>（）；
            auto-host=my->p2p_address.substr（0，my->p2p_address.find（'：'）；
            auto-port=my->p2p_address.substr（host.size（）+1，my->p2p_address.size（））；
            idump（（主机）（端口））；
            tcp:：resolver:：query查询（tcp:：v4（），host.c_str（），port.c_str（））；
            //注意：还需要添加对IPv6的支持吗？

            my->listen_endpoint=*my->resolver->resolve（查询）；

            my->acceptor.reset（new tcp:：acceptor（app（）.get_io_service（））；

            if（options.count（“p2p服务器地址”））
               my->p2p_address=options.at（“p2p server address”）.as<string>（）；
            }否则{
               if（my->listendpoint.address（）.to_v4（）==address_v4:：any（））
                  boost：：系统：：错误代码EC；
                  自动主机=主机名（EC）；
                  如果（ec.value（）！=boost:：system:：errc:：success）

                     fc_throw_exception（fc：：无效的_arg_exception，
                                       “无法检索主机名。$msg“，（”msg“，ec.message（））；

                  }
                  auto-port=my->p2p_address.substr（my->p2p_address.find（'：'），my->p2p_address.size（））；
                  my->p2p_地址=主机+端口；
               }
            }
         }

         if（options.count（“p2p对等地址”））
            my->supplied_peers=options.at（“p2p peer address”）.as<vector<string>>（）；
         }
         if（options.count（“代理名称”））
            my->user_agent_name=options.at（“agent name”）.as<string>（）；
         }

         if（options.count（“allowed connection”））
            const std:：vector<std:：string>allowed_remotes=options[“allowed connection”]。
            for（const std:：string&allowed_remote:allowed_remotes）
               if（allowed_remote==“任何”）
                  my->allowed_connections=net_plugin_impl:：any；
               else if（allowed_remote==“生产者”）
                  my->allowed_connections=net_plugin_impl:：producers；
               else if（allowed_remote==“指定”）
                  my->allowed_connections=net_plugin_impl:：specified；
               else if（allowed_remote==“无”）
                  my->allowed_connections=net_plugin_impl:：none；
            }
         }

         if（my->allowed_connections&net_plugin_impl:：specified）
            eos_断言（options.count（“peer key”），
                        插件配置异常，
                       “必须至少有一个对等密钥与'allowed connection=specified'”）；

         if（options.count（“peer key”））
            const std:：vector<std:：string>key_strings=options[“peer key”]。
            for（const std:：string&key_string:key_strings）
               my->allowed_peers.push_back（dejsonify<chain:：public_key_type>（key_string））；
            }
         }

         if（options.count（“对等私钥”））
            const std:：vector<std:：string>key_id_to_wif_pair_strings=options[“peer private key”]。
            for（const std:：string&key_id_to_wif_pair_string:key_id_to_wif_pair_strings）
               auto key_id_to_wif_pair=dejsonify<std:：pair<chain:：public_key_type，std:：string>>（
                     键“id”到“wif”对“string”）；
               my->private_key[key_id_to_wif_pair.first]=fc:：crypto:：private_key（key_id_to_wif_pair.second）；
            }
         }

         my->chain_plug=app（）。查找_plugin<chain_plugin>（）；
         eos_断言（my->chain_plug，chain:：missing_chain_plugin_exception，“”）；
         my->chain_id=my->chain_plug->get_chain_id（）；
         fc:：rand_pseudo_bytes（my->node_id.data（），my->node_id.data_size（））；
         ilog（“我的节点_id是$id”，“id”，“my->node_id”）；

         my->keepalive_timer.reset（new boost:：asio:：steady_timer（app（）.get_io_service（））；
         我的-TcCar（）；
      fc_log_和_rethrow（））
   }

   void net_plugin:：plugin_startup（）
      my->producer_plug=app（）。查找_plugin<producer_plugin>（）；
      如果（my->acceptor）
         my->acceptor->open（my->listen_endpoint.protocol（））；
         my->acceptor->set_option（tcp:：acceptor:：reuse_address（true））；
         尝试{
           my->acceptor->bind（my->listen_endpoint）；
         catch（const std:：exception&e）
           ilog（“net_plugin:：plugin_startup未能绑定到端口$端口”，
             （“port”，my->listen_endpoint.port（））；
           投掷E；
         }
         my->acceptor->listen（）；
         ilog（“Starting Listener，max clients is$mc”，“mc”，“my->max_client_count”）；
         my->start_listen_loop（）；
      }
      chain:：controller&cc=my->chain_plug->chain（）；
      {
         cc.accepted_block.connect（boost:：bind（&net_plugin_impl:：accepted_block，my.get（），_1））；
      }

      my->incoming_transaction_ack_subscription=app（）.get_channel<channels:：transaction_ack>（）.subscript（boost:：bind（&net_plugin_impl:：transaction_ack，my.get（），_1））；

      if（cc.get_read_mode（）==chain:：db_read_mode:：read_only）
         my->max_nodes_per_host=0；
         ilog（“只读模式下的节点，将每个主机的最大节点数设置为0以防止连接”）；
      }

      my->start_monitors（）；

      for（auto seed_node:my->supplied_peers）
         连接（种子节点）；
      }

      如果（fc:：get_logger_map（）.find（logger_name）！=fc:：get_logger_map（）.end（））
         logger=fc:：get_logger_map（）[logger_name]；
   }

   void net_plugin:：plugin_shutdown（）
      尝试{
         ILOG（“停机”）；
         my->done=true；
         如果（my->acceptor）
            ILOG（“关闭接受方”）；
            my->acceptor->close（）；

            ilog（“close$s connections”，“s”，“my->connections.size（））；
            auto-cons=my->connections；
            用于（auto-con:cons）
               my->关闭（con）；
            }

            my->acceptor.reset（空指针）；
         }
         ILOG（“出口关闭”）；
      }
      fc_capture_和_rethrow（）。
   }

   大小_t net_plugin:：num_peers（）const_
      返回my->count_open_sockets（）；
   }

   /**
    *用于触发来自rpc api的新连接
    **/

   string net_plugin::connect( const string& host ) {
      if( my->find_connection( host ) )
         return "already connected";

      connection_ptr c = std::make_shared<connection>(host);
      fc_dlog(logger,"adding new connection to the list");
      my->connections.insert( c );
      fc_dlog(logger,"calling active connector");
      my->connect( c );
      return "added connection";
   }

   string net_plugin::disconnect( const string& host ) {
      for( auto itr = my->connections.begin(); itr != my->connections.end(); ++itr ) {
         if( (*itr)->peer_addr == host ) {
            (*itr)->reset();
            my->close(*itr);
            my->connections.erase(itr);
            return "connection removed";
         }
      }
      return "no known connection for host";
   }

   optional<connection_status> net_plugin::status( const string& host )const {
      auto con = my->find_connection( host );
      if( con )
         return con->get_status();
      return optional<connection_status>();
   }

   vector<connection_status> net_plugin::connections()const {
      vector<connection_status> result;
      result.reserve( my->connections.size() );
      for( const auto& c : my->connections ) {
         result.push_back( c->get_status() );
      }
      return result;
   }
   connection_ptr net_plugin_impl::find_connection(const string& host )const {
      for( const auto& c : connections )
         if( c->peer_addr == host ) return c;
      return connection_ptr();
   }

   uint16_t net_plugin_impl::to_protocol_version(uint16_t v) {
      if (v >= net_version_base) {
         v -= net_version_base;
         return (v > net_version_range) ? 0 : v;
      }
      return 0;
   }

}

