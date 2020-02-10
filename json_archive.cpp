// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include "exception_util.h"
#include "json_archive.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"

using namespace std;

namespace moose {

JSONArchive::JSONArchive () : Archive (true)
{}

JSONArchive::doc_t& JSONArchive::new_document ()
{
	m_doc = std::unique_ptr<doc_t>(new doc_t);
	return *m_doc;
}

void JSONArchive::parse_file (const char* filename)
{
	ifstream in (filename);
	MOOSE_AR_CTHROW (!in, "File not found: " << filename);

	doc_t& d = new_document ();
	rapidjson::IStreamWrapper inWrapper(in);
	rapidjson::ParseResult res = d.ParseStream (inWrapper);
	if(!res){
		//	we have to find the line where the error occurred using the
		//	read offset of 'res'.
		in.clear();
		in.seekg(0);
		string buf;
		int counter = 0;
		int curLine = 0;
		while(!in.eof() && (counter <= (int)res.Offset())){
			getline(in, buf, '\n');
			counter += (int)buf.size() + 1; //count line-ending
			++curLine;
		}

		//	remove cr from windows line endings
		if(!buf.empty() && buf[buf.size() - 1] == '\r')
			buf.resize(buf.size() - 1);

		MOOSE_AR_THROW ("JSON Parse error in file '" << filename << "', line "
		    << curLine << ": " << rapidjson::GetParseError_En(res.Code())
		    << " ('" << buf << "')");
	}

	m_entries.push (entry_t (&d, "_root_"));
}

void JSONArchive::parse_string (const char* str)
{
  doc_t& d = new_document ();
  rapidjson::ParseResult res = d.Parse (str);
  if(!res){
    MOOSE_AR_THROW ("JSON Parse error in string '" << str << "': "
        << rapidjson::GetParseError_En(res.Code()));
  }

  m_entries.push (entry_t (&d, "_root_"));
}

void JSONArchive::begin_read (const char* name)
{
	MOOSE_AR_CTHROW (m_entries.empty(),
				  "End of file reached. Couldn't read field '" << name << "'");

	entry_t& e = m_entries.top();

//	if we're currently iterating over elements with the given name, we don't
//	have to initialize the iterators
	if(!e.iter_valid() || (strcmp(name, e.iter_name()) != 0))
	{
		e.init_iter(name);
	}

	MOOSE_AR_CTHROW (!e.iter_valid(), "No entry with name '" << name
		          << "' found in current object '" << e.name() << "'.");

	// cout << "<dbg> pushing entry '" << e.iter_name() << "'\n";
	m_entries.push(entry_t(&e.iter_value(), e.iter_name()));
	if(m_entries.top().is_array())
		m_entries.top().init_iter("");
}

void JSONArchive::begin_array_read (const char* name)
{
}

bool JSONArchive::array_has_next (const char* name)
{
	return m_entries.top().iter_valid();
}

void JSONArchive::end_array_read (const char* name)
{
}



void JSONArchive::end_read (const char* name)
{
	MOOSE_AR_CTHROW (m_entries.empty(),
	              "JSONArchive::end_read called on empty stack for entry '"
				  << name << "'");

	// cout << "<dbg> end read of object '" << name << "'" << endl;

	m_entries.pop();

	if(!m_entries.empty()){
		m_entries.top().advance();
	}
}

std::string JSONArchive::get_type_name ()
{
	MOOSE_AR_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	val_t& value = m_entries.top().value();
	if(value.HasMember("@type")){
		// cout << "<dbg> returning Typename: " << value["@type"].GetString() << endl;
		return value["@type"].GetString();
	}
	else
		return "_unknown_";
}

void JSONArchive::read (const char* name, double& val)
{
	MOOSE_AR_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	entry_t& e = m_entries.top();
	cout << "<dbg> reading field '" << e.name() << "' as double" << endl;
	val = e.value().GetDouble();
}


void JSONArchive::read (const char* name, std::string& val)
{
	MOOSE_AR_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	entry_t& e = m_entries.top();
	cout << "<dbg> reading field '" << e.name() << "' as string" << endl;
	val = e.value().GetString();
}


namespace impl {
JSONEntry::
JSONEntry () :
	m_val (nullptr),
	m_name (""),
	m_type (Value)
{}

JSONEntry::
JSONEntry (val_t* _val, const char* _name) :
	m_val (_val),
	m_name (_name)
{
	if(_val->IsObject()){
		m_type = Object;
		m_icurMem = _val->MemberEnd();
	}
	else if(_val->IsArray()){
		m_type = Array;
		m_icurVal = _val->End();
	}
	else
		m_type = Value;
}

void JSONEntry::
init_iter (const char* name)
{
	switch(m_type) {
		case Object: m_icurMem = m_val->FindMember (name); break;
		case Array: m_icurVal = m_val->Begin(); break;
		case Value: break;
	}
}

bool JSONEntry::
iter_valid () const
{
	switch(m_type) {
		case Object: return m_val && (m_icurMem != m_val->MemberEnd());
		case Array: return m_val && (m_icurVal != m_val->End());
		case Value: return false;
        default:
            MOOSE_AR_THROW("Invalid code path");
	}
}

bool JSONEntry::
value_valid () const
{
	return m_val != nullptr;
}

JSONEntry::val_t& JSONEntry::
value ()
{
	return *m_val;
}

const char* JSONEntry::
name ()
{
	return m_name;
}

JSONEntry::val_t& JSONEntry::
iter_value ()
{
	switch(m_type) {
		case Object: return m_icurMem->value;
		case Array: return *m_icurVal;
		case Value: MOOSE_AR_THROW("Values don't have iterators which could be accessed.");
        default:
            MOOSE_AR_THROW("Invalid code path");
	}
}

const char* JSONEntry::
iter_name ()
{
	switch(m_type) {
		case Object: return m_icurMem->name.GetString();
		case Array: return "";
		case Value: MOOSE_AR_THROW("Values don't have iterators which could be accessed.");
        default:
            MOOSE_AR_THROW("Invalid code path");
	}
}

void JSONEntry::
advance ()
{
	switch(m_type) {
		case Object: ++m_icurMem; break;
		case Array: ++m_icurVal; break;
		case Value: break;
	}
}
}//	end of namespace impl
}//	end of namespace moose
