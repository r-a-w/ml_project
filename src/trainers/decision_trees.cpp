#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
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

  if(current_node->node_index == node_index) return current_node;

  if(current_node->branch_nodes.size()){
      typename std::vector<node<T>*>::iterator it;
      node<T> *found_node;
      for(it=current_node->branch_nodes.begin();it != current_node->branch_nodes.end();++it){
        found_node = DecisionTree<T>::findNode(node_index, *it);
        if(found_node->node_index == node_index) return found_node;
      }
  } else if(current_node->node_index == (n_nodes-1)) ML_EXCEPTION("cannot find node", 0);

  return current_node;

}


template <typename T> node<T>* mlglobal::Trainers::DecisionTree<T>::addBranch(node<T> *node_f, unsigned int column_index, T value){

  node<T> *new_node = new node<T>;
  node_f->branch_nodes.push_back(new_node);
  new_node->column_index = column_index;
  new_node->node_value = value;
  new_node->parent_node = node_f;
  new_node->node_index = n_nodes;
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




/* Wrapper for ID3 Algorithm for labeled data container which also allows for filterting attributes by column vector */
template <typename T>  mlglobal::Trainers::DecisionTree<T> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute, std::vector<unsigned int> attributes){

  /* Create decision tree */
  mlglobal::Trainers::DecisionTree<T> *decision_tree = new mlglobal::Trainers::DecisionTree<T>;

  /* get all data attributes in container */
  ML_EXCEPTION("No data in container, cannot run ID3 algorithm", (data_container.numberOfColumns()-1));

  /* get column index for target attribute */
  size_t target = data_container.getIndexByLabel(target_attribute);

  /* gather column indexes for training attributes */
  vector<unsigned int> iattributes;
  unsigned int n_columns = data_container.numberOfColumns();
  for(std::vector<unsigned int>::iterator it=attributes.begin();it!=attributes.end();++it){
    if(*it>n_columns) ML_EXCEPTION("attribute column value out of bounds",0);
    if(*it==target) continue;
    iattributes.push_back(*it);
  }


  /* get root node and train decision tree */
  node<T> *root_node = decision_tree->findNode(0);
  _DecisionTree_Alg_ID3(*decision_tree, root_node, data_container, target, iattributes);

  return *decision_tree;


}


/* Wrapper for ID3 Algorithm for labeled data container which also allows for filterting attributes by string attribute vector */
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

/* Wrapper for ID3 Algorithm for labeled data container */
template <typename T>  mlglobal::Trainers::DecisionTree<T> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute){

  /* Create decision tree */
  mlglobal::Trainers::DecisionTree<T> *decision_tree = new mlglobal::Trainers::DecisionTree<T>;

  /* get all data attributes in container */
  ML_EXCEPTION("No data in container, cannot run ID3 algorithm", (data_container.numberOfColumns()-1));

  /* get column index for target attribute */
  size_t target = data_container.getIndexByLabel(target_attribute);

  /* save labels in the tree */
  decision_tree->labels = data_container.getLabels();

  /* gather column indexes for training attributes */
  vector<unsigned int> iattributes;
  for(unsigned int i=0;i<data_container.numberOfColumns();++i){
    if(i==target) continue;
    iattributes.push_back(i);
  }

  /* get root node and train decision tree */
  node<T> *root_node = decision_tree->findNode(0);
  _DecisionTree_Alg_ID3(*decision_tree, root_node, data_container, target, iattributes);

  return *decision_tree;



}

