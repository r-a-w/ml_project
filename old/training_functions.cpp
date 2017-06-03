#include <iostream>
#include "ml_trainers.h"
#include "training_functions.h"



template <typename T>  mlglobal::Trainers::DecisionTree mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute, std::vector<std::string> attributes){
  std::cout << "in teh function" << std::endl;
  return mlglobal::Trainers::DecisionTree();
}




/*
 * Explicit instansiations
*/
template mlglobal::Trainers::DecisionTree mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<int>(DataContainers::LabeledData<int> data_container, std::string target_attribute, std::vector<std::string> attributes );
