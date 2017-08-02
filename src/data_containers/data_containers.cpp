#include "data_containers.h"
#include <iostream>

using namespace mlglobal::DataContainers;


//=====================================================================
/* 
 * Function Definitions for Labeled Data 
*/
//=====================================================================

/* add row label to data container */
template <typename T> void LabeledData<T>::appendLabel(std::string addlabel){
  labels.push_back(addlabel);
  ++ncolumns;
}

/* add integer data to data container */
template<> void LabeledData<int>::appendData(std::string datapoint){
  data.push_back(std::stoi(datapoint));
  ++ndatapoints;
}
/* add float data to data container */
template<> void LabeledData<float>::appendData(std::string datapoint){
  data.push_back(std::stof(datapoint));
  ++ndatapoints;
}

/* add string data to data container */
template<> void LabeledData<std::string>::appendData(std::string datapoint){
  data.push_back(datapoint);
  ++ndatapoints;
}

/* print all labels in container */
template <typename T> void LabeledData<T>::printLabels(void){
  std::vector<std::string>::iterator it=labels.begin();
  if(it==labels.end()){
      std::cout << "No labels in container" << std::endl;
      return;
  }
  for(;it!=labels.end();++it){
    std::cout << *it << std::endl;
  }
  return;
}



template <typename T> void LabeledData<T>::printDataByLabel(std::string label){
  for(std::vector<std::string>::iterator it=labels.begin();it!=labels.end();++it){
    if(!label.compare(*it)){
      int index = it-labels.begin();
      for(typename std::vector<T>::iterator it_d=data.begin()+index;it_d<data.end();it_d+=ncolumns) std::cout << *it_d << std::endl;
      return;
    }
  }
  std::cout << "No label named \"" << label << "\"" << std::endl;
  ML_EXCEPTION("label error",0);
  return;
}


template <typename T> std::vector<T> LabeledData<T>::getDataByLabel(std::string label){
  for(std::vector<std::string>::iterator it=labels.begin();it!=labels.end();++it){
    if(!label.compare(*it)){
      std::vector<T> labelData;
      int index = it-labels.begin();
      for(typename std::vector<T>::iterator it_d=data.begin()+index;it_d<data.end();it_d+=ncolumns) labelData.push_back(*it_d);
      return labelData;
    }
  }
  std::cout << "Cannot retrieve data, no label named \"" << label << "\"" << std::endl;
  ML_EXCEPTION("label error",0);
  return std::vector<T>();
}

template <typename T> LabeledData<T> LabeledData<T>::getSubset(T datapoint, size_t index){

  LabeledData<T> subset_container(*this);
  typename std::vector<T>::iterator it_d;
  unsigned int i;
  for(it_d=subset_container.data.begin()+index;it_d<subset_container.data.end();){
    if(*it_d!=datapoint){
      it_d-=index;
      for(i=0;i<ncolumns;++i){
        it_d=subset_container.data.erase(it_d);
      }
      subset_container.ndatapoints-=ncolumns;
      it_d+=index;
    }else{
      it_d+=ncolumns;
    }
  }

  return subset_container;

}

template <typename T> std::vector<std::string> LabeledData<T>::getLabels(){
  return labels;
}

template <typename T> size_t LabeledData<T>::getIndexByLabel(std::string label){
  ML_EXCEPTION("Cannot get index, labels are empty", labels.size());
  size_t index=0;
  for(std::vector<std::string>::iterator it=labels.begin();it!=labels.end();++it){
    if(!label.compare(*it)) return index;
    else ++index;
  }
  std::cout << "no label named " << label << " in data container" << std::endl;
  ML_EXCEPTION("index error", 0);
  return index;
}


template <typename T> void LabeledData<T>::printDataByColumn(size_t columnIndex){
  if(columnIndex > ncolumns){
    std::cout << "Cannot print data, column index out of range, index = " << columnIndex << std::endl;
    ML_EXCEPTION("index error",0);
    return;
  }
  for(typename std::vector<T>::iterator it_d=data.begin()+columnIndex-1;it_d<data.end();it_d+=ncolumns) std::cout << *it_d << std::endl;
}

