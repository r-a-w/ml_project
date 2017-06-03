//=======================================================================
/*
 * Defines global exception class and some exception macros 
 *
 */
//=======================================================================

#ifndef ML_EXECEPTION_H
#define ML_EXECEPTION_H

#include <exception>
#include <string>

namespace mlglobal{

class ML_Exception: public std::exception{
  public:
    /* const'r */
    ML_Exception(const std::string & what = "an exception occured") : ml_what(what){}
    /* dest'r */
    ~ML_Exception() throw() {};

    virtual const char* what() const throw(){
      return ml_what.c_str();
    }

  protected:
    std::string ml_what;

  };

}

#define ML_EXCEPTION(error_string, condition) if (!condition) throw mlglobal::ML_Exception(error_string)
#define DPRINT(string) std::cout << "[DEBUG] " << string << std::endl



#endif