/* implementation of the ID3 algorithm */
template <typename T>  void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<T>& decision_tree, node<T> *current_node, mlglobal::DataContainers::LabeledData<T>& data_container, unsigned int target, vector<unsigned int> iattributes){

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
    if(info_gain > max_info_gain or it_u==iattributes.begin()){
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


/* actual recursive prediction code for float and int input */
template <typename T> T _predict(std::vector<T> input, node<T> *current_node, std::vector<int> matched_index){

  typename std::vector<node<T>*>::iterator it;
  node<T> *branch_node;
  unsigned int column_index, pred_column_index;

  /* if at leaf node, return prediction */
  if(current_node->branch_nodes.size() == 1){
    branch_node=current_node->branch_nodes.at(0);
    return branch_node->node_value;
  }

  /* determine column indexes for current branching */
  branch_node = current_node->branch_nodes.at(0);
  column_index = branch_node->column_index;
  pred_column_index = matched_index.at(column_index);
  if(pred_column_index == -1) ML_EXCEPTION("data label not found in decision tree, check prediction csv file",0);

  /* loop through branch nodes to determine path through tree using input parameter */
  T node_value;
  for(it=current_node->branch_nodes.begin();it!=current_node->branch_nodes.end();++it){
    branch_node = *it;
    node_value = branch_node->node_value;
    if(node_value == input.at(pred_column_index)) return _predict(input, branch_node, matched_index);
  }

  ML_EXCEPTION("prediction failed, corrupt tree or unknown inputs",0);
}


/* wrapper for predicting outputs from given inputs */
template <typename T> std::vector<T> mlglobal::Trainers::DecisionTree<T>::predict(mlglobal::DataContainers::LabeledData<T> data_container){

  /* match column indexs of prediciton container and decision tree */
  std::vector<std::string> pred_container_labels = data_container.getLabels();
  std::vector<std::string>::iterator it_p, it_d;
  std::vector<int> matched_index;
  std::string curr_label;
  for(it_d = labels.begin();it_d != labels.end();++it_d){
    curr_label = *it_d;
    for(it_p = pred_container_labels.begin();it_p != pred_container_labels.end();++it_p){
      if(!curr_label.compare(*it_p)){
          matched_index.push_back(it_p-pred_container_labels.begin());
          break;
      }
    }
    if(it_p == pred_container_labels.end()) matched_index.push_back(-1);
  }


  /* run each input row through the tree to get the prediciton */
  std::vector<T> predictions;
  unsigned int i;
  for(i=0;i<data_container.numberOfRows();++i){
    predictions.push_back(_predict(data_container.getDataByRow(i), root_node, matched_index));
  }

  return predictions;

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


template <typename T> void mlglobal::Trainers::DecisionTree<T>::saveTree(std::string file_location){

  if(file_location.size() == 0){
    file_location = getenv("HOME");
    ML_EXCEPTION("Cannot save, no $HOME environment variable", &file_location);
    file_location.append("/ml_save/saved_tree");
  }

  /* seperate file_ocation into directory and filename */
  std::string directory(file_location), file_name;
  std::string::iterator it;
  it = directory.end();
  while(it!=directory.begin()){
    if(*it == '/') break;
    --it;
  }
  ++it;
  while(it!=directory.end()){
    file_name.push_back(*it);
    directory.erase(it);
  }


  /* create directory if it doesnt exist, check for existing file */
  DIR *dir;
  struct dirent *ent;
  if((dir = opendir(directory.c_str())) != NULL){
    while((ent = readdir(dir)) != NULL){
      if(!std::strcmp(file_name.c_str(), ent->d_name)){
        char choice;
        std::cout << "\"" << file_name << "\" already exists, overwrite? (y/n)";
        std::cin >> choice;
        if(choice == 'y') break;
        else std::exit(0);
      }
    }
  }else mkdir(directory.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);


  /* write tree data to file */
  ofstream o_file(file_location);
  unsigned int i,k;
  node<T> *current_node, *temp_node;
  typename std::vector<node<T>*>::iterator it_n;

  /* parent node first */
  current_node = this->findNode(0);
  o_file << "========BEGIN NODE========" << std::endl;
  o_file << "node_index=" << current_node->node_index << std::endl;
  o_file << "column_index=" << std::endl;
  o_file << "node_value=" << current_node->node_value << std::endl;
  o_file << "parent_node=" << std::endl;
  o_file << "branch_nodes=";
  for(k=0;k<current_node->branch_nodes.size();++k){ // have to do this dumb loop b.c wasnt compiling using iterators for some reason
    temp_node = current_node->branch_nodes.at(k);
    o_file << temp_node->node_index;
    if(k < current_node->branch_nodes.size()-1) o_file << ",";
  }
  o_file << std::endl;
  o_file << "========END NODE========" << std::endl << std::endl;
  for(i = 1;i < n_nodes;++i){
    o_file << "========BEGIN NODE========" << std::endl;
    current_node = this->findNode(i);
    o_file << "node_index=" << current_node->node_index << std::endl;
    o_file << "column_index=" << current_node->column_index << std::endl;
    o_file << "node_value=" << current_node->node_value << std::endl;
    o_file << "parent_node=" << current_node->parent_node->node_index << std::endl;
    o_file << "branch_nodes=";
    if(current_node->branch_nodes.size()){
      for(k=0;k<current_node->branch_nodes.size();++k){
        temp_node = current_node->branch_nodes.at(k);
        o_file << temp_node->node_index;
        if(k < current_node->branch_nodes.size()-1) o_file << ",";
      }
    }
    o_file << std::endl;
    o_file << "========END NODE========" << std::endl << std::endl;
  }

  /* save labels */
  std::vector<std::string>::iterator it_l;
  o_file << "========BEGIN LABELS========" << std::endl;
  for(it_l=labels.begin();it_l != labels.end();++it_l) o_file << *it_l << std::endl;
  o_file << "========END LABELS========" << std::endl << std::endl;




  std::cout << "[+] saving \"" << file_name << "\" to " << directory << std::endl;

  o_file.close();

}


template <typename T>
struct loaded_node{
  unsigned int parent_node;
  unsigned int node_index;
  unsigned int column_index;
  T node_value;
  std::vector<unsigned int> branch_nodes;
};

template <typename T> void createTreeFromLoadedNodes(mlglobal::Trainers::DecisionTree<T> &decision_tree, std::vector<loaded_node<T>*> loaded_nodes, size_t n_nodes, node<T> *curr_node){


  if(n_nodes == 0) return;

  loaded_node<T> *curr_loaded_node = loaded_nodes.at((size_t) curr_node->node_index);
  loaded_node<T> *load_branch;
  node<T> *branch_node;

  std::vector<unsigned int>::iterator it;
  for(it = curr_loaded_node->branch_nodes.begin();it != curr_loaded_node->branch_nodes.end();++it){
    load_branch = loaded_nodes.at((size_t) *it);
    branch_node = decision_tree.addBranch(curr_node, load_branch->column_index, load_branch->node_value);
    createTreeFromLoadedNodes(decision_tree, loaded_nodes, n_nodes--, branch_node);
  }

  return;

}

/* unique functions for each possible datatype when converting node value to correct type 
 * dummy variale is a little hack to get it to use the correct function, fuck templates!
 * 
*/
std::string nodeValueDetermination(std::string node_value_string, std::string dummy){
  return node_value_string;
}

int nodeValueDetermination(std::string node_value_string, int dummy){
  return std::atoi(node_value_string.c_str());
}

float nodeValueDetermination(std::string node_value_string, float dummy){
  return std::atof(node_value_string.c_str());
}

template <typename T> void  mlglobal::Trainers::loadTree(std::string file_location, mlglobal::Trainers::DecisionTree<T> *decision_tree){

  ifstream stream(file_location);
  ML_EXCEPTION("Cannot open decision tree file", stream);

  ML_EXCEPTION("You must allocate memory for input tree (new Trainers::DecisionTree<>)", decision_tree);

  /* read all file contents */
  stream.seekg(0, std::ios::end);
  size_t size = stream.tellg();
  std::string file_contents(size, ' ');
  stream.seekg(0, std::ios::beg);
  stream.read(&file_contents[0], size);

  /*close file*/
  stream.close();


  /* build initial structure of decision tree*/
  std::string::iterator it;
  std::vector<loaded_node<T>*> loaded_nodes;
  size_t begin_position = 0, end_position = 0, pos = 0, pos_e = 0, n_nodes=0;
  int i=0;
  T dummy;
  while(true){
    begin_position = file_contents.find("BEGIN NODE", begin_position);
    end_position = file_contents.find("END NODE",begin_position);
    if(begin_position == -1 ) break;
    loaded_node<T> *curr_node = new loaded_node<T>;

    /* node index*/
    pos = file_contents.find("node_index=", begin_position);
    if(pos == -1 or pos > end_position) ML_EXCEPTION("cannot load tree, corrcupted saved tree", 0);
    pos_e = file_contents.find('\n', pos);
    if(pos_e != pos){
      curr_node->node_index = std::strtoul(file_contents.substr(pos+11,(pos_e-pos-11)).c_str(), NULL, 10);
    }

    /* column_index*/
    pos = file_contents.find("column_index=", begin_position);
    if(pos == -1 or pos > end_position) ML_EXCEPTION("cannot load tree, corrcupted saved tree", 0);
    pos_e = file_contents.find('\n', pos);
    if(pos_e != pos){
      curr_node->column_index = std::strtoul(file_contents.substr(pos+13,(pos_e-pos-13)).c_str(), NULL, 10);
    }

    /* node_value*/
    pos = file_contents.find("node_value=", begin_position);
    if(pos == -1 or pos > end_position) ML_EXCEPTION("cannot load tree, corrcupted saved tree", 0);
    pos_e = file_contents.find('\n', pos);
    if(pos_e != pos){
      curr_node->node_value = nodeValueDetermination(file_contents.substr(pos+11,(pos_e-pos-11)), dummy);
    }

    /* parent_node*/
    pos = file_contents.find("parent_node=", begin_position);
    if(pos == -1 or pos > end_position) ML_EXCEPTION("cannot load tree, corrcupted saved tree", 0);
    pos_e = file_contents.find('\n', pos);
    if(pos_e != pos){
      curr_node->parent_node = std::strtoul(file_contents.substr(pos+12,(pos_e-pos-12)).c_str(), NULL, 10);
    }

    /* branch nodes */
    pos = file_contents.find("branch_nodes=", begin_position);
    if(pos == -1 or pos > end_position) ML_EXCEPTION("cannot load tree, corrcupted saved tree", 0);
    pos+=12;// one less cause of loop
    pos_e = file_contents.find('\n', pos);
    if(pos_e != pos+1){
      pos_e=pos;
      while(file_contents.at(pos_e) != '\n'){
        pos_e++;
        if(file_contents.at(pos_e) == ',' or file_contents.at(pos_e) == '\n'){
          curr_node->branch_nodes.push_back(std::strtoul(file_contents.substr(pos+1,(pos_e-pos-1)).c_str(), NULL, 10));
          pos=pos_e;
        }
      }
    }

    loaded_nodes.push_back(curr_node);
    n_nodes++;
    begin_position++;

  }

  begin_position = file_contents.rfind("BEGIN LABELS");
  end_position = file_contents.find("END LABELS", begin_position);
  if(begin_position == -1 ) ML_EXCEPTION("No labels present in saved tree", 0);
  pos = file_contents.find('\n', begin_position);
  pos_e = file_contents.find('\n', pos+1);
  while(pos_e < end_position){
    decision_tree->labels.push_back(file_contents.substr(pos+1,(pos_e-pos-1)));
    pos = pos_e;
    pos_e = file_contents.find('\n', pos_e+1);
  }




  node<T> *root_node = decision_tree->findNode(0);
  createTreeFromLoadedNodes(*decision_tree, loaded_nodes, n_nodes++, root_node);

  /* free up loaded_nodes */
  typename std::vector<loaded_node<T>*>::iterator it_n;
  for(it_n= loaded_nodes.begin(); it_n != loaded_nodes.end(); ++it_n) delete *it_n;


  return;


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
template void mlglobal::Trainers::DecisionTree<int>::saveTree(std::string file_location);
template void  mlglobal::Trainers::loadTree(std::string file_location, mlglobal::Trainers::DecisionTree<int> *decision_tree);
template void createTreeFromLoadedNodes(mlglobal::Trainers::DecisionTree<int> &decision_tree, std::vector<loaded_node<int>*> loaded_nodes, size_t n_nodes, node<int> *curr_node);
template std::vector<int> mlglobal::Trainers::DecisionTree<int>::predict(mlglobal::DataContainers::LabeledData<int> data_container);
template int _predict(std::vector<int> input, node<int> *current_node, std::vector<int> matched_index);


/* for labeled float data_container */
template mlglobal::Trainers::DecisionTree<float>::DecisionTree();
template node<float>* mlglobal::Trainers::DecisionTree<float>::addBranch(node<float> *node_f, unsigned int column_index, float value);
template node<float>* mlglobal::Trainers::DecisionTree<float>::findNode(unsigned int node_index, node<float> *current_node);
template node<float>* mlglobal::Trainers::DecisionTree<float>::makeLeaf(node<float> *node_f, unsigned int column_index, float value);
template void mlglobal::Trainers::DecisionTree<float>::printTree(node<float> *current_node);
template void mlglobal::Trainers::DecisionTree<float>::saveTree(std::string file_location);
template void  mlglobal::Trainers::loadTree(std::string file_location, mlglobal::Trainers::DecisionTree<float> *decision_tree);
template void createTreeFromLoadedNodes(mlglobal::Trainers::DecisionTree<float> &decision_tree, std::vector<loaded_node<float>*> loaded_nodes, size_t n_nodes, node<float> *curr_node);
template std::vector<float> mlglobal::Trainers::DecisionTree<float>::predict(mlglobal::DataContainers::LabeledData<float> data_container);
template float _predict(std::vector<float> input, node<float> *current_node, std::vector<int> matched_index);


/* for labeled std::string data_container */
template mlglobal::Trainers::DecisionTree<std::string>::DecisionTree();
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::addBranch(node<std::string> *node_f, unsigned int column_index, std::string value);
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::findNode(unsigned int node_index, node<std::string> *current_node);
template node<std::string>* mlglobal::Trainers::DecisionTree<std::string>::makeLeaf(node<std::string> *node_f, unsigned int column_index, std::string value);
template void mlglobal::Trainers::DecisionTree<std::string>::printTree(node<std::string> *current_node);
template void mlglobal::Trainers::DecisionTree<std::string>::saveTree(std::string file_location);
template void  mlglobal::Trainers::loadTree(std::string file_location, mlglobal::Trainers::DecisionTree<std::string> *decision_tree);
template void createTreeFromLoadedNodes(mlglobal::Trainers::DecisionTree<std::string> &decision_tree, std::vector<loaded_node<std::string>*> loaded_nodes, size_t n_nodes, node<std::string> *curr_node);
template std::vector<std::string> mlglobal::Trainers::DecisionTree<std::string>::predict(mlglobal::DataContainers::LabeledData<std::string> data_container);
template std::string _predict(std::vector<std::string> input, node<std::string> *current_node, std::vector<int> matched_index);

//============================================
/* For decision tree algorithms */
//============================================

/* for labeled int data_container */
template mlglobal::Trainers::DecisionTree<int> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<int>(DataContainers::LabeledData<int> data_container, std::string target_attribute);
template mlglobal::Trainers::DecisionTree<int> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<int> data_container, std::string target_attribute, std::vector<std::string> attributes);
template mlglobal::Trainers::DecisionTree<int> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<int> data_container, std::string target_attribute, std::vector<unsigned int> attributes);
template void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<int>& decision_tree, node<int> *current_node, mlglobal::DataContainers::LabeledData<int>& data_container, unsigned int target, vector<unsigned int> iattributes);
template int mostCommonValue<int>(vector<int> data);
template vector<int> uniqueValues(vector<int> data);
template double weightedEntropy(vector<int> data, vector<int> target, vector<int> unique_target);
template double calcEntropy(vector<int> data, vector<int> unique_data);


/* for labeled float data_container */
template mlglobal::Trainers::DecisionTree<float> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<float>(DataContainers::LabeledData<float> data_container, std::string target_attribute);
template mlglobal::Trainers::DecisionTree<float> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<float> data_container, std::string target_attribute, std::vector<std::string> attributes);
template mlglobal::Trainers::DecisionTree<float> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<float> data_container, std::string target_attribute, std::vector<unsigned int> attributes);
template void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<float>& decision_tree, node<float> *current_node, mlglobal::DataContainers::LabeledData<float>& data_container, unsigned int target, vector<unsigned int> iattributes);
template float mostCommonValue<float>(vector<float> data);
template vector<float> uniqueValues(vector<float> data);
template double weightedEntropy(vector<float> data, vector<float> target, vector<float> unique_target);
template double calcEntropy(vector<float> data, vector<float> unique_data);

/* for labeled std::string data_container */
template mlglobal::Trainers::DecisionTree<std::string> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3<std::string>(DataContainers::LabeledData<std::string> data_container, std::string target_attribute);
template mlglobal::Trainers::DecisionTree<std::string> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<std::string> data_container, std::string target_attribute, std::vector<std::string> attributes);
template mlglobal::Trainers::DecisionTree<std::string> mlglobal::TrainingFunctions::DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<std::string> data_container, std::string target_attribute, std::vector<unsigned int> attributes);
template void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<std::string>& decision_tree, node<std::string> *current_node, mlglobal::DataContainers::LabeledData<std::string>& data_container, unsigned int target, vector<unsigned int> iattributes);
template std::string mostCommonValue<std::string>(vector<std::string> data);
template vector<std::string> uniqueValues(vector<std::string> data);
template double weightedEntropy(vector<std::string> data, vector<std::string> target, vector<std::string> unique_target);
template double calcEntropy(vector<std::string> data, vector<std::string> unique_data);


