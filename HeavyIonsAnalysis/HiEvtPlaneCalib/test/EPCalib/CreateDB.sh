#!/bin/bash
rm tmp.lis
ls -1  /rfs/sanders/crab_projects/crab_PbPb2015_HIMinimumBias2/* > tmp.lis
ls -1  /rfs/sanders/crab_projects/crab_PbPb2015_HIMinimumBias3/* >> tmp.lis
#ls -1  /rfs/sanders/crab_projects/crab_PbPb2015_HIMinimumBias4/* >> tmp.lis

cd EPCalib
rm *.so
rm *.d
rm *.pcm
rm -rf HiEvtPlaneList.h
rm -rf HiEvtPlaneFlatten.h
ln -s $CMSSW_BASE/src/RecoHI/HiEvtPlaneAlgos/interface/HiEvtPlaneList.h
ln -s $CMSSW_BASE/src/RecoHI/HiEvtPlaneAlgos/interface/HiEvtPlaneFlatten.h
cd ..
rm -rf data/*.root
rm -rf RescorTables
rm *.db
rm tmpsave
rm EP.root
mkdir RescorTables
root -l -b -q "EPCalib/EPCalib.C+(1,262799)"
cp EP.root save/EP_1_262799.root
cd data
ln -s ../EP.root rpflat_combined.root
cd ..
cmsRun moveflatparamstodb_cfg.py print outputFile=HeavyIonRPRcd_gap_1_262799.db outputTag=HeavyIonRPRcd begin=1 end=262799
rm tmpsave

rm -rf data/*.root
rm EP.root
rm -rf RescorTables
mkdir RescorTables
root -l -b -q "EPCalib/EPCalib.C+(262800,263230)"
cp EP.root save/EP_262800_263230.root
cd data
ln -s ../EP.root rpflat_combined.root
cd ..
cmsRun moveflatparamstodb_cfg.py print outputFile=HeavyIonRPRcd_gap_262800_263230.db outputTag=HeavyIonRPRcd begin=262800 end=263230
rm tmpsave

rm -rf data/*.root
rm EP.root
rm -rf RescorTables
mkdir RescorTables
root -l -b -q "EPCalib/EPCalib.C+(263231,263359)"
cp EP.root save/EP_263231_263359.root
cd data
ln -s ../EP.root rpflat_combined.root
cd ..
cmsRun moveflatparamstodb_cfg.py print outputFile=HeavyIonRPRcd_gap_263231_263359.db outputTag=HeavyIonRPRcd begin=263231 end=263359
rm  tmpsave

rm -rf data/*.root
rm EP.root
rm -rf RescorTables
mkdir RescorTables
root -l -b -q "EPCalib/EPCalib.C+(263360,263379)"
cp EP.root save/EP_263360_263379.root
cd data
ln -s ../EP.root rpflat_combined.root
cd ..
cmsRun moveflatparamstodb_cfg.py print outputFile=HeavyIonRPRcd_gap_263360_263379.db outputTag=HeavyIonRPRcd begin=263360 end=263379
rm tmpsave

rm -rf data/*.root
rm EP.root
rm -rf RescorTables
mkdir RescorTables
root -l -b -q "EPCalib/EPCalib.C+(263380,263614)"
cp EP.root save/EP_263380_263614.root
cd data
ln -s ../EP.root rpflat_combined.root
cd ..
cmsRun moveflatparamstodb_cfg.py print outputFile=HeavyIonRPRcd_gap_263380_263614.db outputTag=HeavyIonRPRcd begin=263380 end=263614
rm tmpsave

conddb_import -f sqlite_file:HeavyIonRPRcd_gap_1_262799.db -c sqlite_file:HeavyIonRPRcd_gap_v2and3_offline.db -i HeavyIonRPRcd -t HeavyIonRPRcd_gap_v2and3_offline -b 1 -e 262799
conddb_import -f sqlite_file:HeavyIonRPRcd_gap_262800_263230.db -c sqlite_file:HeavyIonRPRcd_gap_v2and3_offline.db -i HeavyIonRPRcd -t HeavyIonRPRcd_gap_v2and3_offline -b 262800 -e 263230
conddb_import -f sqlite_file:HeavyIonRPRcd_gap_263231_263359.db -c sqlite_file:HeavyIonRPRcd_gap_v2and3_offline.db -i HeavyIonRPRcd -t HeavyIonRPRcd_gap_v2and3_offline -b 263231 -e 263359
conddb_import -f sqlite_file:HeavyIonRPRcd_gap_263360_263379.db -c sqlite_file:HeavyIonRPRcd_gap_v2and3_offline.db -i HeavyIonRPRcd -t HeavyIonRPRcd_gap_v2and3_offline -b 263360 -e 263379
conddb_import -f sqlite_file:HeavyIonRPRcd_gap_263380_263614.db -c sqlite_file:HeavyIonRPRcd_gap_v2and3_offline.db -i HeavyIonRPRcd -t HeavyIonRPRcd_gap_v2and3_offline -b 263380 -e 263614



