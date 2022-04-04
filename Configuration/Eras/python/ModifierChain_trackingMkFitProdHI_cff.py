import FWCore.ParameterSet.Config as cms

# to replace CKF with MkFit in select iterations
from Configuration.ProcessModifiers.trackingMkFitCommon_cff import *
from Configuration.ProcessModifiers.trackingMkFitInitialStepPreSplitting_cff import *
from Configuration.ProcessModifiers.trackingMkFitInitialStepHI_cff import *
from Configuration.ProcessModifiers.trackingMkFitHighPtTripletStepHI_cff import *
from Configuration.ProcessModifiers.trackingMkFitDetachedQuadStepHI_cff import *
from Configuration.ProcessModifiers.trackingMkFitDetachedTripletStepHI_cff import *
from Configuration.ProcessModifiers.trackingMkFitPixelLessStepHI_cff import *

trackingMkFitProdHI =  cms.ModifierChain(
    trackingMkFitCommon,
    trackingMkFitInitialStepPreSplitting,
    trackingMkFitInitialStepHI,
    trackingMkFitHighPtTripletStepHI,
    trackingMkFitDetachedQuadStepHI,
    trackingMkFitDetachedTripletStepHI,
    trackingMkFitPixelLessStepHI,
)
