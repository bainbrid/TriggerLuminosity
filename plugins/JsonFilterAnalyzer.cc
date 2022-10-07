#include "PhysicsTools/TriggerLuminosity/interface/JsonFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//
class JsonFilterAnalyzer : public edm::one::EDAnalyzer<> {

public:
  
  explicit JsonFilterAnalyzer(const edm::ParameterSet&);
  ~JsonFilterAnalyzer() override;
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  int verbose_;
  std::vector<std::string> jsonFiles_;
  std::vector<JsonFilter> jsonFilters_;
  
};

////////////////////////////////////////////////////////////////////////////////
//
JsonFilterAnalyzer::JsonFilterAnalyzer(const edm::ParameterSet& iConfig) 
  : verbose_(iConfig.getParameter<int>("Verbose")),
    jsonFiles_(iConfig.getParameter<std::vector<std::string> >("JsonFiles")),
    jsonFilters_()
{
  for ( auto const& file : jsonFiles_ ) {
    if (verbose_>0) { 
      std::cout << "[JsonFilterAnalyzer::JsonFilterAnalyzer] Parsing JSON file path: " << file << std::endl;
    }
    JsonFilter filter(file,verbose_);
    filter.fillRunLSMap();
    jsonFilters_.push_back(filter);
    if (verbose_>0) { 
      std::cout << "[JsonFilterAnalyzer::JsonFilterAnalyzer] Parsed JSON file name: " << filter.jsonFileName() << std::endl;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
JsonFilterAnalyzer::~JsonFilterAnalyzer() {
}

////////////////////////////////////////////////////////////////////////////////
//
void JsonFilterAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  auto run = iEvent.id().run();
  auto lumi = iEvent.luminosityBlock();
  auto event = iEvent.id().event();
  if (verbose_>0) {
    std::cout << "[JsonFilterAnalyzer::analyze]"
	      << " Run: " << run << " LS: " << lumi << " Event: " << event << std::endl;
  }
  if (verbose_>1) {
    std::cout << "[JsonFilterAnalyzer::analyze]"
	      << " The following JSONs identify a valid run/lumi: " << std::endl;
    for ( auto& filter : jsonFilters_ ) {
      if ( filter.isGoodRunLS(iEvent) ) { std::cout << "  " << filter.jsonFileName() << std::endl; }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//
void JsonFilterAnalyzer::beginJob() {
}

////////////////////////////////////////////////////////////////////////////////
//
void JsonFilterAnalyzer::endJob() {
}

void JsonFilterAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(JsonFilterAnalyzer);
