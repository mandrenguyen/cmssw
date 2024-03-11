import FWCore.ParameterSet.Config as cms

from RecoBTag.FeatureTools.pfDeepFlavourTagInfos_cfi import pfDeepFlavourTagInfos as pfDeepFlavourTagInfosDeepFlavour
from RecoBTag.ONNXRuntime.pfDeepFlavourJetTags_cfi import pfDeepFlavourJetTags as pfDeepFlavourJetTagsDeepFlavour

pfDeepFlavourJetTagsDeepFlavour.src = 'pfDeepFlavourTagInfosDeepFlavour'

pfDeepFlavourTagInfosDeepFlavour.jets = cms.InputTag("updatedPatJetsDeepFlavour")
#pfDeepFlavourTagInfosDeepFlavour.jets = cms.InputTag("slimmedJets")
#secondary_vertices = cms.InputTag("inclusiveCandidateSecondaryVertices"),
#shallow_tag_infos = cms.InputTag("pfDeepCSVTagInfos"),
#vertex_associator = cms.InputTag("primaryVertexAssociation","original"),
pfDeepFlavourTagInfosDeepFlavour.vertices = cms.InputTag("offlineSlimmedPrimaryVerticesRecovery")
pfDeepFlavourTagInfosDeepFlavour.shallow_tag_infos = "pfDeepCSVTagInfosDeepFlavour"
