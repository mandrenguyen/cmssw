import FWCore.ParameterSet.Config as cms

hiFJRhoAnalyzer = cms.EDAnalyzer(
    'HiFJRhoAnalyzer',
    etaMap        = cms.InputTag('hiFJRhoProducer','mapEtaEdges','HiForest'),
    rho           = cms.InputTag('hiFJRhoProducer','mapToRho'),
    rhom          = cms.InputTag('hiFJRhoProducer','mapToRhoM'),
    rhoCorr       = cms.InputTag('hiFJGridEmptyAreaCalculator','mapToRhoCorr'),
    rhomCorr      = cms.InputTag('hiFJGridEmptyAreaCalculator','mapToRhoMCorr'),
    rhoCorr1Bin   = cms.InputTag('hiFJGridEmptyAreaCalculator','mapToRhoCorr1Bin'),
    rhomCorr1Bin  = cms.InputTag('hiFJGridEmptyAreaCalculator','mapToRhoMCorr1Bin'),
    rhoGrid       = cms.InputTag('hiFJGridEmptyAreaCalculator','mapRhoVsEtaGrid'),
    meanRhoGrid   = cms.InputTag('hiFJGridEmptyAreaCalculator','mapMeanRhoVsEtaGrid'),
    etaMaxRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapEtaMaxGrid'),
    etaMinRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculator','mapEtaMinGrid'),
    ptJets        = cms.InputTag('hiFJRhoProducer','ptJets'),
    etaJets       = cms.InputTag('hiFJRhoProducer','etaJets'),
    areaJets      = cms.InputTag('hiFJRhoProducer','areaJets'),
)

hiFJRhoAnalyzerFinerBins = cms.EDAnalyzer(
    'HiFJRhoAnalyzer',
    etaMap        = cms.InputTag('hiFJRhoProducerFinerBins','mapEtaEdges','HiForest'),
    rho           = cms.InputTag('hiFJRhoProducerFinerBins','mapToRho'),
    rhom          = cms.InputTag('hiFJRhoProducerFinerBins','mapToRhoM'),
    rhoCorr       = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapToRhoCorr'),
    rhomCorr      = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapToRhoMCorr'),
    rhoCorr1Bin   = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapToRhoCorr1Bin'),
    rhomCorr1Bin  = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapToRhoMCorr1Bin'),
    rhoGrid       = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapRhoVsEtaGrid'),
    meanRhoGrid   = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapMeanRhoVsEtaGrid'),
    etaMaxRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapEtaMaxGrid'),
    etaMinRhoGrid = cms.InputTag('hiFJGridEmptyAreaCalculatorFinerBins','mapEtaMinGrid'),
    ptJets        = cms.InputTag('hiFJRhoProducerFinerBins','ptJets'),
    etaJets       = cms.InputTag('hiFJRhoProducerFinerBins','etaJets'),
    areaJets      = cms.InputTag('hiFJRhoProducerFinerBins','areaJets'),
)
