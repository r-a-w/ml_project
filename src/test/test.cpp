#include <fstream>
#include <iostream>
#include "data_containers.h"
#include "csv_read.h"
#include "decision_trees.h"
#include <string>

using namespace std;
using namespace mlglobal;

int main(){
  //DataContainers::LabeledData<int> test_container;
  //std::string file_name = "/Users/SMI2LE/Documents/machine_learning_project/datasets/challenger.csv";
  DataContainers::LabeledData<std::string> test_container;
  std::string file_name = "/Users/SMI2LE/Documents/machine_learning_project/datasets/test_set.csv";

  readCSVFile(file_name, test_container, ',', '#');

  //test_container.printDataByLabel("Decision");


  vector<std::string> test_array;
  test_array.push_back("Decision");

  Trainers::DecisionTree<std::string> test_tree;
  test_tree = TrainingFunctions::DecisionTree_Alg_ID3(test_container, "Decision");

  //test_tree.printTree();



}
