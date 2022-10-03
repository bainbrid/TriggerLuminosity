import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
#process.load('Configuration.Geometry.GeometryRecoDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = cms.string('124X_dataRun3_Prompt_v4')

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring('root://cms-xrd-global.cern.ch//store/data/Run2022C/ParkingDoubleElectronLowMass0/MINIAOD/PromptReco-v1/000/356/170/00000/45c0f2ed-eb5b-4292-abc8-3117424d9432.root')
)

process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))
process.source.skipEvents=cms.untracked.uint32(0)

process.load("bainbrid.TriggerLuminosity.MiniAODTriggerAnalyzer_cff")
process.p = cms.Path(process.miniAODTriggerSequence)
