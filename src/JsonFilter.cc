#include "../interface/elements.h"
#include "../interface/reader.h"
#include "../interface/JsonFilter.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// 
JsonFilter::JsonFilter()
  : verbose_(0),
    goodRunLS(),
    jsonFile_(std::string("")),
    lastRun_(-1),
    lastLumi_(-1)
{;}

////////////////////////////////////////////////////////////////////////////////
// 
JsonFilter::JsonFilter(std::string jsonFilePath, int verbose )
  : verbose_(verbose),
    goodRunLS(),
    jsonFile_(jsonFilePath),
    lastRun_(-1),
    lastLumi_(-1)
{;}

////////////////////////////////////////////////////////////////////////////////
// 
void JsonFilter::fillRunLSMap() {

  if (jsonFile_ == "")
    {
      std::cout << "[JsonFilter::fillRunLSMap] Cannot fill RunLSMap. json file not configured" << std::endl;
      return;
    }

  std::ifstream jsonFileStream;
  jsonFileStream.open(jsonFile_.c_str());
  if (!jsonFileStream.is_open())
    {
      std::cout << "[JsonFilter::fillRunLSMap] Unable to open file " << jsonFile_ << std::endl;
      return;
    }

  json::Object elemRootFile;
  json::Reader::Read(elemRootFile, jsonFileStream);
  
  for (json::Object::const_iterator itRun=elemRootFile.Begin();itRun!=elemRootFile.End();++itRun) {
    const json::Array& lsSegment = (*itRun).element;
    LSSegments thisRunSegments; 
    for (json::Array::const_iterator lsIterator=lsSegment.Begin();lsIterator!=lsSegment.End();++lsIterator)
      {
	json::Array lsSegment=(*lsIterator);
	json::Number lsStart=lsSegment[0];           
	json::Number lsEnd=lsSegment[1];
	aLSSegment thisSegment;
	thisSegment.first=lsStart.Value();
	thisSegment.second=lsEnd.Value();
	thisRunSegments.push_back(thisSegment);
	//std::pair<int, int> lsSegment=std::pair<int, int>(atoi(,lsIterator[1]); 
      }
    goodRunLS.insert(aRunsLSSegmentsMapElement(atoi((*itRun).name.c_str()),thisRunSegments));
  }

  if (verbose_>0) std::cout << "[JsonFilter::fillRunLSMap]"
			    << " Good Run LS map filled with " << goodRunLS.size() << " runs" << std::endl;
  for (runsLSSegmentsMap::const_iterator itR=goodRunLS.begin(); itR!=goodRunLS.end(); ++itR)
    {
      if (verbose_>0) std::cout << "[JsonFilter::fillRunLSMap] Run " << (*itR).first <<  " LS ranges are: ";
      for (LSSegments::const_iterator iSeg=(*itR).second.begin();iSeg!=(*itR).second.end();++iSeg)
	if (verbose_>0) std::cout << "[" << (*iSeg).first << "," << (*iSeg).second << "] "; 
      if (verbose_>0) std::cout << std::endl;
    }
  if (verbose_>0) std::cout << "[JsonFilter::fillRunLSMap]"
			    << " And this is all the data I will consider, nothing more. " << std::endl;

}

////////////////////////////////////////////////////////////////////////////////
// 
bool JsonFilter::isGoodRunLS(int run, int lumi) {

  if (jsonFile_ == "") return true;

  runsLSSegmentsMap::const_iterator thisRun=goodRunLS.find(run);
  if (thisRun == goodRunLS.end()) {
    if (verbose_>1) std::cout << "[JsonFilter::isGoodRunLS] Run " << run << " is rejected" << std::endl;
    return false;
  }
  for (LSSegments::const_iterator iSeg=goodRunLS[run].begin();iSeg!=goodRunLS[run].end();++iSeg)
    {
      if ( lumi >= (*iSeg).first && lumi <= (*iSeg).second) {
        if(lastRun_ != run || lastLumi_ != lumi) {
          lastRun_ = run;  
          lastLumi_ = lumi;
	  if (verbose_>1) std::cout << "[JsonFilter::isGoodRunLS]"
				    << " Run " << lastRun_ << " LS " << lastLumi_ << " is OK" << std::endl;
        }
        return true;
      }
    }
  if(lastRun_ != run || lastLumi_ != lumi) {
    lastRun_ = run;  
    lastLumi_ = lumi;
  }
  if (verbose_>1) std::cout << "[JsonFilter::isGoodRunLS]"
			    << " Run " << lastRun_ << " LS " << lastLumi_ << " is rejected" << std::endl;
  return false;

}

////////////////////////////////////////////////////////////////////////////////
// 
std::string JsonFilter::jsonFilePath() {
  return jsonFile_;
}

////////////////////////////////////////////////////////////////////////////////
// 
std::string JsonFilter::jsonFileName(const std::string& path) {
  size_t slash = path.find_last_of('/');
  size_t dot = path.find_last_of('.');
  if (slash != string::npos && dot != string::npos ) { return path.substr(slash+1,dot-slash-1); }
  else if (dot != string::npos )                     { return path.substr(0,dot-1); }
  else                                               { return path; }
}
