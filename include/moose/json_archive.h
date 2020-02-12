// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_json_archive
#define __H__moose_json_archive

#include <stack>
#include <memory>
#include <moose/archive.h>
#include <rapidjson/document.h>


namespace moose {

namespace impl {
struct JSONEntry {
	typedef rapidjson::Value val_t;

	JSONEntry ();
	JSONEntry (val_t* _val, const char* _name);

	void init_iter (const char* name);

	bool iter_valid () const;

	bool value_valid () const;

	val_t& value ();
	const char* name ();

	val_t& iter_value ();
	const char* iter_name ();

	void advance ();

	bool is_object ()	{return m_type == Object;}
	bool is_array ()	{return m_type == Array;}
	bool is_value ()	{return m_type == Value;}

private:
	typedef rapidjson::Value::MemberIterator mem_iter_t;
	typedef rapidjson::Value::ValueIterator val_iter_t;

	enum Type {Object, Array, Value};

	val_t*		m_val;
	mem_iter_t	m_icurMem;
	val_iter_t	m_icurVal;
	const char* m_name;
	Type 		m_type;
};
}


class JSONArchive : public Archive {
public:
	JSONArchive ();
	virtual ~JSONArchive ()	{}

	void parse_file (const char* filename);
  void parse_string (const char* filename);

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
	typedef rapidjson::Value::MemberIterator mem_iter_t;
	typedef rapidjson::Value::ValueIterator val_iter_t;
	typedef impl::JSONEntry entry_t;

	doc_t& new_document ();

	std::stack<entry_t>	m_entries;
	std::unique_ptr<doc_t> m_doc;
};

}//	end of namespace moose

#endif	//__H__moose_json_archive
