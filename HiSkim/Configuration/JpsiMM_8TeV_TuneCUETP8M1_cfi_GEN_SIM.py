# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: JpsiMM_8TeV_TuneCUETP8M1_cfi --conditions auto:run1_mc -n 10 --eventcontent RAWSIM --relval 66000,1000 -s GEN,SIM --datatier GEN-SIM --beamspot Realistic8TeVCollision
import FWCore.ParameterSet.Config as cms

process = cms.Process('SIM')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.Geometry.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic8TeVCollision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('JpsiMM_8TeV_TuneCUETP8M1_cfi nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('JpsiMM_8TeV_TuneCUETP8M1_cfi_GEN_SIM.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run1_mc', '')

process.oniafilter = cms.EDFilter("PythiaFilter",
    MaxEta = cms.untracked.double(1000.0),
    MinEta = cms.untracked.double(-1000.0),
    MinPt = cms.untracked.double(0.0),
    ParticleID = cms.untracked.int32(443),
    Status = cms.untracked.int32(2)
)


process.mugenfilter = cms.EDFilter("MCSingleParticleFilter",
    MinPt = cms.untracked.vdouble(10.0, 10.0),
    ParticleID = cms.untracked.vint32(13, -13),
    Status = cms.untracked.vint32(1, 1)
)


process.generator = cms.EDFilter("Pythia8GeneratorFilter",
    PythiaParameters = cms.PSet(
        parameterSets = cms.vstring('processParameters', 
            'pythia8CommonSettings', 
            'pythia8CUEP8M1Settings'),
        processParameters = cms.vstring('Charmonium:states(3S1) = 443 ', 
            'Charmonium:O(3S1)[3S1(1)] = 1.16', 
            'Charmonium:O(3S1)[3S1(8)] = 0.0119', 
            'Charmonium:O(3S1)[1S0(8)] = 0.01', 
            'Charmonium:O(3S1)[3P0(8)] = 0.01', 
            'Charmonium:gg2ccbar(3S1)[3S1(1)]g = on', 
            'Charmonium:gg2ccbar(3S1)[3S1(8)]g = on', 
            'Charmonium:qg2ccbar(3S1)[3S1(8)]q = on', 
            'Charmonium:qqbar2ccbar(3S1)[3S1(8)]g = on', 
            'Charmonium:gg2ccbar(3S1)[1S0(8)]g = on', 
            'Charmonium:qg2ccbar(3S1)[1S0(8)]q = on', 
            'Charmonium:qqbar2ccbar(3S1)[1S0(8)]g = on', 
            'Charmonium:gg2ccbar(3S1)[3PJ(8)]g = on', 
            'Charmonium:qg2ccbar(3S1)[3PJ(8)]q = on', 
            'Charmonium:qqbar2ccbar(3S1)[3PJ(8)]g = on', 
            '443:onMode = off', 
            '443:onIfAny = 13', 
            'PhaseSpace:pTHatMin = 10.'),
        pythia8CUEP8M1Settings = cms.vstring('Tune:pp 14', 
            'Tune:ee 7', 
            'MultipartonInteractions:pT0Ref=2.4024', 
            'MultipartonInteractions:ecmPow=0.25208', 
            'MultipartonInteractions:expPow=1.6'),
        pythia8CommonSettings = cms.vstring('Tune:preferLHAPDF = 2', 
            'Main:timesAllowErrors = 10000', 
            'Check:epTolErr = 0.01', 
            'Beams:setProductionScalesFromLHEF = off', 
            'SLHA:keepSM = on', 
            'SLHA:minMassSM = 1000.', 
            'ParticleDecays:limitTau0 = on', 
            'ParticleDecays:tau0Max = 10', 
            'ParticleDecays:allowPhotonRadiation = on')
    ),
    comEnergy = cms.double(8000.0),
    crossSection = cms.untracked.double(1256000.0),
    filterEfficiency = cms.untracked.double(0.138),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    pythiaPylistVerbosity = cms.untracked.int32(0)
)


process.mumugenfilter = cms.EDFilter("MCParticlePairFilter",
    MaxEta = cms.untracked.vdouble(2.5, 2.5),
    MinEta = cms.untracked.vdouble(-2.5, -2.5),
    MinP = cms.untracked.vdouble(2.7, 2.7),
    MinPt = cms.untracked.vdouble(0.5, 0.5),
    ParticleCharge = cms.untracked.int32(-1),
    ParticleID1 = cms.untracked.vint32(13),
    ParticleID2 = cms.untracked.vint32(13),
    Status = cms.untracked.vint32(1, 1)
)


process.ProductionFilterSequence = cms.Sequence(process.generator+process.oniafilter+process.mumugenfilter+process.mugenfilter)

# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.simulation_step,process.endjob_step,process.RAWSIMoutput_step)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq 


