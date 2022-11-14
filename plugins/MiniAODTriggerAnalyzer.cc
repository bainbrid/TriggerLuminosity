#include "CondFormats/DataRecord/interface/L1TUtmTriggerMenuRcd.h"
#include "CondFormats/L1TObjects/interface/L1TUtmTriggerMenu.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"
#include <cmath>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>

//#define CMSSW_10_2_X

typedef std::vector<int> LS;
typedef std::map<int,LS> JSON;
typedef std::pair<std::string,std::string> TRG;
typedef std::map<TRG,JSON> JSONS;

////////////////////////////////////////////////////////////////////////////////
//
class MiniAODTriggerAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns> {

public:

  explicit MiniAODTriggerAnalyzer (const edm::ParameterSet&);
  ~MiniAODTriggerAnalyzer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:

  void beginJob() override;
  void endJob() override;
  void beginRun(const edm::Run & run, const edm::EventSetup & iSetup) override;
  void endRun(const edm::Run & run, const edm::EventSetup & iSetup) override {;}
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;

  void L1Prescales(const edm::Event&, 
		   const edm::EventSetup&,
		   const edm::EDGetTokenT<GlobalAlgBlkBxCollection>&);
  void printPathsAndObjects(const edm::Handle<edm::TriggerResults>&,
			    const edm::Handle<pat::PackedTriggerPrescales>&,
			    const edm::Handle<std::vector<pat::TriggerObjectStandAlone> >&,
			    const edm::TriggerNames&);
  void printJSONs();
  size_t getPathIndex(const std::string& hltPath, const edm::TriggerNames&);
    
  HLTPrescaleProvider hltPrescaleProvider_;
  std::string const hltProcess_;
  std::vector<std::string> hltPaths_;
  std::vector<std::string> l1Seeds_;
  //
  const edm::EDGetTokenT<edm::TriggerResults> triggerResults_;
  const edm::EDGetTokenT<std::vector<pat::TriggerObjectStandAlone> > triggerObjects_;
  const edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;
  JSONS jsons_;
  int verbose_;
  bool onlyLowestUnprescaledHltPath_;
  int moduloPrescale_;
};

////////////////////////////////////////////////////////////////////////////////
//
MiniAODTriggerAnalyzer::MiniAODTriggerAnalyzer(const edm::ParameterSet& iConfig):
  hltPrescaleProvider_(iConfig.getParameter<edm::ParameterSet>("cfg"), consumesCollector(), *this),
  hltProcess_(iConfig.getParameter<std::string>("HLTProcess")),
  hltPaths_(iConfig.getParameter<std::vector<std::string> >("HLTPaths")),
  l1Seeds_(iConfig.getParameter<std::vector<std::string> >("L1Seeds")),
  triggerResults_(consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("bits"))),
  triggerObjects_(consumes<std::vector<pat::TriggerObjectStandAlone> >(iConfig.getParameter<edm::InputTag>("objects"))),
  triggerPrescales_(consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("prescales"))),
  jsons_{},
  verbose_(iConfig.getParameter<int>("Verbose")),
  onlyLowestUnprescaledHltPath_(iConfig.getParameter<bool>("OnlyLowestUnprescaledHltPath")),
  moduloPrescale_(iConfig.getParameter<int>("ModuloPrescale"))
  {;}

////////////////////////////////////////////////////////////////////////////////
//
MiniAODTriggerAnalyzer::~MiniAODTriggerAnalyzer() {}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::beginJob() {}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::endJob() {
  std::cout << "[MiniAODTriggerAnalyzer::endJob]" << std::endl;
  printJSONs();
}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  //std::cout << "[MiniAODTriggerAnalyzer::beginRun]" << std::endl;
  bool changed = false;
  bool ok = hltPrescaleProvider_.init(iRun,iSetup,hltProcess_,changed);
  if (!ok) {
    std::cerr << " HLT config extraction failure with process name HLT" << std::endl;
    return;
  }
  if (changed) { std::cout << "HLTPrescaleProvider changed..."  << std::endl; }
}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addWithDefaultLabel(desc);
}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::printJSONs() {

  // Init
  std::string name = "output";
  std::ofstream file;
  file.open(name+".json");
  std::stringstream ss;
  //ss << name << std::endl;
  ss << "{" << std::endl;

  uint idx1 = 0;
  for ( auto iter : jsons_ ) { 
  
    // Find JSON map for given L1,HLT combination
    TRG trigger = iter.first;
    auto json = jsons_.find(trigger);
    if (json == jsons_.end()) { 
      ss << "Trigger not found!" << std::endl; 
      std::cout << ss.str() << std::endl;
      return;
    }
    
    ss << "\"L1_" << trigger.first << "_HLT_" << trigger.second << "\":" << std::endl;
    
    // Create JSON output with format: { "run": [[ls,ls]], ... }
    ss << "{" << std::endl;
    uint idx2 = 0;
    for (auto run : json->second) {
      ss << "\"" << int(run.first) << "\": ["; // Run number
      int previous = 0;
      auto ls = run.second;
      std::sort(ls.begin(),ls.end());
      for ( uint idx = 0; idx < ls.size(); ++idx ) { // Lumi sections
	if      (ls.size() == 1)       { ss << "[" << ls[idx] << ", " << ls[idx] << "]"; } // Only a single LS
	else if (idx == 0)             { ss << "[" << ls[idx] << ", "; }                   // First LS (of multiple)
	else if (idx+1 == ls.size())   { ss << ls[idx] << "]"; }                           // Final LS (of multiple)
	else if (ls[idx] > previous+1) { ss << previous << "], [" << ls[idx] << ", "; }    // Non-consecutive LS
	previous = ls[idx];
      }
      // no comma for last entry!
      ss << "]";
      if (idx2+1<json->second.size()) { ss << ","; }
      ++idx2;
      ss << std::endl;
    }

    // no comma for last entry!
    ss << "}";
    if (idx1+1<jsons_.size()) { ss << ","; }
    ++idx1;
    ss << std::endl;

  }
  ss << "}" << std::endl;

  // Output 
  file << ss.str();
  file.close();
  if (verbose_>0) { std::cout << ss.str() << std::endl; }

}
  
