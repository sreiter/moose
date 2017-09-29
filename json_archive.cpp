// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include "json_archive.h"
#include "rapidjson/istreamwrapper.h"

using namespace std;

namespace moose{
JSONArchive::JSONArchive () : IArchive (true)
{}

JSONArchive::doc_t& JSONArchive::new_document ()
{
	m_doc = std::unique_ptr<doc_t>(new doc_t);
	return *m_doc;
}

void JSONArchive::parse_file (const char* filename)
{
	ifstream in (filename);
	if(!in)
		throw runtime_error (string("File not found: ").append(filename));

	doc_t& d = new_document ();
	rapidjson::IStreamWrapper inWrapper(in);
	if(d.ParseStream (inWrapper).HasParseError ())
		throw runtime_error (string("JSON Parse error in file: ").append(filename));

	m_iterators.push (make_pair(d.MemberBegin(), d.MemberEnd()));
}

void JSONArchive::begin_read (const char* name)
{
	if(m_iterators.empty())
		throw runtime_error (string("End of file reached. Couldn't read field '")
		                     .append(name).append("'"));

	m_actMember = *m_iterators.top().first;
	mem_t& m = m_actMember;
	++m_iterators.top().first;
	
	iter_pair_t iters;
	if(m.value.IsObject()){
		m_iterators.push(iter_pair_t(m.value.MemberBegin(), m.value.MemberEnd()));
	}

	while(!m_iterators.empty() && m_iterators.top().first == m_iterators.top().second)
		m_iterators.pop();
}


void JSONArchive::end_read (const char* name)
{
}

std::string JSONArchive::get_type_name ()
{
	return m_actMember.name.GetString();
}

void JSONArchive::read (const char* name, double& val)
{
	mem_t& m = m_actMember;
	cout << "<dbg> reading field '" << m.name.GetString() << "' as double" << endl;
	val = m.value.GetDouble();
}


void JSONArchive::read (const char* name, std::string& val)
{
	mem_t& m = m_actMember;
	cout << "<dbg> reading field '" << m.name.GetString() << "' as string" << endl;
	val = m.value.GetString();
}

}//	end of namespace moose
