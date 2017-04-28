#include "RecoBTag/CSVscikit/interface/CSVscikitTagger.h"

#include "DataFormats/BTauReco/interface/CandSoftLeptonTagInfo.h"
#include "DataFormats/BTauReco/interface/CandIPTagInfo.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/DataRecord/interface/GBRWrapperRcd.h"


//#include <ext/functional>
//#include "DataFormats/BTauReco/interface/TaggingVariable.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>

const bool PbPbdebug = false;


CSVscikitTagger::CSVscikitTagger(const edm::ParameterSet & configuration):
	sl_computer_(configuration.getParameter<edm::ParameterSet>("slComputerCfg")),
	sv_computer_(configuration.getParameter<edm::ParameterSet>("slComputerCfg")),
  mva_name_( configuration.getParameter<std::string >("mvaName") ),
  use_condDB_(configuration.getParameter<bool>("useCondDB")),
  gbrForest_label_(configuration.getParameter<std::string>("gbrForestLabel")),
  weight_file_(configuration.getParameter<edm::FileInPath>("weightFile")),
  use_GBRForest_(configuration.getParameter<bool>("useGBRForest")),
  use_adaBoost_(configuration.getParameter<bool>("useAdaBoost"))
{
	vpset vars_definition = configuration.getParameter<vpset>("variables");

	for(auto &var : vars_definition) {
		MVAVar mva_var;
		mva_var.name = var.getParameter<std::string>("name");
		mva_var.id = reco::getTaggingVariableName(
			var.getParameter<std::string>("taggingVarName")
			);
		//if (mva_var.id == btau::lastTaggingVariable) {
		//if (mva_var.id == 1) {
		
		//}
		
		
		
		mva_var.has_index = var.existsAs<int>("idx") ;
		mva_var.index = mva_var.has_index ? var.getParameter<int>("idx") : 0;
		mva_var.default_value = var.getParameter<double>("default");

		if (PbPbdebug) std::cout << "CSVscikit:mva_var.id =" << mva_var.id 
					 << " var.getParameter(taggingVarName)=" 
					 <<  var.getParameter<std::string>("taggingVarName") <<" index = "<<mva_var.index << std::endl;


		variables_.push_back(mva_var);
	}

	uses(0, "akPu4PFImpactParameterTagInfos");
	uses(1, "akPu4PFSecondaryVertexTagInfos");

	//uses(0, "pfImpactParameterTagInfos");
	//uses(1, "pfInclusiveSecondaryVertexFinderCvsLTagInfos");
	//uses(1, "pfInclusiveSecondaryVertexFinderTagInfos");
	//uses(2, "softPFMuonsTagInfos");
	//uses(3, "softPFElectronsTagInfos");
}

void CSVscikitTagger::initialize(const JetTagComputerRecord & record)
{
	mvaID_.reset(new TMVAEvaluator());

	std::vector<std::string> variable_names;
	variable_names.reserve(variables_.size());

	for(auto &var : variables_) {
		variable_names.push_back(var.name);
	}
	std::vector<std::string> spectators;

  if(use_condDB_) {
		const GBRWrapperRcd & gbrWrapperRecord = record.getRecord<GBRWrapperRcd>();

		edm::ESHandle<GBRForest> gbrForestHandle;
		gbrWrapperRecord.get(gbrForest_label_.c_str(), gbrForestHandle);

		mvaID_->initializeGBRForest(
			gbrForestHandle.product(), variable_names, 
			spectators, use_adaBoost_
			);
  }
  else {
    mvaID_->initialize(
			"Color:Silent:Error", mva_name_.c_str(),
			weight_file_.fullPath(), variable_names, 
			spectators, use_GBRForest_, use_adaBoost_
			);
  }
}

CSVscikitTagger::~CSVscikitTagger()
{
}

