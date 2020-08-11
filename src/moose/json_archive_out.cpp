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

#include <cassert>

#include <moose/json_archive_out.h>

using std::endl;

namespace
{
  void WriteWhitespace (std::ostream& out, size_t n)
  {
    for (size_t i = 0; i < n; ++i)
      out << ' ';
  }
}

namespace moose
{
  JSONArchiveOut::JSONArchiveOut (const char* filename)
    : Archive (Mode::Write)
    , m_out (filename)
  {
    if (!m_out)
      throw ArchiveError () << "Could not open '" << filename << "' for writing";

    m_out << "{" << endl;
    ++m_currentDepth;
  }

  JSONArchiveOut::JSONArchiveOut (JSONArchiveOut&& other)
    : Archive (Mode::Write)
    , m_out {std::move (other.m_out)}
    , m_currentDepth {other.m_currentDepth}
    , m_lastWrittenDepth {other.m_lastWrittenDepth}
  {}

  JSONArchiveOut::~JSONArchiveOut ()
  {
    if (!m_out)
      return;

    m_out << "}" << endl;
    --m_currentDepth;
    assert (m_currentDepth == 0);
  }

  JSONArchiveOut& JSONArchiveOut::operator = (JSONArchiveOut&& other)
  {
    m_out = std::move (other.m_out);
    m_currentDepth = other.m_currentDepth;
    m_lastWrittenDepth = other.m_lastWrittenDepth;
    return *this;
  }

  void JSONArchiveOut::begin_entry (const char* name, EntryType entryType)
  {
    WriteWhitespace (m_out, m_currentDepth * 2);

    m_out << "\"" << name << "\": ";

    switch (entryType)
    {
      case EntryType::StructOrValue:
        m_out << "{" << std::endl;
        break;

      case EntryType::Array:
        m_out << "[" << std::endl;
        break;
    }

    m_lastWrittenDepth = m_currentDepth;
    ++m_currentDepth;
  }

  void JSONArchiveOut::end_entry (const char* name, EntryType entryType)
  {
    m_out << std::endl;
    --m_currentDepth;
    WriteWhitespace (m_out, m_currentDepth * 2);

    switch (entryType)
    {
      case EntryType::StructOrValue:
        m_out << "}" << std::endl;
        break;

      case EntryType::Array:
        m_out << "]" << std::endl;
        break;
    }
  }

  void JSONArchiveOut::write_type_name (std::string const& typeName)
  {
    prepare_content ();
    m_out << "\"@type\": \"" << typeName << "\"";
  }

  void JSONArchiveOut::archive (const char* name, double& val)
  {
    prepare_content ();
    m_out << "\"" << name << "\": " << val;
  }

  void JSONArchiveOut::archive (const char* name, std::string& val)
  {
    m_out << "\"" << name << "\": \"" << val << "\"";
  }

  void JSONArchiveOut::prepare_content ()
  {
    if (m_lastWrittenDepth == m_currentDepth)
      m_out << "," << endl;
    WriteWhitespace (m_out, m_currentDepth * 2);
  }
}