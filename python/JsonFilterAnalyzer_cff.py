import FWCore.ParameterSet.Config as cms

files = [
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_10p5_HLT_5p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_10p5_HLT_6p5_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_11p0_HLT_6p5_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_5p5_HLT_4p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_5p5_HLT_6p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_6p0_HLT_4p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_6p5_HLT_4p5_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_7p0_HLT_5p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_8p0_HLT_5p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/L1_8p5_HLT_5p0_final.json",
    "/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/JSON/TOTAL.json",
]

JsonFilterAnalyzer = cms.EDAnalyzer(
    "JsonFilterAnalyzer",
    Verbose = cms.int32(0),
    JsonFiles = cms.vstring(files),
)

jsonFilterSequence = cms.Sequence(
    JsonFilterAnalyzer
)
