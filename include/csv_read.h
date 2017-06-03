//==================================================================
/*
 * Defines csv input reading functions for labeled and unlabeled data
 *
 */
//=================================================================


#ifndef CSV_READ_H
#define CSV_READ_H

#include "data_containers.h"

namespace mlglobal{

/* reads labeled csv file into data container */
template <class T> 
  void readCSVFile(std::string file, DataContainers::LabeledData<T> & data_container, char delimiter=',', char comment='#', bool labellocation=true);


/* reads unlabeled csv file into data container */
template <class T> 
  void readCSVFile(std::string file, DataContainers::UnlabeledData<T> & data_container, char delimiter=',', char comment='#');


}// mlglobal namespace



#endif
