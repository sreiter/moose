// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_object_factory
#define __H__moose_object_factory

namespace moose{

class ObjectFactory {
public:
	template <class T> static
	void register_type (const char* name)
	{
		register_type<T> (std::string(name));
	}

	template <class T> static
	void register_type (const std::string& name)
	{
		inst().m_createMap[name] = &CreateFunc<T>;
	}

	template <class TBase> static
	TBase* create (const std::string& name)
	{
		using namespace std;
		create_map_t::iterator i = create_map().find(name);
		if(i == create_map().end()){
			throw runtime_error(
					string("Can't create object for unregistered type '")
						.append(name).append("'"));
		}

	///todo: add a mechanism to check whether cast is valid
		return reinterpret_cast<TBase*> (i->second());
	}

private:
	typedef void* (*create_fnc_t)();
	typedef std::map<std::string, create_fnc_t>	create_map_t;

	template <class T> static void* CreateFunc ()	{return new T;}

	inline static ObjectFactory& inst ()		{static ObjectFactory of; return of;}
	inline static create_map_t& create_map ()	{return inst().m_createMap;}

	ObjectFactory()			{}

	create_map_t m_createMap;
};

}//	end of namespace moose

#endif	//__H__moose_object_factory
