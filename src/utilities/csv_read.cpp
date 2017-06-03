#include "data_containers.h" 
#include "csv_read.h"
#include <fstream>
#include <iostream>



/* quick function to grab line from string
 *
 * returns line without newline character 
 * if end of file is not reached iterator is increased 1 past the newline
 * 
*/
static inline std::string getLineString(std::string::iterator &it, std::string::iterator eof){
  std::string line;
  while(*it!='\n' and it!=eof){
    line.push_back(*it);
    ++it;
  }
  if(it!=eof) ++it;
  return line;
}

/* reads in labeled csv data and stores it in the data containers */
template <class T>
void mlglobal::readCSVFile(std::string file, DataContainers::LabeledData<T> & data_container, char delimiter, char comment, bool labellocation){
  std::ifstream stream(file.c_str());
  ML_EXCEPTION("Could not open file stream", stream);

  /* read all file contents */
  stream.seekg(0, std::ios::end);
  size_t size = stream.tellg();
  std::string file_contents(size, ' ');
  stream.seekg(0, std::ios::beg);
  stream.read(&file_contents[0], size);

  /*close file*/
  stream.close();


  char eol='\n';
  std::string label;
  std::string::iterator it;
  std::string::iterator eof = file_contents.end();
  /* get label location and read in labels */
  if(labellocation) it=file_contents.begin();
  else{
    it=eof-1;
    while(*it!=eol) --it;
    ++it;
  }

  std::string line = getLineString(it, eof);
  std::string::iterator it_l;
  for(it_l=line.begin();it_l!=line.end();++it_l){
    if(*it_l!='\"' and *it_l!=0x20 and *it_l!=delimiter) label.push_back(*it_l); /* 0x20 is whtespace */
    if(*it_l==delimiter or it_l==line.end()-1){
      data_container.appendLabel(label);
      label.clear();
    }
  }
  //data_container.printLabels();

  /* parse contents and store in data container */
  std::string datapoint;
  if(!labellocation) it = file_contents.begin();
  while(it!=eof){
    line = getLineString(it, eof);
    for(it_l=line.begin();it_l!=line.end();++it_l){
      if(*it_l!='\"' and *it_l!=delimiter) datapoint.push_back(*it_l);
      if(*it_l==delimiter or it_l==line.end()-1){
        while(std::isspace(datapoint.at(0))) datapoint.erase(datapoint.begin()); /* remove extraneous whitespace */
        while(std::isspace(datapoint.at(datapoint.size()-1))) datapoint.erase(datapoint.end()-1);
        data_container.appendData(datapoint);
        datapoint.clear();
      }
    }
  }


} /* readCSVFile */


/* 
 * Explicit template instantiations
 */
template void mlglobal::readCSVFile<int>(std::string file, DataContainers::LabeledData<int> & data_container, char delimiter, char comment, bool labellocation);
template void mlglobal::readCSVFile<float>(std::string file, DataContainers::LabeledData<float> & data_container, char delimiter, char comment, bool labellocation);
template void mlglobal::readCSVFile<std::string>(std::string file, DataContainers::LabeledData<std::string> & data_container, char delimiter, char comment, bool labellocation);



/* reads in unlabeled csv data and stores it in the data containers */
template <class T>
void mlglobal::readCSVFile(std::string file, DataContainers::UnlabeledData<T> & data_container, char delimiter, char comment){
  std::ifstream stream(file.c_str());
  ML_EXCEPTION("Could not open file stream", stream);


  /* read all file contents */
  stream.seekg(0, std::ios::end);
  size_t size = stream.tellg();
  std::string file_contents(size, ' ');
  stream.seekg(0, std::ios::beg);
  stream.read(&file_contents[0], size);

  /*close file*/
  stream.close();

  /* read number of columns */
  std::string::iterator it = file_contents.begin();
  std::string::iterator eof = file_contents.end();
  std::string line = getLineString(it, eof);
  std::string::iterator it_l;
  for(it_l=line.begin();it_l!=line.end();++it_l){
    if(*it_l==delimiter or it_l==line.end()-1) data_container.addColumn();
  }

  /* parse contents and store in data container */
  char eol='\n';
  std::string datapoint;
  it = file_contents.begin();
  while(it!=eof){
    line = getLineString(it, eof);
    for(it_l=line.begin();it_l!=line.end();++it_l){
      if(*it_l!='\"' and *it_l!=delimiter) datapoint.push_back(*it_l);
      if(*it_l==delimiter or it_l==line.end()-1){
        while(std::isspace(datapoint.at(0))) datapoint.erase(datapoint.begin()); /* remove extraneous whitespace */
        while(std::isspace(datapoint.at(datapoint.size()-1))) datapoint.erase(datapoint.end()-1);
        data_container.appendData(datapoint);
        datapoint.clear();
      }
    }
  }


} /* readCSVFile */


/* 
 * Explicit template instantiations
 */
template void mlglobal::readCSVFile<int>(std::string file, DataContainers::UnlabeledData<int> & data_container, char delimiter, char comment);
template void mlglobal::readCSVFile<float>(std::string file, DataContainers::UnlabeledData<float> & data_container, char delimiter, char comment);
template void mlglobal::readCSVFile<std::string>(std::string file, DataContainers::UnlabeledData<std::string> & data_container, char delimiter, char comment);