template <typename T> std::vector<T> LabeledData<T>::getDataByColumn(size_t columnIndex){
  if(columnIndex > ncolumns){
    std::cout << "Cannot retrieve data, column index out of range, index = " << columnIndex << std::endl;
    ML_EXCEPTION("index error",0);
    return std::vector<T>();
  }
  std::vector<T> columnData;
  for(typename std::vector<T>::iterator it_d=data.begin()+columnIndex-1;it_d<data.end();it_d+=ncolumns) columnData.push_back(*it_d);
  return columnData;

}


template <typename T> std::vector<T> LabeledData<T>::getDataByRow(size_t rowIndex){
  if(rowIndex > ndatapoints/ncolumns){
    std::cout << "Cannot retrieve data, row index out of range, index = " << rowIndex << std::endl;
    ML_EXCEPTION("index error", 0);
  }
  std::vector<T> row_data;
  unsigned int i;
  for(i = 0;i < ncolumns;++i) row_data.push_back(data.at(rowIndex*ncolumns+i));
  return row_data;
}



template <typename T> std::vector<T> LabeledData<T>::getDataByIndex(size_t index){
  if(index > ncolumns-1){
    std::cout << "Cannot retrieve data, column index out of range, index = " << index << std::endl;
    ML_EXCEPTION("index error",0);
    return std::vector<T>();
  }
  std::vector<T> columnData;
  for(typename std::vector<T>::iterator it_d=data.begin()+index;it_d<data.end();it_d+=ncolumns) columnData.push_back(*it_d);
  return columnData;

}


template <typename T> size_t LabeledData<T>::numberOfColumns(){
  return ncolumns;
}

template <typename T> size_t LabeledData<T>::numberOfRows(){
  return ndatapoints/ncolumns;
}


/*
 * explicit instansiations
*/
/* for int data */
template void LabeledData<int>::appendLabel(std::string addlabel);
template void LabeledData<int>::printLabels(void);
template void LabeledData<int>::printDataByLabel(std::string label);
template void LabeledData<int>::printDataByColumn(size_t columnIndex);
template size_t LabeledData<int>::numberOfRows(void);
template size_t LabeledData<int>::numberOfColumns(void);
template std::vector<int> LabeledData<int>::getDataByLabel(std::string label);
template std::vector<int> LabeledData<int>::getDataByColumn(size_t columnIndex);
template std::vector<int> LabeledData<int>::getDataByRow(size_t rowIndex);
template std::vector<int> LabeledData<int>::getDataByIndex(size_t index);
template LabeledData<int> LabeledData<int>::getSubset(int datapoint, size_t index);
template std::vector<std::string> LabeledData<int>::getLabels();
template size_t LabeledData<int>::getIndexByLabel(std::string label);

/* for float data */
template void LabeledData<float>::appendLabel(std::string addlabel);
template void LabeledData<float>::printLabels(void);
template void LabeledData<float>::printDataByLabel(std::string label);
template void LabeledData<float>::printDataByColumn(size_t columnIndex);
template size_t LabeledData<float>::numberOfRows(void);
template size_t LabeledData<float>::numberOfColumns(void);
template std::vector<float> LabeledData<float>::getDataByLabel(std::string label);
template std::vector<float> LabeledData<float>::getDataByColumn(size_t columnIndex);
template std::vector<float> LabeledData<float>::getDataByRow(size_t rowIndex);
template std::vector<float> LabeledData<float>::getDataByIndex(size_t index);
template LabeledData<float> LabeledData<float>::getSubset(float datapoint, size_t index);
template std::vector<std::string> LabeledData<float>::getLabels();
template size_t LabeledData<float>::getIndexByLabel(std::string label);

/* for string data */
template void LabeledData<std::string>::appendLabel(std::string addlabel);
template void LabeledData<std::string>::printLabels(void);
template void LabeledData<std::string>::printDataByLabel(std::string label);
template void LabeledData<std::string>::printDataByColumn(size_t columnIndex);
template size_t LabeledData<std::string>::numberOfRows(void);
template size_t LabeledData<std::string>::numberOfColumns(void);
template std::vector<std::string> LabeledData<std::string>::getDataByLabel(std::string label);
template std::vector<std::string> LabeledData<std::string>::getDataByColumn(size_t columnIndex);
template std::vector<std::string> LabeledData<std::string>::getDataByRow(size_t rowIndex);
template std::vector<std::string> LabeledData<std::string>::getDataByIndex(size_t index);
template LabeledData<std::string> LabeledData<std::string>::getSubset(std::string datapoint, size_t index);
template std::vector<std::string> LabeledData<std::string>::getLabels();
template size_t LabeledData<std::string>::getIndexByLabel(std::string label);






