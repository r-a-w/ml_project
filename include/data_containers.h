//=======================================================================
/*
 * \brief Declares the data containers and operation on those containers
 *
*/
//=======================================================================

#ifndef DATA_CONTAINERS_H
#define DATA_CONTAINERS_H

#include <iostream>
#include <vector>
#include "ml_exceptions.h"

namespace mlglobal{

namespace DataContainers{

  /* class definitions for labeled data container:
   *   appendData - adds datapoint to container
   *   appendLabel - adds new data label to container
   *   getDataByLabel - retireves all data associaated to label
   *   getDataByColumn - retrieve data based on column number (starts at 1!)
   *   getDataByIndex - retrieve data based on column index (starts at 0!)
   *   getSubset - retrieves data_container with subset of examples containing datapoint at column index
   *   getLabels - returns string vector of labels in container
   *   getIndexByLabel - return the data column index associated to the label
   *   printLabels - prints all labels in container
   *   printDataByLabel - print all data associated with label
   *   printDataByColumn - print data by column index (starts at 1!)
   *   numberOfColumns - returns number of columns (equal to number of labels)
   *   numberOfRows - returns number of rows (size of csv file)
   * 
  */
  template <typename T> class LabeledData{
    public:
      /* copy const. */
      //LabeledData(const LabeledData &obj);
      LabeledData();
      /* destr. */
      //~LabeledData();

      void appendData(std::string datapoint);
      void appendLabel(std::string addlabel);
      std::vector<T> getDataByLabel(std::string label);
      std::vector<T> getDataByColumn(size_t columnIndex);
      std::vector<T> getDataByRow(size_t rowIndex);
      std::vector<T> getDataByIndex(size_t index);
      LabeledData<T> getSubset(T datapoint, size_t index);
      std::vector<std::string> getLabels();
      size_t getIndexByLabel(std::string label);

      void printLabels(void);
      void printDataByLabel(std::string label);
      void printDataByColumn(size_t columnIndex);
      size_t numberOfColumns(void);
      size_t numberOfRows(void);

    private:
      std::vector<T> data;
      std::vector<std::string> labels;
      size_t ncolumns;
      size_t ndatapoints;


  };

  template <typename T>
  LabeledData<T>::LabeledData(){
  //  LabeledData<T>::LabeledData(const LabeledData<T> &obj){
    ncolumns=0;
    ndatapoints=0;
  }

  /*
   * Specilization declarations
  */
  template<> void LabeledData<int>::appendData(std::string datapoint);
  template<> void LabeledData<float>::appendData(std::string datapoint);
  template<> void LabeledData<std::string>::appendData(std::string datapoint);



  /* class definitions for unlabeled data container:
   *   appendData - adds datapoint to container
   *   addColumn - increase ncolumns by 1
   *   getDataByColumn - retrieve data based on column (starts at 1!)
   *   getDataByIndex - retrieve data based on index (starts at 0!)
   *   printDataByColumn - print data by column index (starts at 1!)
   *   numberOfColumns - returns number of columns (equal to number of labels)
   *   numberOfRows - returns number of rows (size of csv file)
   * 
  */
  template <typename T> class UnlabeledData{
      public:
        /* const. */
        UnlabeledData();
        /* destr. */
        //~UnlabeledData();

        void appendData(std::string datapoint);
        void addColumn(void);
        std::vector<T> getDataByColumn(size_t columnIndex);
        std::vector<T> getDataByIndex(size_t index);
        void printDataByColumn(size_t columnIndex);

        size_t numberOfColumns(void);
        size_t numberOfRows(void);

      private:
        std::vector<T> data;
        size_t ncolumns;
        size_t ndatapoints;


    };

    template <typename T>
      UnlabeledData<T>::UnlabeledData(void){
        ncolumns=0;
        ndatapoints=0;
      }

    /*
     * Specilization declarations
     */
    template<> void UnlabeledData<int>::appendData(std::string datapoint);
    template<> void UnlabeledData<float>::appendData(std::string datapoint);
    template<> void UnlabeledData<std::string>::appendData(std::string datapoint);

} /* DataContainers namespace */
} /* mlglobal namespace */

#endif
