// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdexcept>
#include <fstream>
#include <moose/exceptions.h>
#include <moose/json_archive_in.h>
#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>

namespace
{

struct JSONEntry
{
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

  bool is_object () {return m_type == Object;}
  bool is_array ()  {return m_type == Array;}
  bool is_value ()  {return m_type == Value;}

private:
  enum Type {Object, Array, Value};

  val_t* m_val;
  rapidjson::Value::MemberIterator m_icurMem;
  rapidjson::Value::ValueIterator m_icurVal;
  const char* m_name;
  Type m_type;
};

}// end of namespace

namespace moose
{

struct JSONArchiveIn::ParseData
{
  typedef rapidjson::Document doc_t;

  doc_t& new_document ();

  std::stack <JSONEntry> m_entries;
  std::unique_ptr <doc_t> m_doc;
};

auto JSONArchiveIn::ParseData::new_document () -> doc_t&
{
  m_doc = std::unique_ptr<doc_t>(new doc_t);
  return *m_doc;
}

JSONArchiveIn JSONArchiveIn::fromFile (const char* filename)
{
  JSONArchiveIn ar;
  ar.parse_file (filename);
  return ar;
}

JSONArchiveIn JSONArchiveIn::fromString (const char* str)
{
  JSONArchiveIn ar;
  ar.parse_string (str);
  return ar;
}

JSONArchiveIn::JSONArchiveIn ()
  : Archive (Mode::Read)
  , m_parseData (std::make_shared <ParseData> ())
{}

JSONArchiveIn::JSONArchiveIn (JSONArchiveIn&& other)
  : Archive (Mode::Read)
  , m_parseData (std::move (other.m_parseData))
{}

JSONArchiveIn& JSONArchiveIn::operator = (JSONArchiveIn&& other)
{
  m_parseData = std::move (other.m_parseData);
  return *this;
}

void JSONArchiveIn::parse_file (const char* filename)
{
  std::ifstream in (filename);
  if (!in) throw ArchiveError () << "File not found: " << filename;
  parse_stream (in);
}

void JSONArchiveIn::parse_stream (std::istream& in)
{
  if (!in) throw ArchiveError () << "Invalid stream specified.";

  auto& d = m_parseData->new_document ();
  rapidjson::IStreamWrapper inWrapper (in);
  rapidjson::ParseResult res = d.ParseStream (inWrapper);
  
  if (!res)
  {
    //  we have to find the line where the error occurred using the
    //  read offset of 'res'.
    in.clear ();
    in.seekg (0);
    std::string buf;
    int counter = 0;
    int curLine = 0;
    while (!in.eof () && (counter <= static_cast <int> (res.Offset())))
    {
      std::getline(in, buf, '\n');
      counter += (int)buf.size() + 1; //count line-ending
      ++curLine;
    }

    //  remove cr from windows line endings
    if(!buf.empty() &&
       buf[buf.size() - 1] == '\r')
    {
      buf.resize(buf.size() - 1);
    }

    throw ArchiveError () << "JSON Parse error in line "
      << curLine << ": " << rapidjson::GetParseError_En(res.Code())
      << " ('" << buf << "')";
  }

  m_parseData->m_entries.push (JSONEntry (&d, "_root_"));
}

void JSONArchiveIn::parse_string (const char* str)
{
  auto& d = m_parseData->new_document ();
  rapidjson::ParseResult res = d.Parse (str);
  if(!res){
    throw ArchiveError () << "JSON Parse error in string '" << str << "': "
        << rapidjson::GetParseError_En(res.Code());
  }

  m_parseData->m_entries.push (JSONEntry (&d, "_root_"));
}

bool JSONArchiveIn::begin_entry (const char* name, EntryType entryType, Hint)
{
  auto& entries = m_parseData->m_entries;
  if (entries.empty())
    throw ArchiveError () << "End of file reached. Couldn't archive field '" << name << "'";

  auto& e = entries.top();

//  if we're currently iterating over elements with the given name, we don't
//  have to initialize the iterators
  if(!e.iter_valid() || (strcmp(name, e.iter_name()) != 0))
    e.init_iter(name);

  if (!e.iter_valid())
    return false;

  // cout << "<dbg> pushing entry '" << e.iter_name() << "'\n";
  entries.push (JSONEntry (&e.iter_value(), e.iter_name()));
  if(entries.top().is_array())
    entries.top().init_iter("");

  return true;
}

void JSONArchiveIn::end_entry (const char* name, EntryType entryType)
{
  auto& entries = m_parseData->m_entries;
  if (entries.empty())
    throw ArchiveError () << "JSONArchiveIn::end_read called on empty stack for entry '" << name << "'";

  entries.pop();

  if(!entries.empty()){
    entries.top().advance();
  }
}

bool JSONArchiveIn::read_array_has_next (const char* name)
{
  return m_parseData->m_entries.top().iter_valid();
}

std::string JSONArchiveIn::read_type_name ()
{
  auto& entries = m_parseData->m_entries;
  if (entries.empty()) throw ArchiveError () << "JSONArchiveIn::read_type_name: entry stack empty!";

  auto& value = entries.top().value();
  
  if(value.HasMember("@type"))
    return value["@type"].GetString();
  else
    return {};
}

void JSONArchiveIn::archive (const char* name, double& val)
{
  auto& entries = m_parseData->m_entries;
  if (entries.empty()) throw ArchiveError () << "JSONArchiveIn::archive: entry stack empty!";

  auto& e = entries.top();
  val = e.value().GetDouble();
}

void JSONArchiveIn::archive (const char* name, std::string& val)
{
  auto& entries = m_parseData->m_entries;
  if (entries.empty()) throw ArchiveError () << "JSONArchiveIn::archive: entry stack empty!";

  auto& e = entries.top();
  val = e.value().GetString();
}

}// end of namespace moose

namespace
{

JSONEntry::JSONEntry () :
  m_val (nullptr),
  m_name (""),
  m_type (Value)
{}

JSONEntry::JSONEntry (val_t* _val, const char* _name) :
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

void JSONEntry::init_iter (const char* name)
{
  switch(m_type) {
    case Object: m_icurMem = m_val->FindMember (name); break;
    case Array: m_icurVal = m_val->Begin(); break;
    case Value: break;
  }
}

bool JSONEntry::iter_valid () const
{
  switch(m_type) {
    case Object: return m_val && (m_icurMem != m_val->MemberEnd());
    case Array: return m_val && (m_icurVal != m_val->End());
    case Value: return false;
    default: throw moose::ArchiveError () << "Invalid code path";
  }
}

bool JSONEntry::value_valid () const
{
  return m_val != nullptr;
}

JSONEntry::val_t& JSONEntry::value ()
{
  return *m_val;
}

const char* JSONEntry::name ()
{
  return m_name;
}

JSONEntry::val_t& JSONEntry::iter_value ()
{
  switch(m_type) {
    case Object: return m_icurMem->value;
    case Array: return *m_icurVal;
    case Value: throw moose::ArchiveError () << "Values don't have iterators which could be accessed.";
    default: throw moose::ArchiveError () << "Invalid code path";
  }
}

const char* JSONEntry::iter_name ()
{
  switch(m_type) {
    case Object: return m_icurMem->name.GetString();
    case Array: return "";
    case Value: throw moose::ArchiveError () << "Values don't have iterators which could be accessed.";
    default: throw moose::ArchiveError () << "Invalid code path";
  }
}

void JSONEntry::advance ()
{
  switch(m_type) {
    case Object: ++m_icurMem; break;
    case Array: ++m_icurVal; break;
    case Value: break;
  }
}

}// end of namespace
