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

#include <moose/exceptions.h>
#include <moose/json_writer.h>

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
  auto JSONWriter::toFile (const char* filename) -> std::shared_ptr<JSONWriter>
  {
    return std::make_shared <JSONWriter> (filename);
  }

  JSONWriter::JSONWriter (const char* filename)
    : JSONWriter (std::make_shared <std::ofstream> (filename))
  {
  }

  JSONWriter::JSONWriter (std::shared_ptr <std::ostream> out)
    : m_out (std::move (out))
  {
    if (!m_out ||
        m_out->fail ())
    {
      throw ArchiveError () << "Invalid file or stream specified for writing.";
    }

    this->out () << "{";
    ++m_currentDepth;
    optional_endl ();
  }

  JSONWriter::JSONWriter (JSONWriter&& other)
    : m_out {std::move (other.m_out)}
    , m_currentDepth {other.m_currentDepth}
    , m_lastWrittenDepth {other.m_lastWrittenDepth}
  {}

  JSONWriter::~JSONWriter ()
  {
    --m_currentDepth;
    optional_endl ();
    out () << "}" << std::endl;
    out ().flush ();
    assert (m_currentDepth == 0);
  }

  JSONWriter& JSONWriter::operator = (JSONWriter&& other)
  {
    m_out = std::move (other.m_out);
    m_currentDepth = other.m_currentDepth;
    m_lastWrittenDepth = other.m_lastWrittenDepth;
    return *this;
  }

  bool JSONWriter::begin_entry (const char* name, ContentType type, Hint hint)
  {
    prepare_content ();

    if (this->hint () == Hint::ChildrenOneLine)
      hint = Hint::OneLine;

    m_hints.push (hint == Hint::None ? this->hint () : hint);

    if (name != nullptr &&
        *name != 0)
    {
      out () << "\"" << name << "\": ";
    }

    ++m_currentDepth;
    switch (type)
    {
      case ContentType::Array:
        out () << "[";
        optional_endl ();
        break;

      case ContentType::Struct:
        out () << "{";
        optional_endl ();
        break;

      case ContentType::Value:
        break;
    }

    return true;
  }

  void JSONWriter::end_entry (const char*, ContentType type)
  {
    --m_currentDepth;
    m_lastWrittenDepth = m_currentDepth;

    switch (type)
    {
      case ContentType::Array:
        optional_endl ();
        out () << "]";
        break;

      case ContentType::Struct:
        optional_endl ();
        out () << "}";
        break;

      case ContentType::Value:
        break;
    }

    assert (!m_hints.empty ());

    if (!m_hints.empty ())
      m_hints.pop ();
  }

  void JSONWriter::write_type_name (std::string const& typeName)
  {
    prepare_content ();
    out () << "\"@type\": \"" << typeName << "\"";
    m_lastWrittenDepth = m_currentDepth;
  }

  void JSONWriter::write_type_version (Version const& version)
  {
    prepare_content ();
    out () << "\"@type_version\": \"" << version.toString () << "\"";
    m_lastWrittenDepth = m_currentDepth;
  }

  void JSONWriter::write (const char*, bool val)
  {
    out () << val;
  }

  void JSONWriter::write (const char*, double val)
  {
    out () << val;
  }

  void JSONWriter::write (const char*, std::string const& val)
  {
    auto& out = this->out ();
    out << "\"";
    for (auto c : val)
    {
      if (c == '\\')
        out << "\\\\";
      else
        out << c; 
    }
    out << "\"";
  }

  void JSONWriter::prepare_content ()
  {
    if (m_lastWrittenDepth == m_currentDepth)
    {
      out () << ",";
      optional_endl ();
    }
  }

  void JSONWriter::optional_endl ()
  {
    if (hint () == Hint::OneLine)
      return;
    out () << std::endl;
    WriteWhitespace (out (), m_currentDepth * 2);
  }

  auto JSONWriter::hint () const -> Hint
  {
    if (m_hints.empty ())
      return Hint::None;
    return m_hints.top ();
  }

  auto JSONWriter::out () -> std::ostream &
  {
    return *m_out;
  }
}