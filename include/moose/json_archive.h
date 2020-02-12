// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_json_archive
#define __H__moose_json_archive

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

	virtual ~JSONArchive ()	{}

  JSONArchive& operator = (JSONArchive const&) = delete;
  JSONArchive& operator = (JSONArchive&& other);

	void parse_file (const char* filename);
  void parse_string (const char* str);

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
  struct ParseData;
  std::shared_ptr <ParseData> m_parseData;
};

}//	end of namespace moose

#endif	//__H__moose_json_archive
