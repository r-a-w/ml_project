//===================================================================================
/* Defines the training functions and trainer classes for decision trees
 * 
 *
 */
//===================================================================================


#ifndef DECISION_TREES_H
#define DECISION_TREES_H

#include "data_containers.h"
#include <vector>

template <typename T>
struct node{
  node<T> *parent_node;
  unsigned int node_index;
  unsigned int column_index;
  T node_value;
  std::vector<node<T>*> branch_nodes;
};

namespace mlglobal{

  namespace Trainers{

    /* Decision tree classes and functions */
    template <typename T> class DecisionTree {
      public:
        /* const'r */
        DecisionTree();
        node<T>* addBranch(node<T> *node_f, unsigned int column_index, T value);
        node<T>* makeLeaf(node<T> *node_f, unsigned int column_index, T value);
        node<T>* findNode(unsigned int node_index, node<T> *current_node=NULL);

        void printTree(node<T> *current_node=NULL);

      private:
        node<T> *root_node;
        size_t n_nodes;
    };


  } /* Trainers namespace */
} /* mlglobal namespace */


namespace mlglobal{
  namespace TrainingFunctions{
    /* wrapper for actual ID3 algorithm 
     *
     * this is the one to be used by user
    */
    
    template <typename T>  mlglobal::Trainers::DecisionTree<T> DecisionTree_Alg_ID3(mlglobal::DataContainers::LabeledData<T> data_container, std::string target_attribute, std::vector<std::string> attributes = std::vector<std::string>());
  } /* TrainingFunctions namespace */
} /* mlglobal namespace */



/* actual implentation of ID3 algorithm */
template <typename T>  void _DecisionTree_Alg_ID3(mlglobal::Trainers::DecisionTree<T>& decision_tree, node<T> *current_node, mlglobal::DataContainers::LabeledData<T>& data_container, unsigned int target,  std::vector<unsigned int> iattributes);

#endif 
