To setup Bfinder
=====

Ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiForestSetup#Setup_for_9_4_10_For_2017_pp_ReR

Branch for CMSSW_9XX Recommended using 
* `CMSSW_9_4_10` for data (re-reco)
* `CMSSW_9_4_11` for MC (official)

```
cmsrel CMSSW_9_4_10 # replace CMSSW_9_4_10 with the proper release
cd CMSSW_9_4_10/src
cmsenv
git cms-merge-topic -u CmsHI:forest_CMSSW_9_4_10 # forest_CMSSW_9_4_10 regardless of release
cd HeavyIonsAnalysis/JetAnalysis/python/jets
./makeJetSequences.sh
cd ../../../..
scram b -j4
```

To add D/Bfinder to forest:
=====

```
cd $CMSSW_BASE/src
cmsenv
git clone -b Dfinder_9XX https://github.com/boundino/Bfinder.git --depth 1
source Bfinder/test/DnBfinder_to_Forest_94X.sh
scram b -j4
# Dfinder MC:
mkdir -p dfinder && cp HeavyIonsAnalysis/JetAnalysis/test/runForestAOD_pp_MC_94X_onlyDfinder.py dfinder/runForestAOD_pp_MC_94X_onlyDfinder.py
# Dfinder data:
mkdir -p dfinder && cp HeavyIonsAnalysis/JetAnalysis/test/runForestAOD_pp_DATA_94X_onlyDfinder.py dfinder/runForestAOD_pp_DATA_94X_onlyDfinder.py
# Bfinder MC:
mkdir -p bfinder && cp HeavyIonsAnalysis/JetAnalysis/test/runForestAOD_pp_MC_94X_onlyBfinder.py bfinder/runForestAOD_pp_MC_94X_onlyBfinder.py
# Bfinder data:
mkdir -p bfinder && cp HeavyIonsAnalysis/JetAnalysis/test/runForestAOD_pp_DATA_94X_onlyBfinder.py bfinder/runForestAOD_pp_DATA_94X_onlyBfinder.py
cd bfinder/
```

To run:
=====

* MC:
```
cmsRun runForestAOD_pp_MC_94X_onlyDfinder.py
cmsRun runForestAOD_pp_MC_94X_onlyBfinder.py
```
* data:
```
cmsRun runForestAOD_pp_DATA_94X_onlyDfinder.py
cmsRun runForestAOD_pp_DATA_94X_onlyBfinder.py
```
