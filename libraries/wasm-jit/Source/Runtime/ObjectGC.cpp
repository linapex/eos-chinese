
//<developer>
//    <name>linapex 曹一峰</name>
//    <email>linapex@163.com</email>
//    <wx>superexc</wx>
//    <qqgroup>128148617</qqgroup>
//    <url>https://jsq.ink</url>
//    <role>pku engineer</role>
//    <date>2019-03-16 19:44:16</date>
//</624457047068381184>

#include "Inline/BasicTypes.h"
#include "Runtime.h"
#include "RuntimePrivate.h"
#include "Intrinsics.h"

#include <set>
#include <vector>

namespace Runtime
{
//保留所有对象的全局列表。
	struct GCGlobals
	{
		std::set<GCObject*> allObjects;

		static GCGlobals& get()
		{
			static GCGlobals globals;
			return globals;
		}
		
	private:
		GCGlobals() {}
	};

	GCObject::GCObject(ObjectKind inKind): ObjectInstance(inKind)
	{
//将对象添加到全局数组。
		GCGlobals::get().allObjects.insert(this);
	}

	GCObject::~GCObject()
	{
//从全局数组中移除对象。
		GCGlobals::get().allObjects.erase(this);
	}

	void freeUnreferencedObjects(std::vector<ObjectInstance*>&& rootObjectReferences)
	{
		std::set<ObjectInstance*> referencedObjects;
		std::vector<ObjectInstance*> pendingScanObjects;

//从运行的WASM线程收集GC根。
		getThreadGCRoots(rootObjectReferences);

//从rootObjectReferences和内部对象初始化referencedObjects集。
		for(auto object : rootObjectReferences)
		{
			if(object && !referencedObjects.count(object))
			{
				referencedObjects.insert(object);
				pendingScanObjects.push_back(object);
			}
		}

		const std::vector<ObjectInstance*> intrinsicObjects = Intrinsics::getAllIntrinsicObjects();
		for(auto object : intrinsicObjects)
		{
			if(object && !referencedObjects.count(object))
			{
				referencedObjects.insert(object);
				pendingScanObjects.push_back(object);
			}
		}

//扫描到目前为止添加到引用集的对象：收集其子引用并递归。
		while(pendingScanObjects.size())
		{
			ObjectInstance* scanObject = pendingScanObjects.back();
			pendingScanObjects.pop_back();

//根据此对象的类型收集其子引用。
			std::vector<ObjectInstance*> childReferences;
			switch(scanObject->kind)
			{
			case ObjectKind::function:
			{
				FunctionInstance* function = asFunction(scanObject);
				childReferences.push_back(function->moduleInstance);
				break;
			}
			case ObjectKind::module:
			{
				ModuleInstance* moduleInstance = asModule(scanObject);
				childReferences.insert(childReferences.begin(),moduleInstance->functionDefs.begin(),moduleInstance->functionDefs.end());
				childReferences.insert(childReferences.begin(),moduleInstance->functions.begin(),moduleInstance->functions.end());
				childReferences.insert(childReferences.begin(),moduleInstance->tables.begin(),moduleInstance->tables.end());
				childReferences.insert(childReferences.begin(),moduleInstance->memories.begin(),moduleInstance->memories.end());
				childReferences.insert(childReferences.begin(),moduleInstance->globals.begin(),moduleInstance->globals.end());
				childReferences.push_back(moduleInstance->defaultMemory);
				childReferences.push_back(moduleInstance->defaultTable);
				break;
			}
			case ObjectKind::table:
			{
				TableInstance* table = asTable(scanObject);
				childReferences.insert(childReferences.end(),table->elements.begin(),table->elements.end());
				break;
			}
			case ObjectKind::memory:
			case ObjectKind::global: break;
			default: Errors::unreachable();
			};

//将对象的子引用添加到引用集，并将其排队进行扫描。
			for(auto reference : childReferences)
			{
				if(reference && !referencedObjects.count(reference))
				{
					referencedObjects.insert(reference);
					pendingScanObjects.push_back(reference);
				}
			}
		};

//遍历所有对象，并删除根集没有直接或间接引用的对象。
		GCGlobals& gcGlobals = GCGlobals::get();
		auto objectIt = gcGlobals.allObjects.begin();
		while(objectIt != gcGlobals.allObjects.end())
		{
			if(referencedObjects.count(*objectIt)) { ++objectIt; }
			else
			{
				ObjectInstance* object = *objectIt;
				objectIt = gcGlobals.allObjects.erase(objectIt);
				delete object;
			}
		}
	}
}

