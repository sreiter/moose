// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include "error.h"
#include "json_archive.h"
#include "rapidjson/istreamwrapper.h"

using namespace std;

namespace moose{
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
	MOOSE_CTHROW (!in, "File not found: " << filename);

	doc_t& d = new_document ();
	rapidjson::IStreamWrapper inWrapper(in);
	if(d.ParseStream (inWrapper).HasParseError ())
		throw runtime_error (string("JSON Parse error in file: ").append(filename));

	m_entries.push (Entry (&d, "_root_"));
}

void JSONArchive::begin_read (const char* name)
{
	MOOSE_CTHROW (m_entries.empty(),
				  "End of file reached. Couldn't read field '" << name << "'");

	Entry& e = m_entries.top();
	if(e.iter_valid()
	   && (e.icur->value.IsArray()
	       || (strcmp(name, e.icur->name.GetString()) == 0)))
	{
	}
	else {
		if(e.value().IsArray()) {
			MOOSE_CTHROW(strlen(name) > 0, "Array elements can't have a name! ('"
			             << name << "')");
			e.icur = e.value().MemberBegin();
		}
		else{
			e.icur = e.value().FindMember (name);
		}
	}

	MOOSE_CTHROW (!e.iter_valid(), "No entry with name '" << name
		          << "' found in current object '" << e.name << "'.");

	cout << "<dbg> pushing entry '" << e.icur->name.GetString() << "'\n";
	m_entries.push(Entry(&e.icur->value, e.icur->name.GetString()));

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
	MOOSE_CTHROW (m_entries.empty(),
	              "JSONArchive::end_read called on empty stack for entry '"
				  << name << "'");

	cout << "<dbg> end read of object '" << name << "'" << endl;

	m_entries.pop();

	if(!m_entries.empty()){
		++m_entries.top().icur;
	}
}

std::string JSONArchive::get_type_name ()
{
	MOOSE_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	val_t& value = *m_entries.top().val;
	if(value.HasMember("_type_")){
		cout << "<dbg> returning Typename: " << value["_type_"].GetString() << endl;
		return value["_type_"].GetString();
	}
	else
		return "_unknown_";
}

void JSONArchive::read (const char* name, double& val)
{
	MOOSE_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	Entry& e = m_entries.top();
	cout << "<dbg> reading field '" << e.name << "' as double" << endl;
	val = e.val->GetDouble();
}


void JSONArchive::read (const char* name, std::string& val)
{
	MOOSE_CTHROW (m_entries.empty(), "JSONArchive::read: entry stack empty!")

	Entry& e = m_entries.top();
	cout << "<dbg> reading field '" << e.name << "' as string" << endl;
	val = e.val->GetString();
}

}//	end of namespace moose
