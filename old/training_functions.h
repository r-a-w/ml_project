//===================================================================================
/* Defines the training functions 
 * 
 *
*/
//===================================================================================


#ifndef TRAINING_FUNCTIONS_H  
#define TRAINING_FUNCTIONS_H 

#include "data_containers.h"
#include "ml_trainers.h"
#include <vector>


namespace mlglobal{
namespace TrainingFunctions{
  template <typename T>  mlglobal::Trainers::DecisionTree DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute, std::vector<std::string> attributes = std::vector<std::string>());
} /* TrainingFunctions namespace */
} /* mlglobal namespace */


#endif 
