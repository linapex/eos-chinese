
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457045537460224>

#pragma once

#include "IR/Types.h"
#include "Inline/Floats.h"

#include <string.h>

#undef ENABLE_SIMD_PROTOTYPE

namespace Runtime
{
//任何类型的运行时值。
	struct UntaggedValue
	{
		union
		{
			I32 i32;
			U32 u32;
			I64 i64;
			U64 u64;
			F32 f32;
			F64 f64;
			#if ENABLE_SIMD_PROTOTYPE
			IR::V128 v128;
			#endif
		};
		
		UntaggedValue(I32 inI32) { i32 = inI32; }
		UntaggedValue(I64 inI64) { i64 = inI64; }
		UntaggedValue(U32 inU32) { u32 = inU32; }
		UntaggedValue(U64 inU64) { u64 = inU64; }
		UntaggedValue(F32 inF32) { f32 = inF32; }
		UntaggedValue(F64 inF64) { f64 = inF64; }
		#if ENABLE_SIMD_PROTOTYPE
		UntaggedValue(IR::V128 inV128) { v128 = inV128; }
		#endif
		UntaggedValue() {memset(this,0,sizeof(*this));}
	};

//装箱值：可以保存可以传递给通过运行时调用的函数的任何值。
	struct Value : UntaggedValue
	{
		IR::ValueType type;

		Value(I32 inI32): UntaggedValue(inI32), type(IR::ValueType::i32) {}
		Value(I64 inI64): UntaggedValue(inI64), type(IR::ValueType::i64) {}
		Value(U32 inU32): UntaggedValue(inU32), type(IR::ValueType::i32) {}
		Value(U64 inU64): UntaggedValue(inU64), type(IR::ValueType::i64) {}
		Value(F32 inF32): UntaggedValue(inF32), type(IR::ValueType::f32) {}
		Value(F64 inF64): UntaggedValue(inF64), type(IR::ValueType::f64) {}
		#if ENABLE_SIMD_PROTOTYPE
		Value(const IR::V128& inV128): UntaggedValue(inV128), type(IR::ValueType::v128) {}
		#endif
		Value(IR::ValueType inType,UntaggedValue inValue): UntaggedValue(inValue), type(inType) {}
		Value(): type(IR::ValueType::any) {}
		
		friend std::string asString(const Value& value)
		{
			switch(value.type)
			{
			case IR::ValueType::i32: return "i32(" + std::to_string(value.i32) + ")";
			case IR::ValueType::i64: return "i64(" + std::to_string(value.i64) + ")";
			case IR::ValueType::f32: return "f32(" + Floats::asString(value.f32) + ")";
			case IR::ValueType::f64: return "f64(" + Floats::asString(value.f64) + ")";
			#if ENABLE_SIMD_PROTOTYPE
			case IR::ValueType::v128: return "v128(" + std::to_string(value.v128.u64[0]) + "," + std::to_string(value.v128.u64[1]) + ")";
			#endif
			default: Errors::unreachable();
			}
		}
	};

//装箱值：可以保存任何可以从运行时调用的函数返回的值。
	struct Result : UntaggedValue
	{
		IR::ResultType type;

		Result(I32 inI32): UntaggedValue(inI32), type(IR::ResultType::i32) {}
		Result(I64 inI64): UntaggedValue(inI64), type(IR::ResultType::i64) {}
		Result(U32 inU32): UntaggedValue(inU32), type(IR::ResultType::i32) {}
		Result(U64 inU64): UntaggedValue(inU64), type(IR::ResultType::i64) {}
		Result(F32 inF32): UntaggedValue(inF32), type(IR::ResultType::f32) {}
		Result(F64 inF64): UntaggedValue(inF64), type(IR::ResultType::f64) {}
		#if ENABLE_SIMD_PROTOTYPE
		Result(const IR::V128& inV128): UntaggedValue(inV128), type(IR::ResultType::v128) {}
		#endif
		Result(IR::ResultType inType,UntaggedValue inValue): UntaggedValue(inValue), type(inType) {}
		Result(const Value& inValue): UntaggedValue(inValue), type(asResultType(inValue.type)) {}
		Result(): type(IR::ResultType::none) {}

		friend std::string asString(const Result& result)
		{
			switch(result.type)
			{
			case IR::ResultType::none: return "()";
			case IR::ResultType::i32: return "i32(" + std::to_string(result.i32) + ")";
			case IR::ResultType::i64: return "i64(" + std::to_string(result.i64) + ")";
			case IR::ResultType::f32: return "f32(" + Floats::asString(result.f32) + ")";
			case IR::ResultType::f64: return "f64(" + Floats::asString(result.f64) + ")";
			#if ENABLE_SIMD_PROTOTYPE
			case IR::ResultType::v128: return "v128(" + std::to_string(result.v128.u64[0]) + "," + std::to_string(result.v128.u64[1]) + ")";
			#endif
			default: Errors::unreachable();
			}
		}
	};

//比较同一类型的两个未标记值是否具有相同的位。
	inline bool areBitsEqual(IR::ResultType type,UntaggedValue a,UntaggedValue b)
	{
		switch(type)
		{
		case IR::ResultType::i32:
		case IR::ResultType::f32: return a.i32 == b.i32;
		case IR::ResultType::i64:
		case IR::ResultType::f64: return a.i64 == b.i64;
		#if ENABLE_SIMD_PROTOTYPE
		case IR::ResultType::v128: return a.v128.u64[0] == b.v128.u64[0] && a.v128.u64[1] == b.v128.u64[1];
		#endif
		case IR::ResultType::none: return true;
		default: Errors::unreachable();
		};
	}

//比较两个值/结果是否具有相同的类型和位。
	inline bool areBitsEqual(const Value& a,const Value& b) { return a.type == b.type && areBitsEqual(asResultType(a.type),a,b); }
	inline bool areBitsEqual(const Result& a,const Result& b) { return a.type == b.type && areBitsEqual(a.type,a,b); }
	inline bool areBitsEqual(const Result& a,const Value& b) { return a.type == asResultType(b.type) && areBitsEqual(a.type,a,b); }
	inline bool areBitsEqual(const Value& a,const Result& b) { return asResultType(a.type) == b.type && areBitsEqual(b.type,a,b); }
}