////////////////////////////////////////////////////////////////////////////////
//
size_t MiniAODTriggerAnalyzer::getPathIndex(const std::string& hltPath,
					    const edm::TriggerNames& triggerNames) {
  for(size_t index = 0;index<triggerNames.size(); index++){
    if(triggerNames.triggerName(index).find(hltPath)==0){
      return index;
    }
  }
  return triggerNames.size();
}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::analyze(const edm::Event& iEvent,
				     const edm::EventSetup& iSetup) {

  auto event = iEvent.id().event();
  if ( moduloPrescale_ > 1 && event % moduloPrescale_ != 0 ) return;

#ifndef CMSSW_10_2_X
  auto triggerResultsHandle = iEvent.getHandle(triggerResults_);
  auto triggerObjectsHandle = iEvent.getHandle(triggerObjects_);
  auto triggerPrescalesHandle = iEvent.getHandle(triggerPrescales_);
#else
  edm::Handle<edm::TriggerResults> triggerResultsHandle;
  iEvent.getByToken(triggerResults_, triggerResultsHandle);
  edm::Handle<std::vector<pat::TriggerObjectStandAlone> > triggerObjectsHandle;
  iEvent.getByToken(triggerObjects_, triggerObjectsHandle);
  edm::Handle<pat::PackedTriggerPrescales > triggerPrescalesHandle;
  iEvent.getByToken(triggerPrescales_, triggerPrescalesHandle);
#endif
  const edm::TriggerNames& triggerNames = iEvent.triggerNames(*triggerResultsHandle);

  //printPathsAndObjects(triggerResultsHandle, triggerPrescalesHandle, triggerObjectsHandle, triggerNames);
  
  for ( const auto& hltPath: hltPaths_ ) {

    //std::cout << "HLT path: " << hltPath << std::endl;
    std::string hltPathVersioned;
    size_t pathIndex = getPathIndex(hltPath,triggerNames);
    if ( pathIndex >= triggerNames.size() ) {
      std::cout << "hltPath " << hltPath << " is not found in the trigger menu!" << std::endl;
      continue;
    } else { hltPathVersioned = triggerNames.triggerName(pathIndex); }
    //std::cout << "HLT path (versioned): " << hltPathVersioned << std::endl;
    
    int prescaleSet = hltPrescaleProvider_.prescaleSet(iEvent, iSetup);
#ifndef CMSSW_10_2_X
    auto const l1HLTDetailPSDouble = hltPrescaleProvider_.prescaleValuesInDetail<double>(iEvent, iSetup, hltPathVersioned);
#else
    auto const l1HLTDetailPSDouble = hltPrescaleProvider_.prescaleValuesInDetail(iEvent, iSetup, hltPathVersioned);
#endif
    
    std::string hlt_path = hltPath; // Don't use versioned here
    double hlt_prescale = l1HLTDetailPSDouble.second;
    std::string l1_seed = "";
    double l1_prescale = 0;
    for (const auto& entry : l1HLTDetailPSDouble.first) {
      if (int(entry.second)>0) {
	l1_seed = entry.first;
	l1_prescale = entry.second;
	break; // Always find lowest unprescaled L1 seed ...
      }
    }

    if (hlt_prescale>0 && l1_prescale>0) {
      if (verbose_>1) {
	std::cout << "HLT path (prescale): " << hlt_path 
		  << "(" << hlt_prescale << ")"
		  << " L1 seed (prescale): " << l1_seed 
		  << "(" << l1_prescale << ")"
		  << " Prescale column: " << prescaleSet
		  << std::endl;
      }
      auto run = iEvent.id().run();
      auto ls = iEvent.luminosityBlock();
      //auto event = iEvent.id().event();

      // Reformat L1 seed and HLT path strings
      // L1:  L1_DoubleEGXXpX_er1p2_dR_Max0p6
      // HLT: HLT_DoubleEleXXpX_eta1p22_mMax6
      std::string delimiter = "";
      std::string l1_str = l1_seed;
      delimiter = "L1_DoubleEG";
      l1_str = l1_str.substr(l1_str.find(delimiter)+delimiter.length(),std::string::npos);
      delimiter = "_er1p2";
      l1_str = l1_str.substr(0,l1_str.find(delimiter));
      if (l1_str.find("p")==std::string::npos) { l1_str.append("p0"); }
      std::string hlt_str = hlt_path;
      delimiter = "HLT_DoubleEle";
      hlt_str = hlt_str.substr(hlt_str.find(delimiter)+delimiter.length(),std::string::npos);
      delimiter = "_eta1p22";
      hlt_str = hlt_str.substr(0,hlt_str.find(delimiter));
      if (hlt_str.find("p")==std::string::npos) { hlt_str.append("p0"); }
      
      // Add entry to JSON maps
      TRG trigger(l1_str,hlt_str);
      auto entry1 = jsons_.find(trigger);
      if (entry1 == jsons_.end()) { jsons_[trigger] = JSON(); }
      auto entry2 = jsons_[trigger].find(run);
      if (entry2 == jsons_[trigger].end()) { jsons_[trigger][run] = LS(); }
      auto entry3 = std::find(jsons_[trigger][run].begin(),jsons_[trigger][run].end(),ls);
      if (entry3 == jsons_[trigger][run].end()) { jsons_[trigger][run].push_back(ls); }
      if (onlyLowestUnprescaledHltPath_) { break; } // Find only lowest unprescaled HLT path?
    }

  }
    
}

