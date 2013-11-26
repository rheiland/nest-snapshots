/*
 *  functiondatum.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FUNCTIONDATUM_H
#define FUNCTIONDATUM_H
/* 
    Datum class for SLI builtin functions.
*/

#include "datum.h"
#include "interpret.h"
#include "slifunction.h"
#include "allocator.h"
/*
  Each SLI command is represented by a derived class of class SLIFunction.
  For each command, the constructor of the interpreter must then create 
  a SLIDatum object from a given name (e.g. "add") and a const & to
  an instance of the derived SLIFunction (e.g. AddFunction). This new
  datum object must then be registered in the dictionary (hopefully
  by the same name as before).
 */

class FunctionDatum: public TypedDatum<&SLIInterpreter::Functiontype>
{
  static sli::pool memory;

  Name name;
  
  Datum * clone(void) const
    {
      return new FunctionDatum(*this);
    }
  
  SLIFunction const & operator=(SLIFunction const &f)
    {
      std::cerr << "Warning: Definition of FunctionDatum ("<<name<<") changed!!\n";
      
      action= &f;
      return f;
    }
  
public:
  FunctionDatum(FunctionDatum const &fd)
    :TypedDatum<&SLIInterpreter::Functiontype>(fd),name(fd.name)
    {}
  
  FunctionDatum(Name const &n, SLIFunction const *f)
    : TypedDatum<&SLIInterpreter::Functiontype>(), name(n)
    {
      //! Here, we shortcut the default action of the type object and directly
      //! place the function pointer in the datum's action field. Thus, we
      //! avoid an extra virtual function call.
      assert(f != NULL);
      action=f;
    }
  
  
  void print(std::ostream& o) const
    {
      o << '-' << name << '-';
    }
  
  void pprint(std::ostream& o) const
    {
      print(o);
    }
  
  void info(std::ostream &out) const
    {
      out << "FunctionDatum::info\n";
      out << "name = " << name << std::endl;
    }
  
  bool equals(Datum const *dat) const
    {
      const FunctionDatum
	*fd=dynamic_cast<FunctionDatum * >(const_cast< Datum *>(dat));
      
        if(fd == NULL)
	  return false;
	
        return action == fd->action;
    }
  
  const Name & getname(void) const
    {
        return name;
    }

  void backtrace(SLIInterpreter *, int) const;

  static void * operator new(size_t size)
    {
      if(size != sizeof(FunctionDatum))
	return ::operator new(size);
      return memory.alloc();
    }

  static void operator delete(void *p, size_t size)
    {
      if(p == NULL)
	return;
      if(size != sizeof(FunctionDatum))
      {
	::operator delete(p);
	return;
      }
      memory.free(p);
    }
  
};


#endif


