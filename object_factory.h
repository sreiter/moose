// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_object_factory
#define __H__moose_object_factory

#include <map>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>
#include "serialize.h"

namespace moose{

class Archive;


class ObjectFactory {
public:

	template <class T> static
	void register_type (std::string name)
	{
		add_entry<T> (name);
	}

	template <class T, class TBase1, class... TBaseOthers> static
	void register_type (std::string name)
	{
		Entry& e = add_entry<T>(name);
		add_base_class<TBase1, TBaseOthers ...> (e);
	}

	template <class TBase> static
	TBase* create (const std::string& name)
	{
		using namespace std;
		entry_map_t::iterator i = entry_map().find(name);
		if(i == entry_map().end()){
			throw runtime_error(
					string("Can't create object for unregistered type '")
						.append(name).append("'"));
		}

		Entry& e = i->second;

		if(!e.createFnc)
			throw runtime_error(string("Can't create instance of abstract type '")
					.append(name).append("'"));

	//	Make sure that we may convert the type we intend to create to 'TBase*'
		const std::string& baseName = get_typename<TBase> ();
		if(baseName.empty())
			throw runtime_error(string("Can't convert type '").append(name)
					.append("' to unregistered type '")
					.append(typeid(TBase).name()).append("'"));


		if((name != baseName) && !is_base(e, baseName))
			throw runtime_error(string("Can't convert type '").append(name)
					.append("' to type '").append(baseName).append("' "));

	//todo:	this may cause problem in cases of multiple inheritance.
	//		Think abount storing cast-methods between types and registered base classes.
		return reinterpret_cast<TBase*> (e.createFnc());
	}

	template <class TBase> static
	void call_serialize (const std::string& name, Archive& ar, TBase* b)
	{
		using namespace std;
		entry_map_t::iterator i = entry_map().find(name);
		if(i == entry_map().end()){
			throw runtime_error(
					string("Can't create object for unregistered type '")
						.append(name).append("'"));
		}

		Entry& e = i->second;
		e.serializeFnc(ar, b);
	}

private:
	typedef void* (*create_fnc_t)();
	typedef void (*serialize_fnc_t)(Archive&, void*);

	struct Entry {
		create_fnc_t				createFnc;
		serialize_fnc_t				serializeFnc;
		std::string					name;
		std::vector<std::string>	baseClasses;
	};

	typedef std::map<std::string, Entry>		entry_map_t;
	typedef std::map<std::size_t, std::string>	typename_map_t;

	template <class T> static void* CreateFunc ()	{return new T;}
	template <class T> static void CallSerialize (Archive& ar, void* val)
	{
		Serialize (ar, *reinterpret_cast<T*>(val));
	}

	inline static ObjectFactory& inst ()			{static ObjectFactory of; return of;}
	inline static entry_map_t& entry_map ()			{return inst().m_entryMap;}
	inline static typename_map_t& typename_map ()	{return inst().m_typenameMap;}

	template <class T>
	inline static std::string& get_typename ()		{return typename_map()[typeid(T).hash_code()];}

	inline static bool is_base (Entry& e, const std::string& baseName)
	{
		for(auto& name : e.baseClasses) {
			if(name == baseName)
				return true;

			Entry& e = entry_map()[name];
			if(is_base(e, baseName))
				return true;
		}
		return false;
	}

	template <class T>
	static typename std::enable_if <!std::is_abstract <T>::value, Entry&>::type
	add_entry (std::string name)
	{
		Entry& e = entry_map()[name];
		typename_map()[typeid(T).hash_code()] = name;
		e.name = name;
		e.createFnc = &CreateFunc<T>;
		e.serializeFnc = &CallSerialize<T>;
		return e;
	}

	template <class T>
	static typename std::enable_if <std::is_abstract <T>::value, Entry&>::type
	add_entry (std::string name)
	{
		Entry& e = entry_map()[name];
		typename_map()[typeid(T).hash_code()] = name;
		e.name = name;
		e.createFnc = NULL;
		e.serializeFnc = &CallSerialize<T>;
		return e;
	}

	template <class T> static
	void add_base_class (Entry& e)
	{
		using namespace std;
		const std::string& baseName = get_typename<T>();
		if(baseName.empty())
			throw runtime_error(
					string("Can't associate unregistered base class of type '")
						.append(typeid(T).name())
						.append("' with newly registered type '")
						.append(e.name).append("'"));

		e.baseClasses.push_back(baseName);
	}

	template <class HEAD, class TBase2, class... TBaseOthers> static
	void add_base_class (Entry& e)
	{
		add_base_class <HEAD> (e);
		add_base_class <TBase2, TBaseOthers...> (e);
	}


	ObjectFactory()			{}

	entry_map_t 	m_entryMap;
	typename_map_t	m_typenameMap;

};



template <class T>
void RegisterType (std::string name)
{
	ObjectFactory::register_type<T>(name);
}

template <class T, class TBase1, class... TBaseOthers>
void RegisterType (std::string name)
{
	ObjectFactory::register_type<T, TBase1, TBaseOthers...>(name);
}


}//	end of namespace moose



#endif	//__H__moose_object_factory
