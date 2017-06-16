#include <iostream>
#include <vector>
#include <math.h>
#include "decision_trees.h"
#include "ml_exceptions.h"


using namespace std;

/* Decision tree class constructor */
template <typename T> mlglobal::Trainers::DecisionTree<T>::DecisionTree(){
  root_node = new node<T>;
  root_node->parent_node=NULL;
  root_node->node_index=0;
  n_nodes=1;
}

/* recursive function to find tree node based off index */
template <typename T> node<T>* mlglobal::Trainers::DecisionTree<T>::findNode(unsigned int node_index, node<T> *current_node){

  using namespace mlglobal::Trainers;
  /* current node not given, set to root of tree */
  if(current_node==NULL) current_node=root_node;

  while(current_node->node_index != node_index){
    if(!current_node->branch_nodes.size()){
      current_node = DecisionTree::findNode(node_index, current_node);
    } else{
      ML_EXCEPTION("cannot find node", 0);
    }

  }
  return current_node;

}


template <typename T> node<T>* mlglobal::Trainers::DecisionTree<T>::addBranch(node<T> *node_f, unsigned int column_index, T value){

  node<T> *new_node = new node<T>;
  node_f->branch_nodes.push_back(new_node);
  new_node->column_index = column_index;
  new_node->node_value = value;
  new_node->parent_node = node_f;
  new_node->node_index = n_nodes+1;
  ++n_nodes;

  return new_node;

}

template <typename T> node<T>* mlglobal::Trainers::DecisionTree<T>::makeLeaf(node<T> *node_f, unsigned int column_index, T value){

  node_f->node_value = value;
  node_f->column_index = column_index;

  /* delete all foward branches */
  if(node_f->branch_nodes.size()){
    for(typename std::vector<node<T>*>::iterator it=node_f->branch_nodes.begin(); it!=node_f->branch_nodes.end(); ++it) delete *it;
    --n_nodes;
  }


  return node_f;




}


/* calculates unique data points */
template <typename T> vector<T> uniqueValues(vector<T> data){

  vector<T> unique_data_points;
  typename vector<T>::iterator it, it_u;
  bool unique;
  for(it=data.begin();it!=data.end();++it){
    unique=true;
    for(it_u=unique_data_points.begin();it_u!=unique_data_points.end();++it_u){
      if(*it==*it_u) unique=false;
    }
    if(unique) unique_data_points.push_back(*it);
  }
  return unique_data_points;

}


/* calculates entropy */
template <typename T> double calcEntropy(vector<T> data, vector<T> unique_data){

  size_t data_size=data.size();
  unsigned int count=0;
  typename  vector<T>::iterator it_ud, it_d;
  double entropy=0;
  for(it_ud=unique_data.begin();it_ud!=unique_data.end();++it_ud){
    for(it_d=data.begin();it_d!=data.end();++it_d){
      if(*it_d==*it_ud) ++count;
    }
    if(count){
      entropy-=(((double) count/ (double) data_size)*log2f((double) count/ (double) data_size));
    }
    count=0;
  }

  return entropy;

}


/* calculates weighted entropy of input data */
template <typename T> double weightedEntropy(vector<T> data, vector<T> target, vector<T> unique_target){

  vector<T> unique_data = uniqueValues(data);

  typename vector<T>::iterator it_ut, it_ud, it_d, it_s;
  size_t n_examples = data.size();
  double entropy=0,weighted_entropy=0;
  unsigned int count=0, subset_size;
  vector<T> subset, unique_subset;
  for(it_ud=unique_data.begin();it_ud!=unique_data.end();++it_ud){
    for(it_d=data.begin();it_d!=data.end();++it_d){
      if(*it_d==*it_ud){
        ++count;
        subset.push_back(target.at(it_d-data.begin()));
      }
    }
    entropy = calcEntropy(subset, unique_target);
    weighted_entropy+=((double) count/(double) n_examples)*entropy;
    entropy = 0;
    count = 0;
    subset.clear();
  }

  return weighted_entropy;

}



