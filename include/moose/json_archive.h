// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#pragma once

#include <stack>
#include <memory>
#include <moose/archive.h>

namespace moose
{

class JSONArchive : public Archive {
public:
  static JSONArchive fromFile (const char* filename);
  static JSONArchive fromString (const char* str);

public:
  JSONArchive ();
  JSONArchive (JSONArchive const&) = delete;
  JSONArchive (JSONArchive&& other);

  virtual ~JSONArchive () = default;

  JSONArchive& operator = (JSONArchive const&) = delete;
  JSONArchive& operator = (JSONArchive&& other);

  void parse_file (const char* filename);
  void parse_string (const char* str);

protected:
  void begin_read (const char* name) override;
  void end_read (const char* name) override;

  void begin_array_read (const char* name) override;
  bool array_has_next (const char* name) override;
  void end_array_read (const char* name) override;

  std::string get_type_name () override;
  
  void read (const char* name, double& val) override;
  void read (const char* name, std::string& val) override;

private:
  struct ParseData;
  std::shared_ptr <ParseData> m_parseData;
};

}// end of namespace moose
