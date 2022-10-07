#ifndef VECBOS_JSON_FILTER_HH
#define VECBOS_JSON_FILTER_HH

#include <string>
#include "FWCore/Framework/interface/Event.h"

class JsonFilter {

 public:

  typedef std::pair<int,int> aLSSegment;
  typedef std::vector< std::pair<int,int> > LSSegments;
  typedef unsigned int aRun;
  typedef std::map< aRun, LSSegments > runsLSSegmentsMap;
  typedef std::pair < aRun, LSSegments > aRunsLSSegmentsMapElement;

  /// Constructor from JSON input file
  JsonFilter(std::string jsonFilePath, int verbose = 0);

  /// Destructor
  ~JsonFilter() { };

  /// Fill RunLSMap according to json file
  void fillRunLSMap();

  /// Set Good Run LS
  void setJsonGoodRunList(std::string jsonFilePath) { jsonFile_ = jsonFilePath; }

  /// Check if Run/LS is a good one
  bool isGoodRunLS(const edm::Event& iEvent);

  /// Return JSON file path
  std::string jsonFilePath();

  /// Return JSON file name
  std::string jsonFileName();
    
 private:

  /// Default constructor
  JsonFilter();
 
  int verbose_;
  runsLSSegmentsMap goodRunLS;
  std::string jsonFile_;
  int lastRun_, lastLumi_;
    
};

#endif
