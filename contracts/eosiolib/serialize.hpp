
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:13</date>
//</624457032728055808>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>

#define EOSLIB_REFLECT_MEMBER_OP( r, OP, elem ) \
  OP t.elem 

/*
 *@defgroup serialize序列化API
 *@brief定义要序列化和反序列化对象的函数
 *@ingroup contractdev公司
 **/


/*
 *@ DePixSerialIZEPP序列化C++ API
 *@简要定义C++ API以序列化和反序列化对象
 *@ingroup系列
 *@
 **/


/*
 *定义类的序列化和反序列化
 *
 *@brief定义类的序列化和反序列化
 *
 *@param type-定义其序列化和反序列化的类
 *@param members-成员名称的序列。（字段1）（字段2）（字段3）
 **/

#define EOSLIB_SERIALIZE( TYPE,  MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    return ds BOOST_PP_SEQ_FOR_EACH( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    return ds BOOST_PP_SEQ_FOR_EACH( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 } 

/*
 *为从其他类继承的类定义序列化和反序列化
 *定义其序列化和反序列化
 *
 *@brief为从其他类继承的类定义序列化和反序列化
 *定义其序列化和反序列化
 *
 *@param type-定义其序列化和反序列化的类
 *@param base-基类名的序列（base a）（baseb）（basec）
 *@param members-成员名称的序列。（字段1）（字段2）（字段3）
 **/

#define EOSLIB_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << static_cast<const BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    ds >> static_cast<BASE&>(t); \
    return ds BOOST_PP_SEQ_FOR_EACH( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 } 
///@序列化cpp