////////////////////////////////////////////////////////////////////////////////
//
void MiniAODTriggerAnalyzer::printPathsAndObjects(const edm::Handle<edm::TriggerResults>& triggerResultsHandle,
						  const edm::Handle<pat::PackedTriggerPrescales>& triggerPrescalesHandle,
						  const edm::Handle<std::vector<pat::TriggerObjectStandAlone> >& triggerObjectsHandle,
						  const edm::TriggerNames& triggerNames) {

  std::cout << "\n == TRIGGER PATHS= " << std::endl;
  for (unsigned int i = 0, n = triggerResultsHandle->size(); i < n; ++i) {
    std::cout << "Trigger " << triggerNames.triggerName(i) <<
      ", prescale " << triggerPrescalesHandle->getPrescaleForIndex(i) <<
      ": " << (triggerResultsHandle->accept(i) ? "PASS" : "fail (or not run)")
	      << std::endl;
  }
  
  std::cout << "\n TRIGGER OBJECTS " << std::endl;
  for (pat::TriggerObjectStandAlone obj : *triggerObjectsHandle) { // note: not "const &" since we want to call unpackPathNames
    obj.unpackPathNames(triggerNames);
    std::cout << "\tTrigger object:  pt " << obj.pt() << ", eta " << obj.eta() << ", phi " << obj.phi() << std::endl;
    // Print trigger object collection and type
    std::cout << "\t   Collection: " << obj.collection() << std::endl;
    std::cout << "\t   Type IDs:   ";
    for (unsigned h = 0; h < obj.filterIds().size(); ++h) std::cout << " " << obj.filterIds()[h] ;
    std::cout << std::endl;
    // Print associated trigger filters
    std::cout << "\t   Filters:    ";
    for (unsigned h = 0; h < obj.filterLabels().size(); ++h) std::cout << " " << obj.filterLabels()[h];
    std::cout << std::endl;
    std::vector pathNamesAll = obj.pathNames(false);
    std::vector pathNamesLast = obj.pathNames(true);
    // Print all trigger paths, for each one record also if the object is associated to a 'l3' filter (always true for the
    // definition used in the PAT trigger producer) and if it's associated to the last filter of a successfull path (which
    // means that this object did cause this trigger to succeed; however, it doesn't work on some multi-object triggers)
    std::cout << "\t   Paths (" << pathNamesAll.size()<<"/"<<pathNamesLast.size()<<"):    ";
    for (unsigned h = 0, n = pathNamesAll.size(); h < n; ++h) {
      bool isBoth = obj.hasPathName( pathNamesAll[h], true, true );
      bool isL3   = obj.hasPathName( pathNamesAll[h], false, true );
      bool isLF   = obj.hasPathName( pathNamesAll[h], true, false );
      bool isNone = obj.hasPathName( pathNamesAll[h], false, false );
      std::cout << "   " << pathNamesAll[h];
      if (isBoth) std::cout << "(L,3)";
      if (isL3 && !isBoth) std::cout << "(*,3)";
      if (isLF && !isBoth) std::cout << "(L,*)";
      if (isNone && !isBoth && !isL3 && !isLF) std::cout << "(*,*)";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//
DEFINE_FWK_MODULE(MiniAODTriggerAnalyzer);