/* outputs most common value in input vector */
template <typename T> T mostCommonValue(vector<T> data){

  vector<T> unique_data_points = uniqueValues(data);

  /* find most common value */
  int count[unique_data_points.size()];
  fill_n(count,unique_data_points.size(),0);
  typename vector <T>::iterator it, it_u;
  for(it=data.begin();it!=data.end();++it){
    for(it_u=unique_data_points.begin();it_u!=unique_data_points.end();++it_u){
      if(*it==*it_u){
        ++count[it_u-unique_data_points.begin()];
        break;
      }
    }
  }

  T mcv = unique_data_points.at(max_element(count,count+unique_data_points.size())-count);
  return mcv;
}




/* Wrapper for ID3 Algorithm for labeled data container */
template <typename T>  mlglobal::Trainers::DecisionTree<T> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute, std::vector<std::string> attributes){

  /* Create decision tree */
  mlglobal::Trainers::DecisionTree<T> *decision_tree = new mlglobal::Trainers::DecisionTree<T>;

  /* get all data attributes in container */
  ML_EXCEPTION("No data in container, cannot run ID3 algorithm", (data_container.numberOfColumns()-1));
  if(!attributes.size()) attributes = data_container.getLabels();

  /* gather column indexes for training attributes */
  vector<unsigned int> iattributes;
  for(vector<std::string>::iterator it=attributes.begin();it!=attributes.end();++it){
    if(!target_attribute.compare(*it)) continue;
    iattributes.push_back(data_container.getIndexByLabel(*it));
  }

  /* get column index for target attribute */
  size_t target = data_container.getIndexByLabel(target_attribute);

  /* get root node and train decision tree */
  node<T> *root_node = decision_tree->findNode(0);
  _DecisionTree_Alg_ID3(*decision_tree, root_node, data_container, target, iattributes);

  return *decision_tree;



}

unsigned int test_index=0;

/* implementation of the ID3 algorithm */
template <typename T>  void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<T>& decision_tree, node<T> *current_node, mlglobal::DataContainers::LabeledData<T>& data_container, unsigned int target, vector<unsigned int> iattributes){

  ++test_index;


  vector<T> target_data = data_container.getDataByIndex(target);
  vector<T> unique_target = uniqueValues(target_data);

  if(unique_target.size() == 1){
    decision_tree.addBranch(current_node, target, unique_target.at(0));
    return;
  }

  if(!iattributes.size()){
    decision_tree.addBranch(current_node, target, mostCommonValue(target_data));
    return;
  }


  /* determine the attribute with maximum information gain */
  vector<T> attribute_data, max_attr_data;
  double target_entropy = calcEntropy(target_data, unique_target);
  double info_gain, max_info_gain=0;
  unsigned int max_index;
  vector<unsigned int>::iterator it_u, max_iter;
  for(it_u=iattributes.begin();it_u != iattributes.end(); ++it_u){
    attribute_data = data_container.getDataByIndex(*it_u);
    info_gain = target_entropy - weightedEntropy(attribute_data, target_data, unique_target);
    if(info_gain > max_info_gain){
      max_info_gain = info_gain;
      max_attr_data = attribute_data;
      max_index = *it_u;
      max_iter = it_u;
    }
  }
  iattributes.erase(max_iter); // remove attribute



  using namespace mlglobal;
  /* branch tree based off of attribute */
  vector<T> unique_attribute = uniqueValues(max_attr_data);
  typename vector<T>::iterator it;
  DataContainers::LabeledData<T> data_subset;
  node<T> *next_node;
  for(it=unique_attribute.begin(); it!=unique_attribute.end(); ++it){
    next_node = decision_tree.addBranch(current_node, max_index, *it);
    data_subset = data_container.getSubset(*it, max_index);
    _DecisionTree_Alg_ID3(decision_tree, next_node, data_subset, target, iattributes);
  }



  return;


}




