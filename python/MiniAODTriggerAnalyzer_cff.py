import FWCore.ParameterSet.Config as cms

unpackedPatTrigger = cms.EDProducer(
    "PATTriggerObjectStandAloneUnpacker",
    patTriggerObjectsStandAlone = cms.InputTag('slimmedPatTrigger'),
    triggerResults              = cms.InputTag('TriggerResults::HLT'),
    unpackFilterLabels = cms.bool(True)
)

# Order is important !!!
seeds = [
    'L1_DoubleEG4_er1p2_dR_Max0p9',
    'L1_DoubleEG4p5_er1p2_dR_Max0p9',
    'L1_DoubleEG5_er1p2_dR_Max0p9',
    'L1_DoubleEG5p5_er1p2_dR_Max0p8',
    'L1_DoubleEG6_er1p2_dR_Max0p8',
    'L1_DoubleEG6p5_er1p2_dR_Max0p8',
    'L1_DoubleEG7_er1p2_dR_Max0p8',
    'L1_DoubleEG7p5_er1p2_dR_Max0p7',
    'L1_DoubleEG8_er1p2_dR_Max0p7',
    'L1_DoubleEG8p5_er1p2_dR_Max0p7',
    'L1_DoubleEG9_er1p2_dR_Max0p7',
    'L1_DoubleEG9p5_er1p2_dR_Max0p6',
    'L1_DoubleEG10_er1p2_dR_Max0p6',
    'L1_DoubleEG10p5_er1p2_dR_Max0p6',
    'L1_DoubleEG11_er1p2_dR_Max0p6',
]

# Order is important !!!
paths=[
    'HLT_DoubleEle4_eta1p22_mMax6',
    'HLT_DoubleEle4p5_eta1p22_mMax6',
    'HLT_DoubleEle5_eta1p22_mMax6',
    'HLT_DoubleEle5p5_eta1p22_mMax6',
    'HLT_DoubleEle6_eta1p22_mMax6',
    'HLT_DoubleEle6p5_eta1p22_mMax6',
    'HLT_DoubleEle7_eta1p22_mMax6',
    'HLT_DoubleEle7p5_eta1p22_mMax6',
    'HLT_DoubleEle8_eta1p22_mMax6',
    'HLT_DoubleEle8p5_eta1p22_mMax6',
    'HLT_DoubleEle9_eta1p22_mMax6',
    'HLT_DoubleEle9p5_eta1p22_mMax6',
    'HLT_DoubleEle10_eta1p22_mMax6',
]

miniAODTriggerAnalyzer = cms.EDAnalyzer(
    "MiniAODTriggerAnalyzer",
    HLTProcess = cms.string('HLT'),
    HLTPaths = cms.vstring(paths),
    L1Seeds = cms.vstring(seeds),
    cfg = cms.PSet(
        stageL1Trigger = cms.uint32(2),
        l1tAlgBlkInputTag = cms.InputTag('gtStage2Digis'),
        l1tExtBlkInputTag = cms.InputTag('gtStage2Digis')
    ),
    bits = cms.InputTag("TriggerResults","","HLT"),
    prescales = cms.InputTag("patTrigger"),
    objects = cms.InputTag("unpackedPatTrigger"),
#    ugtToken = cms.InputTag("gtStage2Digis"),
#    stageL1Trigger = cms.uint32(2),
    Verbose = cms.int32(1),
    OnlyLowestUnprescaledHltPath = cms.bool(True),
)

miniAODTriggerSequence = cms.Sequence(
    unpackedPatTrigger+
    miniAODTriggerAnalyzer
)
