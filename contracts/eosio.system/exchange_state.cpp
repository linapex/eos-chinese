
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:12</date>
//</624457030811258880>

#include <eosio.system/exchange_state.hpp>

namespace eosiosystem {
   asset exchange_state::convert_to_exchange( connector& c, asset in ) {

      real_type R(supply.amount);
      real_type C(c.balance.amount+in.amount);
      real_type F(c.weight/1000.0);
      real_type T(in.amount);
      real_type ONE(1.0);

      real_type E = -R * (ONE - std::pow( ONE + T / C, F) );
//打印（“e：”，e，“\n”）；
      int64_t issued = int64_t(E);

      supply.amount += issued;
      c.balance.amount += in.amount;

      return asset( issued, supply.symbol );
   }

   asset exchange_state::convert_from_exchange( connector& c, asset in ) {
      eosio_assert( in.symbol== supply.symbol, "unexpected asset symbol input" );

      real_type R(supply.amount - in.amount);
      real_type C(c.balance.amount);
      real_type F(1000.0/c.weight);
      real_type E(in.amount);
      real_type ONE(1.0);


//可能更准确：
//函数std:：expm1和std:：log1p对于财务计算很有用，例如，
//计算小额日利率时：（1+x）n
//-1可以表示为std:：expm1（n*std:：log1p（x））。
//实型t=c*std:：expm1（f*std:：log1p（e/r））；
      
      real_type T = C * (std::pow( ONE + E/R, F) - ONE);
//打印（“t：”，t，“\n”）；
      int64_t out = int64_t(T);

      supply.amount -= in.amount;
      c.balance.amount -= out;

      return asset( out, c.balance.symbol );
   }

   asset exchange_state::convert( asset from, symbol_type to ) {
      auto sell_symbol  = from.symbol;
      auto ex_symbol    = supply.symbol;
      auto base_symbol  = base.balance.symbol;
      auto quote_symbol = quote.balance.symbol;

//打印（“从：”，从，“到”，资产（0，到），“\n”）；
//print（“base：”，base_symbol，“\n”）；
//print（“引号：”，quote_symbol，“\n”）；
//打印（“ex：”，supply.symbol，“\n”）；

      if( sell_symbol != ex_symbol ) {
         if( sell_symbol == base_symbol ) {
            from = convert_to_exchange( base, from );
         } else if( sell_symbol == quote_symbol ) {
            from = convert_to_exchange( quote, from );
         } else { 
            eosio_assert( false, "invalid sell" );
         }
      } else {
         if( to == base_symbol ) {
            from = convert_from_exchange( base, from ); 
         } else if( to == quote_symbol ) {
            from = convert_from_exchange( quote, from ); 
         } else {
            eosio_assert( false, "invalid conversion" );
         }
      }

      if( to != from.symbol )
         return convert( from, to );

      return from;
   }



} ///namespace eosios系统

