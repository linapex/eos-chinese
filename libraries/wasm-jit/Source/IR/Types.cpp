
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457046003027968>

#include "Types.h"

#include <map>

namespace IR
{
	struct FunctionTypeMap
	{
		struct Key
		{
			ResultType ret;
			std::vector<ValueType> parameters;

			friend bool operator==(const Key& left,const Key& right) { return left.ret == right.ret && left.parameters == right.parameters; }
			friend bool operator!=(const Key& left,const Key& right) { return left.ret != right.ret || left.parameters != right.parameters; }
			friend bool operator<(const Key& left,const Key& right) { return left.ret < right.ret || (left.ret == right.ret && left.parameters < right.parameters); }
		};
		static std::map<Key,FunctionType*>& get()
		{
			static std::map<Key,FunctionType*> map;
			return map;
		}
	};

	template<typename Key,typename Value,typename CreateValueThunk>
	Value findExistingOrCreateNew(std::map<Key,Value>& map,Key&& key,CreateValueThunk createValueThunk)
	{
		auto mapIt = map.find(key);
		if(mapIt != map.end()) { return mapIt->second; }
		else
		{
			Value value = createValueThunk();
			map.insert({std::move(key),value});
			return value;
		}
	}

	const FunctionType* FunctionType::get(ResultType ret,const std::initializer_list<ValueType>& parameters)
	{ return findExistingOrCreateNew(FunctionTypeMap::get(),FunctionTypeMap::Key {ret,parameters},[=]{return new FunctionType(ret,parameters);}); }
	const FunctionType* FunctionType::get(ResultType ret,const std::vector<ValueType>& parameters)
	{ return findExistingOrCreateNew(FunctionTypeMap::get(),FunctionTypeMap::Key {ret,parameters},[=]{return new FunctionType(ret,parameters);}); }
	const FunctionType* FunctionType::get(ResultType ret)
	{ return findExistingOrCreateNew(FunctionTypeMap::get(),FunctionTypeMap::Key {ret,{}},[=]{return new FunctionType(ret,{});}); }
}

