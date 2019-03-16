
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:15</date>
//</624457042278486016>

/*
 *@文件
 *@eos/license中定义的版权
 **/

#pragma once

#include <boost/range/combine.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace eosio { namespace chain {

/*
 *@brief返回与匹配标记对应的datarange中的值
 *
 *采用两个平行范围，一个包含数据值的数据范围，以及一个包含上的标记的标记范围。
 *对应的数据值。返回只包含与匹配的标记对应的值的新数据区域
 *标记值
 *
 *例如：
 *@代码{ CPP}
 *vector<char>data='a'、'b'、'c'
 *vector<bool>markers=真，假，真
 *auto markeddata=filterdatabymarker（数据，标记，真）；
 *//markeddata包含'a'、'c'
 *@终结码
 **/

template<typename DataRange, typename MarkerRange, typename Marker>
DataRange filter_data_by_marker(DataRange data, MarkerRange markers, const Marker& markerValue) {
   auto remove_mismatched_markers = boost::adaptors::filtered([&markerValue](const auto& tuple) {
      return boost::get<0>(tuple) == markerValue;
   });
   auto ToData = boost::adaptors::transformed([](const auto& tuple) {
      return boost::get<1>(tuple);
   });

//将范围压缩在一起，用不匹配的标记筛选出数据，并返回不带标记的数据
   auto range = boost::combine(markers, data) | remove_mismatched_markers | ToData;
   return {range.begin(), range.end()};
}

}} //命名空间eosio:：chain


