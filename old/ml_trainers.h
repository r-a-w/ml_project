//===================================================================================
/* Defines the trainier objects
 * 
 *
 */
//===================================================================================

#ifndef ML_TRAINERS_H
#define ML_TRAINERS_H

namespace mlglobal{

  namespace Trainers{

    /* Decision tree classes and functions */
    class DecisionTree {
      public:
        /* const'r */
        DecisionTree(): x(4){}
        void print_value();

      private:
        int x;
    };


  } /* Trainers namespace */
} /* mlglobal namespace */


#endif