template <typename T> void mlglobal::Trainers::DecisionTree<T>::printTree(node<T> *current_node){

  using namespace mlglobal::Trainers;
  if(current_node == NULL) current_node = root_node;
  else std::cout << current_node->node_value << std::endl;


  if(!current_node->branch_nodes.size()) std::cout << "--LEAF--" << std::endl;

  typename std::vector<node<T>*>::iterator it;
  for(it = current_node->branch_nodes.begin(); it!=current_node->branch_nodes.end(); ++it){
    DecisionTree<T>::printTree(*it);
  }

}



/*
 * Explicit instansiations
 */
//============================================
/* For decision tree classes*/
//============================================

/* for labeled int data_container */
template mlglobal::Trainers::DecisionTree<int>::DecisionTree();
template node<int>* mlglobal::Trainers::DecisionTree<int>::addBranch(node<int> *node_f, unsigned int column_index, int value);
template node<int>* mlglobal::Trainers::DecisionTree<int>::findNode(unsigned int node_index, node<int> *current_node);
template node<int>* mlglobal::Trainers::DecisionTree<int>::makeLeaf(node<int> *node_f, unsigned int column_index, int value);
template void mlglobal::Trainers::DecisionTree<int>::printTree(node<int> *current_node);

/* for labeled float data_container */
template mlglobal::Trainers::DecisionTree<float>::DecisionTree();
template node<float>* mlglobal::Trainers::DecisionTree<float>::addBranch(node<float> *node_f, unsigned int column_index, float value);
template node<float>* mlglobal::Trainers::DecisionTree<float>::findNode(unsigned int node_index, node<float> *current_node);
template node<float>* mlglobal::Trainers::DecisionTree<float>::makeLeaf(node<float> *node_f, unsigned int column_index, float value);
template void mlglobal::Trainers::DecisionTree<float>::printTree(node<float> *current_node);


/* for labeled std::string data_container */
template mlglobal::Trainers::DecisionTree<std::string>::DecisionTree();
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::addBranch(node<std::string> *node_f, unsigned int column_index, std::string value);
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::findNode(unsigned int node_index, node<std::string> *current_node);
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::makeLeaf(node<std::string> *node_f, unsigned int column_index, std::string value);
template void mlglobal::Trainers::DecisionTree<std::string>::printTree(node<std::string> *current_node);

//============================================
/* For decision tree algorithms */
//============================================

/* for labeled int data_container */
template mlglobal::Trainers::DecisionTree<int> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<int>(DataContainers::LabeledData<int> data_container, std::string target_attribute, std::vector<std::string> attributes );
template void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<int>& decision_tree, node<int> *current_node, mlglobal::DataContainers::LabeledData<int>& data_container, unsigned int target, vector<unsigned int> iattributes);
template int mostCommonValue<int>(vector<int> data);
template vector<int> uniqueValues(vector<int> data);
template double weightedEntropy(vector<int> data, vector<int> target, vector<int> unique_target);
template double calcEntropy(vector<int> data, vector<int> unique_data);

/* for labeled std::string data_container */
template mlglobal::Trainers::DecisionTree<std::string> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<std::string>(DataContainers::LabeledData<std::string> data_container, std::string target_attribute, std::vector<std::string> attributes );
template void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<std::string>& decision_tree, node<std::string> *current_node, mlglobal::DataContainers::LabeledData<std::string>& data_container, unsigned int target, vector<unsigned int> iattributes);
template std::string mostCommonValue<std::string>(vector<std::string> data);
template vector<std::string> uniqueValues(vector<std::string> data);
template double weightedEntropy(vector<std::string> data, vector<std::string> target, vector<std::string> unique_target);
template double calcEntropy(vector<std::string> data, vector<std::string> unique_data);


