// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_archive
#define __H__moose_archive

#include <map>
#include <memory>
#include <string>
#include "object_factory.h"

namespace moose{

class IArchive {
public:
	IArchive (bool reading) :
		m_reading (reading)
	{}

	virtual ~IArchive ()	{}

	template <class T>
	void operator () (T& value)
	{
		if(m_reading){
			begin_read ("__noname");
			read ("__noname", value);
			end_read ("__noname");
		}
		else{
			// write (name, value);
		}
	}

	template <class T>
	void operator () (const char* name, T& value)
	{
		if(m_reading){
			begin_read (name);
			read (name, value);
			end_read (name);
		}
		else{
			// write (name, value);
		}
	}

protected:
	virtual void begin_read (const char* name) {};
	virtual void end_read (const char* name) {};

	template <class T>
	void read (const char* name, T& value)
	{
		Serialize (*this, value);
	}

	template <class T>
	void read (const char* name, std::shared_ptr<T>& sp)
	{
		if(!sp){
			sp = std::shared_ptr<T>(ObjectFactory::create<T>(get_type_name()));
		}
		Serialize (*this, *sp);
	}

		template <class T>
	void read (const char* name, std::unique_ptr<T>& up)
	{
		if(!up){
			up = std::unique_ptr<T>(ObjectFactory::create<T>(get_type_name()));
		}
		Serialize (*this, *up);
	}

	template <class T>
	void read (const char* name, T*& p)
	{
		if(!p)
			p = ObjectFactory::create<T>(get_type_name());
		Serialize (*this, *p);
	}

	virtual std::string get_type_name () = 0;

///	reads a number value (int, float, ...).
/** Default implementation redirects to 'read(const char*, double&)'
 * \{ */
	virtual void read (const char* name, char& val);
	virtual void read (const char* name, unsigned char& val);
	virtual void read (const char* name, int& val);
	virtual void read (const char* name, unsigned int& val);
	virtual void read (const char* name, std::size_t& val);
	virtual void read (const char* name, float& val);
	virtual void read (const char* name, double& val) = 0;
/** \} */

	virtual void read (const char* name, std::string& val) = 0;


private:
	template <class T>
	void read_double(const char* name, T& val);

	bool m_reading;
};


template <class T>
void Serialize(IArchive& ar, T& val)
{
	val.serialize (ar);
}

}//	end of namespace moose

#endif	//__H__moose_archive