/// b-tag a jet based on track-to-jet parameters in the extened info collection
float CSVscikitTagger::discriminator(const TagInfoHelper & tagInfo) const {
  // default value, used if there are no leptons associated to this jet
  const reco::TrackIPTagInfo & ip_info = tagInfo.get<reco::TrackIPTagInfo>(0);
	const reco::SecondaryVertexTagInfo & sv_info = tagInfo.get<reco::SecondaryVertexTagInfo>(1);
	//const reco::CandIPTagInfo & ip_info = tagInfo.get<reco::CandIPTagInfo>(0);
	//  std::cout<<2<<std::endl;
	//	const reco::CandSecondaryVertexTagInfo & sv_info = tagInfo.get<reco::CandSecondaryVertexTagInfo>(1);
	//const reco::CandSoftLeptonTagInfo& softmu_info = tagInfo.get<reco::CandSoftLeptonTagInfo>(2);
	//const reco::CandSoftLeptonTagInfo& softel_info = tagInfo.get<reco::CandSoftLeptonTagInfo>(3);
	//reco::TaggingVariableList vars = sl_computer_(ip_info, sv_info, softmu_info, softel_info);
	reco::TaggingVariableList vars = sv_computer_(ip_info, sv_info);

	
	//for (auto x:vars)
	//	  std::cout<<x.first<<" - "<<x.second<<std::endl;

	// Loop over input variables
	std::map<std::string, float> inputs;
	//std::cout << "jetvars->";
	
	//For debugging;
	float save_pt_value = -1.0;
	float save_eta_value = -999.0;
	bool passes_cuts = false;
	
	bool notTaggable = false;
	
	bool printdebug = false;
	
	int jetntracks = vars.getList(reco::btau::trackSip2dSig,false).size();

	for(auto &mva_var : variables_){
		//vectorial tagging variable
		if(mva_var.has_index){
			std::vector<float> vals = vars.getList(mva_var.id, false);
			inputs[mva_var.name] = (vals.size() > mva_var.index) ? vals[mva_var.index] : mva_var.default_value;
			////std::cout << " varVV[" << mva_var.name << "]=" << inputs[mva_var.name];
			
			////std::cout << " varVV[" << mva_var.name << "]";
			
			//at least _0 and _1 should exist
			if (mva_var.name == "TagVarCSV_trackSip3dSig_1" && inputs[mva_var.name] > -99.001 && inputs[mva_var.name] < -98.999) notTaggable = true;
			
			if (passes_cuts) {
			  if (printdebug) std::cout << inputs[mva_var.name] << "\t";
			}

			if (mva_var.name == "Jet_pt") {
			  //std::cout << "foundJet_pt ";
			  save_pt_value = inputs[mva_var.name];
			}

			if (mva_var.name == "Jet_eta") {
			  //std::cout << "foundJet_eta ";
			  save_eta_value = inputs[mva_var.name];
			  passes_cuts = (save_pt_value > 30 && save_eta_value > -2.4 && save_eta_value < 2.4);
			  //passes_cuts = true;
			  
			  if (printdebug) {if (passes_cuts) std::cout << save_pt_value << "\t" << save_eta_value << "\t";}
			}
			
			
			
			
		}
		//single value tagging var
		else {
			inputs[mva_var.name] = vars.get(mva_var.id, mva_var.default_value);
			////std::cout << " varSV[" << mva_var.name << "]=" << inputs[mva_var.name];
			
			////std::cout << " varSV[" << mva_var.name << "]";
			
			if (mva_var.name == "TagVarCSV_jetNTracks") inputs[mva_var.name] = jetntracks;

			if (passes_cuts) {
			  //if (mva_var.name == "TagVarCSV_jetNTracks") std::cout << save_pt_value << "\t" << save_eta_value << "\t" << inputs[mva_var.name] << "\t";
			  //else std::cout << inputs[mva_var.name] << "\t";
			  
			  if (printdebug) std::cout << inputs[mva_var.name] << "\t";
			
			}
			
			//if (mva_var.name == "TagVarCSV_jetNTracks" && inputs[mva_var.name] < 0) notTaggable = true;
			if (mva_var.name == "TagVarCSV_jetNTracks" && inputs[mva_var.name] < 2) notTaggable = true;
			//!!!!!!!
			//if (mva_var.name == "TagVarCSV_jetNTracks") inputs[mva_var.name] = 4;
		}
		
	}

	//get the MVA output
	float tag = (mvaID_->evaluate(inputs)+1)/2.;
	////std::cout << "---> tag = " << tag << std::endl;
        //std::cout << tag <<"\n";
	if (printdebug) {if (passes_cuts) std::cout << tag <<"\n";}
	
	if (notTaggable) {
	  tag = -1;
	  if (PbPbdebug) std::cout<<" --- jet not taggable"<<std::endl;
	  
	}

	if (PbPbdebug) {
	  std::cout<<"Looking at a jet of "<<save_pt_value<<" GeV"<<std::endl;
	  for (auto x:inputs)
	    std::cout<<"Variable = "<<x.first<<" value = "<<x.second<<std::endl;
	  std::cout<<"  ---  Result : "<<tag<<std::endl;
	}
	
	//std::cout <<"\n";
	
	return tag;

	//if (notTaggable) return -1.0;
	//else return tag;
}
