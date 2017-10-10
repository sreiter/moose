// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_json_archive
#define __H__moose_json_archive

#include <stack>
#include <memory>
#include "archive.h"
#include "rapidjson/document.h"


namespace moose{

class JSONArchive : public Archive {
public:
	JSONArchive ();
	virtual ~JSONArchive ()	{}

	void parse_file (const char* filename);

protected:
	virtual void begin_read (const char* name);
	virtual void end_read (const char* name);

	virtual void begin_array_read (const char* name);
	virtual bool array_has_next (const char* name);
	virtual void end_array_read (const char* name);

	virtual std::string get_type_name ();
	
	virtual void read (const char* name, double& val);
	virtual void read (const char* name, std::string& val);

private:
	typedef rapidjson::Value val_t;
	typedef val_t::Member mem_t;
	typedef rapidjson::Document	doc_t;
	typedef rapidjson::Value::MemberIterator iter_t;
	typedef std::pair<iter_t, iter_t> iter_pair_t;

	doc_t& new_document ();

	struct Entry {
		Entry () :
			val (nullptr),
			name ("")
		{}

		Entry (val_t* _val, const char* _name) :
			val (_val),
			name (_name)
		{
			if(_val->IsObject() || _val->IsArray())
				icur = _val->MemberEnd();
		}

		bool iter_valid () const	{return val && (icur != val->MemberEnd());}
		bool value_valid () const	{return val != nullptr;}
		val_t& value ()				{return *val;}
		const val_t& value () const	{return *val;}

		val_t*	val;
		iter_t	icur;
		const char* name;
	};

	std::stack<Entry>		m_entries;
	std::unique_ptr<doc_t> 	m_doc;
	std::stack<iter_t>		m_members;
	std::stack<iter_pair_t>	m_iterators;
};

}//	end of namespace moose

#endif	//__H__moose_json_archive
