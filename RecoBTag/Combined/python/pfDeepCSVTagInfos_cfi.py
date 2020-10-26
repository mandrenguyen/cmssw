import FWCore.ParameterSet.Config as cms
from RecoBTag.SecondaryVertex.combinedSecondaryVertexCommon_cff import combinedSecondaryVertexCommon

pfDeepCSVTagInfos = cms.EDProducer(
	'DeepNNTagInfoProducer',
	svTagInfos = cms.InputTag('pfInclusiveSecondaryVertexFinderTagInfos'),
	computer = combinedSecondaryVertexCommon
	)

from Configuration.ProcessModifiers.run2_miniAOD_pp_on_AA_103X_cff import run2_miniAOD_pp_on_AA_103X
run2_miniAOD_pp_on_AA_103X.toModify(pfDeepCSVTagInfos, svTagInfos = 'pfSecondaryVertexTagInfos' )