//=====================================================================
/* 
 * Function Definitions for Unlabeled Data 
 */
//=====================================================================

/* add integer data to data container */
template<> void UnlabeledData<int>::appendData(std::string datapoint){
  data.push_back(std::stoi(datapoint));
  ++ndatapoints;
}
/* add float data to data container */
template<> void UnlabeledData<float>::appendData(std::string datapoint){
  data.push_back(std::stof(datapoint));
  ++ndatapoints;
}

/* add string data to data container */
template<> void UnlabeledData<std::string>::appendData(std::string datapoint){
  data.push_back(datapoint);
  ++ndatapoints;
}


template <typename T> void UnlabeledData<T>::addColumn(void){
  ++ncolumns;
}


template <typename T> std::vector<T> UnlabeledData<T>::getDataByColumn(size_t columnIndex){
  if(columnIndex > ncolumns){
    std::cout << "Cannot retrieve data, column index out of range, index = " << columnIndex << std::endl;
    ML_EXCEPTION("index error",0);
    return std::vector<T>();
  }
  std::vector<T> columnData;
  for(typename std::vector<T>::iterator it_d=data.begin()+columnIndex-1;it_d<data.end();it_d+=ncolumns) columnData.push_back(*it_d);
  return columnData;

}

template <typename T> std::vector<T> UnlabeledData<T>::getDataByIndex(size_t index){
  if(index > ncolumns-1){
    std::cout << "Cannot retrieve data, column index out of range, index = " << index << std::endl;
    ML_EXCEPTION("index error",0);
  }
  std::vector<T> columnData;
  for(typename std::vector<T>::iterator it_d=data.begin()+index;it_d<data.end();it_d+=ncolumns) columnData.push_back(*it_d);
  return columnData;

}


template <typename T> void UnlabeledData<T>::printDataByColumn(size_t columnIndex){
  if(columnIndex > ncolumns){
    std::cout << "Cannot print data, column index out of range, index = " << columnIndex << std::endl;
    ML_EXCEPTION("index error",0);
    return;
  }
  for(typename std::vector<T>::iterator it_d=data.begin()+columnIndex-1;it_d<data.end();it_d+=ncolumns) std::cout << *it_d << std::endl;
}





template <typename T> size_t UnlabeledData<T>::numberOfColumns(){
  return ncolumns;
}

template <typename T> size_t UnlabeledData<T>::numberOfRows(){
  return ndatapoints/ncolumns;
}


/*
 * explicit instansiations
 */
/* for int data */
template size_t UnlabeledData<int>::numberOfRows(void);
template size_t UnlabeledData<int>::numberOfColumns(void);
template std::vector<int> UnlabeledData<int>::getDataByColumn(size_t columnIndex);
template std::vector<int> UnlabeledData<int>::getDataByIndex(size_t index);
template void UnlabeledData<int>::printDataByColumn(size_t columnIndex);
template void UnlabeledData<int>::addColumn(void);

/* for float data */
template size_t UnlabeledData<float>::numberOfRows(void);
template size_t UnlabeledData<float>::numberOfColumns(void);
template std::vector<float> UnlabeledData<float>::getDataByColumn(size_t columnIndex);
template std::vector<float> UnlabeledData<float>::getDataByIndex(size_t index);
template void UnlabeledData<float>::printDataByColumn(size_t columnIndex);
template void UnlabeledData<float>::addColumn(void);

/* for string data */
template size_t UnlabeledData<std::string>::numberOfRows(void);
template size_t UnlabeledData<std::string>::numberOfColumns(void);
template std::vector<std::string> UnlabeledData<std::string>::getDataByColumn(size_t columnIndex);
template std::vector<std::string> UnlabeledData<std::string>::getDataByIndex(size_t index);
template void UnlabeledData<std::string>::printDataByColumn(size_t columnIndex);
template void UnlabeledData<std::string>::addColumn(void);

