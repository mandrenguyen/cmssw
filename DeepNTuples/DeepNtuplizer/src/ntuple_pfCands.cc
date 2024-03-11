/*
 * ntuple_pfCands.cc
 *
 *  Created on: 13 Feb 2017
 *      Author: jkiesele
 */


#include "../interface/ntuple_pfCands.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "../interface/sorting_modules.h"


#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "TVector3.h"

class TrackInfoBuilder{
public:
    TrackInfoBuilder(edm::ESHandle<TransientTrackBuilder> & build):
        builder(build),
        trackMomentum_(0),
        trackEta_(0),
        trackEtaRel_(0),
        trackPtRel_(0),
        trackPPar_(0),
        trackDeltaR_(0),
        trackPtRatio_(0),
        trackPParRatio_(0),
        trackSip2dVal_(0),
        trackSip2dSig_(0),
        trackSip3dVal_(0),
        trackSip3dSig_(0),

        trackJetDistVal_(0),
        trackJetDistSig_(0)
{


}

    void buildTrackInfo(const pat::PackedCandidate* PackedCandidate_ ,const math::XYZVector&  jetDir, GlobalVector refjetdirection, const reco::Vertex & pv){
			TVector3 jetDir3(jetDir.x(),jetDir.y(),jetDir.z());
			if(!PackedCandidate_->hasTrackDetails()) {
				TVector3 trackMom3(
					PackedCandidate_->momentum().x(),
					PackedCandidate_->momentum().y(),
					PackedCandidate_->momentum().z()
					);
				trackMomentum_=PackedCandidate_->p();
				trackEta_= PackedCandidate_->eta();
				trackEtaRel_=reco::btau::etaRel(jetDir, PackedCandidate_->momentum());
				trackPtRel_=trackMom3.Perp(jetDir3);
				trackPPar_=jetDir.Dot(PackedCandidate_->momentum());
				trackDeltaR_=reco::deltaR(PackedCandidate_->momentum(), jetDir);
				trackPtRatio_=trackMom3.Perp(jetDir3) / PackedCandidate_->p();
				trackPParRatio_=jetDir.Dot(PackedCandidate_->momentum()) / PackedCandidate_->p();
				trackSip2dVal_=0.;
				trackSip2dSig_=0.;
				trackSip3dVal_=0.;
				trackSip3dSig_=0.;
				trackJetDistVal_=0.;
				trackJetDistSig_=0.;
				return;
			}

        const reco::Track & PseudoTrack =  PackedCandidate_->pseudoTrack();

        reco::TransientTrack transientTrack;
        transientTrack=builder->build(PseudoTrack);
        Measurement1D meas_ip2d=IPTools::signedTransverseImpactParameter(transientTrack, refjetdirection, pv).second;
        Measurement1D meas_ip3d=IPTools::signedImpactParameter3D(transientTrack, refjetdirection, pv).second;
        Measurement1D jetdist=IPTools::jetTrackDistance(transientTrack, refjetdirection, pv).second;
        math::XYZVector trackMom = PseudoTrack.momentum();
        double trackMag = std::sqrt(trackMom.Mag2());
        TVector3 trackMom3(trackMom.x(),trackMom.y(),trackMom.z());


        trackMomentum_=std::sqrt(trackMom.Mag2());
        trackEta_= trackMom.Eta();
        trackEtaRel_=reco::btau::etaRel(jetDir, trackMom);
        trackPtRel_=trackMom3.Perp(jetDir3);
        trackPPar_=jetDir.Dot(trackMom);
        trackDeltaR_=reco::deltaR(trackMom, jetDir);
        trackPtRatio_=trackMom3.Perp(jetDir3) / trackMag;
        trackPParRatio_=jetDir.Dot(trackMom) / trackMag;
        trackSip2dVal_=(meas_ip2d.value());

        trackSip2dSig_=(meas_ip2d.significance());
        trackSip3dVal_=(meas_ip3d.value());


        trackSip3dSig_=meas_ip3d.significance();
        trackJetDistVal_= jetdist.value();
        trackJetDistSig_= jetdist.significance();

    }

    const float& getTrackDeltaR() const {return trackDeltaR_;}
    const float& getTrackEta() const {return trackEta_;}
    const float& getTrackEtaRel() const {return trackEtaRel_;}
    const float& getTrackJetDistSig() const {return trackJetDistSig_;}
    const float& getTrackJetDistVal() const {return trackJetDistVal_;}
    const float& getTrackMomentum() const {return trackMomentum_;}
    const float& getTrackPPar() const {return trackPPar_;}
    const float& getTrackPParRatio() const {return trackPParRatio_;}
    const float& getTrackPtRatio() const {return trackPtRatio_;}
    const float& getTrackPtRel() const {return trackPtRel_;}
    const float& getTrackSip2dSig() const {return trackSip2dSig_;}
    const float& getTrackSip2dVal() const {return trackSip2dVal_;}
    const float& getTrackSip3dSig() const {return trackSip3dSig_;}
    const float& getTrackSip3dVal() const {return trackSip3dVal_;}

private:

    edm::ESHandle<TransientTrackBuilder>& builder;

    float trackMomentum_;
    float trackEta_;
    float trackEtaRel_;
    float trackPtRel_;
    float trackPPar_;
    float trackDeltaR_;
    float trackPtRatio_;
    float trackPParRatio_;
    float trackSip2dVal_;
    float trackSip2dSig_;
    float trackSip3dVal_;
    float trackSip3dSig_;

    float trackJetDistVal_;
    float trackJetDistSig_;

};




void ntuple_pfCands::readSetup(const edm::EventSetup& iSetup){

    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);

}

void ntuple_pfCands::getInput(const edm::ParameterSet& iConfig){
	min_candidate_pt_ = (iConfig.getParameter<double>("minCandidatePt"));
}

void ntuple_pfCands::initBranches(TTree* tree){

    addBranch(tree,"ncand", &ncand_,"ncand_/i");
    addBranch(tree,"pfcand_pt", &pfcand_pt_,"pfcand_pt_[ncand_]/F");
    addBranch(tree,"pfcand_eta", &pfcand_eta_,"pfcand_eta_[ncand_]/F");
    addBranch(tree,"pfcand_phi", &pfcand_phi_,"pfcand_phi_[ncand_]/F");
    addBranch(tree,"pfcand_mass", &pfcand_mass_,"pfcand_mass_[ncand_]/F");
    addBranch(tree,"pfcand_energy", &pfcand_energy_,"pfcand_energy_[ncand_]/F");
    addBranch(tree,"pfcand_calofraction", &pfcand_calofraction_,"pfcand_calofraction_[ncand_]/F");
    addBranch(tree,"pfcand_hcalfraction", &pfcand_hcalfraction_,"pfcand_hcalfraction_[ncand_]/F");
    addBranch(tree,"pfcand_dxy", &pfcand_dxy_,"pfcand_dxy_[ncand_]/F");
    addBranch(tree,"pfcand_dxysig", &pfcand_dxysig_,"pfcand_dxysig_[ncand_]/F");
    addBranch(tree,"pfcand_dz", &pfcand_dz_,"pfcand_dz_[ncand_]/F");
    addBranch(tree,"pfcand_dzsig", &pfcand_dzsig_,"pfcand_dzsig_[ncand_]/F");
    addBranch(tree,"pfcand_pperp_ratio", &pfcand_pperp_ratio_,"pfcand_pperp_ratio_[ncand_]/F");
    addBranch(tree,"pfcand_ppara_ratio", &pfcand_ppara_ratio_,"pfcand_ppara_ratio_[ncand_]/F");
    addBranch(tree,"pfcand_deta", &pfcand_deta_,"pfcand_deta_[ncand_]/F");
    addBranch(tree,"pfcand_dphi", &pfcand_dphi_,"pfcand_dphi_[ncand_]/F");
    addBranch(tree,"pfcand_etarel", &pfcand_etarel_,"pfcand_etarel_[ncand_]/F");
    addBranch(tree,"pfcand_frompv", &pfcand_frompv_,"pfcand_frompv_[ncand_]/i");
    addBranch(tree,"pfcand_id", &pfcand_id_,"pfcand_id_[ncand_]/i");
    addBranch(tree,"pfcand_charge", &pfcand_charge_,"pfcand_charge_[ncand_]/I");
    addBranch(tree,"pfcand_track_qual", &pfcand_track_qual_,"pfcand_track_qual_[ncand_]/i");
    addBranch(tree,"pfcand_track_chi2", &pfcand_track_chi2_,"pfcand_track_chi2_[ncand_]/i");
    addBranch(tree,"pfcand_npixhits", &pfcand_npixhits_,"pfcand_npixhits_[ncand_]/i");
    addBranch(tree,"pfcand_nstriphits", &pfcand_nstriphits_,"pfcand_nstriphits_[ncand_]/i");
    addBranch(tree,"pfcand_nlosthits", &pfcand_nlosthits_,"pfcand_nlosthits_[ncand_]/i");
    addBranch(tree,"pfcand_trackjet_d3d", &pfcand_trackjet_d3d_,"pfcand_trackjet_d3d_[ncand_]/F");
    addBranch(tree,"pfcand_trackjet_d3dsig", &pfcand_trackjet_d3dsig_,"pfcand_trackjet_d3dsig_[ncand_]/F");
    addBranch(tree,"pfcand_trackjet_dist", &pfcand_trackjet_dist_,"pfcand_trackjet_dist_[ncand_]/F");
    addBranch(tree,"pfcand_trackjet_decayL", &pfcand_trackjet_decayL_,"pfcand_trackjet_decayL_[ncand_]/F");


    addBranch(tree,"n_Cpfcand", &n_Cpfcand_,"n_Cpfcand_/i");
    addBranch(tree,"nCpfcand", &nCpfcand_,"nCpfcand_/F");

    //addBranch(tree,"Cpfcan_pt", &Cpfcan_pt_,"Cpfcan_pt_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_eta", &Cpfcan_eta_,"Cpfcan_eta_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_phi", &Cpfcan_phi_,"Cpfcan_phi_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_mass", &Cpfcan_mass_,"Cpfcan_mass_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_energy", &Cpfcan_energy_,"Cpfcan_energy_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_calofraction", &Cpfcan_calofraction_,"Cpfcan_calofraction_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_hcalfraction", &Cpfcan_hcalfraction_,"Cpfcan_hcalfraction_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_ptrel", &Cpfcan_ptrel_,"Cpfcan_ptrel_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_erel", &Cpfcan_erel_,"Cpfcan_erel_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_phirel",&Cpfcan_phirel_,"Cpfcan_phirel_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_etarel",&Cpfcan_etarel_,"Cpfcan_etarel_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_deltaR",&Cpfcan_deltaR_,"Cpfcan_deltaR_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_puppiw",&Cpfcan_puppiw_,"Cpfcan_puppiw_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_dxy",&Cpfcan_dxy_,"Cpfcan_dxy_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_dxyerrinv",&Cpfcan_dxyerrinv_,"Cpfcan_dxyerrinv_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_dxysig",&Cpfcan_dxysig_,"Cpfcan_dxysig_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_dz",&Cpfcan_dz_,"Cpfcan_dz_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_VTX_ass",&Cpfcan_VTX_ass_,"Cpfcan_VTX_ass_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_fromPV",&Cpfcan_fromPV_,"Cpfcan_fromPV_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_drminsv",&Cpfcan_drminsv_,"Cpfcan_drminsv_[n_Cpfcand_]/F");

    //commented ones don't work
    /**///addBranch(tree,"Cpfcan_vertexChi2",&Cpfcan_vertexChi2_,"Cpfcan_vertexChi2_[n_Cpfcand_]/F");
    /**///addBranch(tree,"Cpfcan_vertexNdof",&Cpfcan_vertexNdof_,"Cpfcan_vertexNdof_[n_Cpfcand_]/F");
    /**///addBranch(tree,"Cpfcan_vertexNormalizedChi2",&Cpfcan_vertexNormalizedChi2_,"Cpfcan_vertexNormalizedChi2_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_vertex_rho",&Cpfcan_vertex_rho_,"Cpfcan_vertex_rho_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_vertex_phirel",&Cpfcan_vertex_phirel_,"Cpfcan_vertex_phirel_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_vertex_etarel",&Cpfcan_vertex_etarel_,"Cpfcan_vertex_etarel_[n_Cpfcand_]/F");
    /**///addBranch(tree,"Cpfcan_vertexRef_mass",&Cpfcan_vertexRef_mass_,"Cpfcan_vertexRef_mass_[n_Cpfcand_]/F");

    /*
    addBranch(tree,"Cpfcan_dptdpt",&Cpfcan_dptdpt_,"Cpfcan_dptdpt_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_detadeta",&Cpfcan_detadeta_,"Cpfcan_detadeta_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dphidphi",&Cpfcan_dphidphi_,"Cpfcan_dphidphi_[n_Cpfcand_]/F");


    addBranch(tree,"Cpfcan_dxydxy",&Cpfcan_dxydxy_,"Cpfcan_dxydxy_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dzdz",&Cpfcan_dzdz_,"Cpfcan_dzdz_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dxydz",&Cpfcan_dxydz_,"Cpfcan_dxydz_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dphidxy",&Cpfcan_dphidxy_,"Cpfcan_dphidxy_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_dlambdadz",&Cpfcan_dlambdadz_,"Cpfcan_dlambdadz_[n_Cpfcand_]/F");
     */



    //addBranch(tree,"Cpfcan_BtagPf_trackMomentum",&Cpfcan_BtagPf_trackMomentum_,"Cpfcan_BtagPf_trackMomentum_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_BtagPf_trackEta",&Cpfcan_BtagPf_trackEta_,"Cpfcan_BtagPf_trackEta_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackEtaRel",&Cpfcan_BtagPf_trackEtaRel_,"Cpfcan_BtagPf_trackEtaRel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPtRel",&Cpfcan_BtagPf_trackPtRel_,"Cpfcan_BtagPf_trackPtRel_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPPar",&Cpfcan_BtagPf_trackPPar_,"Cpfcan_BtagPf_trackPPar_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackDeltaR",&Cpfcan_BtagPf_trackDeltaR_,"Cpfcan_BtagPf_trackDeltaR_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_BtagPf_trackPtRatio",&Cpfcan_BtagPf_trackPtRatio_,"Cpfcan_BtagPf_trackPtRatio_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackPParRatio",&Cpfcan_BtagPf_trackPParRatio_,"Cpfcan_BtagPf_trackPParRatio[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip3dVal",&Cpfcan_BtagPf_trackSip3dVal_,"Cpfcan_BtagPf_trackSip3dVal_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip3dSig",&Cpfcan_BtagPf_trackSip3dSig_,"Cpfcan_BtagPf_trackSip3dSig_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip2dVal",&Cpfcan_BtagPf_trackSip2dVal_,"Cpfcan_BtagPf_trackSip2dVal_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackSip2dSig",&Cpfcan_BtagPf_trackSip2dSig_,"Cpfcan_BtagPf_trackSip2dSig_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_BtagPf_trackDecayLen",&Cpfcan_BtagPf_trackDecayLen_,"Cpfcan_BtagPf_trackDecayLen_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_BtagPf_trackJetDistVal",&Cpfcan_BtagPf_trackJetDistVal_,"Cpfcan_BtagPf_trackJetDistVal_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_BtagPf_trackJetDistSig",&Cpfcan_BtagPf_trackJetDistSig_,"Cpfcan_BtagPf_trackJetDistSig_[n_Cpfcand_]/F");




    //addBranch(tree,"Cpfcan_isMu",&Cpfcan_isMu_,"Cpfcan_isMu_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_isEl",&Cpfcan_isEl_,"Cpfcan_isEl_[n_Cpfcand_]/F");

    //in16 conversion broken
    //addBranch(tree,"Cpfcan_lostInnerHits",&Cpfcan_lostInnerHits_,"Cpfcan_lostInnerHits_[n_Cpfcand_]/F");
    //addBranch(tree,"Cpfcan_numberOfPixelHits",&Cpfcan_numberOfPixelHits_,"Cpfcan_numberOfPixelHits_[n_Cpfcand_]/F");

    addBranch(tree,"Cpfcan_chi2",&Cpfcan_chi2_,"Cpfcan_chi2_[n_Cpfcand_]/F");
    addBranch(tree,"Cpfcan_quality",&Cpfcan_quality_,"Cpfcan_quality_[n_Cpfcand_]/F");

    // did not give integers !!
    //  addBranch(tree,"Cpfcan_charge",&Cpfcan_charge_,"Cpfcan_charge_[n_Cpfcand_]/i");

    //Neutral Pf candidates
    addBranch(tree,"n_Npfcand", &n_Npfcand_,"n_Npfcand_/i");
    addBranch(tree,"nNpfcand", &nNpfcand_,"nNpfcand/F");
    //addBranch(tree,"Npfcan_pt", &Npfcan_pt_,"Npfcan_pt_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_eta", &Npfcan_eta_,"Npfcan_eta_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_phi", &Npfcan_phi_,"Npfcan_phi_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_ptrel", &Npfcan_ptrel_,"Npfcan_ptrel_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_erel", &Npfcan_erel_,"Npfcan_erel_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_puppiw", &Npfcan_puppiw_,"Npfcan_puppiw_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_phirel",&Npfcan_phirel_,"Npfcan_phirel_[n_Npfcand_]/F");
    //addBranch(tree,"Npfcan_etarel",&Npfcan_etarel_,"Npfcan_etarel_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_deltaR",&Npfcan_deltaR_,"Npfcan_deltaR_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_isGamma",&Npfcan_isGamma_,"Npfcan_isGamma_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_HadFrac",&Npfcan_HadFrac_,"Npfcan_HadFrac_[n_Npfcand_]/F");
    addBranch(tree,"Npfcan_drminsv",&Npfcan_drminsv_,"Npfcan_drminsv_[n_Npfcand_]/F");


}

void ntuple_pfCands::readEvent(const edm::Event& iEvent){


    n_Npfcand_=0;
    n_Cpfcand_=0;

}



//use either of these functions

bool ntuple_pfCands::fillBranches(const pat::Jet & unsubJet, const pat::Jet & jet, const size_t& unsubJetidx, const size_t& jetidx, const  edm::View<pat::Jet> * unsubColl, const  edm::View<pat::Jet> * coll){

  //float etasign = 1.;
  //if (jet.eta()<0) etasign =-1.;
    //if (unsubJet.eta()<0) etasign =-1.;
    math::XYZVector jetDir = jet.momentum().Unit();
    //math::XYZVector jetDir = unsubJet.momentum().Unit();
    GlobalVector jetRefTrackDir(jet.px(),jet.py(),jet.pz());
    //GlobalVector jetRefTrackDir(unsubJet.px(),unsubJet.py(),unsubJet.pz());
    TVector3 jet_direction      (jet.momentum().Unit().x(),jet.momentum().Unit().y(),jet.momentum().Unit().z());

    const reco::Vertex & pv = vertices()->at(0);


    std::vector<sorting::sortingClass<size_t> > sortedcharged, sortedneutrals, sortedcands;

    const float jet_uncorr_pt=jet.correctedJet("Uncorrected").pt();
    //const float jet_uncorr_e=jet.correctedJet("Uncorrected").energy();
    //const float jet_uncorr_pt=unsubJet.pt();
    //const float jet_uncorr_e=unsubJet.energy();

    TrackInfoBuilder trackinfo(builder);
    //create collection first, to be able to do some sorting

    //for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++){
    for (unsigned int i = 0; i <  unsubJet.numberOfDaughters(); i++){
      //const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
      const pat::PackedCandidate* PackedCandidate = dynamic_cast<const pat::PackedCandidate*>(unsubJet.daughter(i));

        if(PackedCandidate){
	  if(PackedCandidate->pt() < min_candidate_pt_) continue; 
	  if(PackedCandidate->charge()!=0){
	    trackinfo.buildTrackInfo(PackedCandidate,jetDir,jetRefTrackDir,pv);
                sortedcharged.push_back(sorting::sortingClass<size_t>
					(i, trackinfo.getTrackSip2dSig(),
					 -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
	  }
	  else{
	    sortedneutrals.push_back(sorting::sortingClass<size_t>
				     (i, -1, -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));
	  }
	  sortedcands.push_back(sorting::sortingClass<size_t>
				   (i, -1, -mindrsvpfcand(PackedCandidate), PackedCandidate->pt()/jet_uncorr_pt));

        }
    }
    std::sort(sortedcharged.begin(),sortedcharged.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_Cpfcand_ = std::min(sortedcharged.size(),max_pfcand_);
    
    std::sort(sortedneutrals.begin(),sortedneutrals.end(),sorting::sortingClass<size_t>::compareByABCInv);
    n_Npfcand_ = std::min(sortedneutrals.size(),max_pfcand_);

    std::sort(sortedcands.begin(),sortedcands.end(),sorting::sortingClass<size_t>::compareByABCInv);
    ncand_ = std::min(sortedcands.size(),max_pfcand_);

    std::vector<size_t> sortedchargedindices,sortedneutralsindices,sortedcandindices;

    sortedchargedindices=sorting::invertSortingVector(sortedcharged);
    sortedneutralsindices=sorting::invertSortingVector(sortedneutrals);
    sortedcandindices=sorting::invertSortingVector(sortedcands);
    
    
    //for (unsigned int i = 0; i <  jet.numberOfDaughters(); i++){
    for (unsigned int i = 0; i <  unsubJet.numberOfDaughters(); i++){
      
      //const pat::PackedCandidate* PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
      const pat::PackedCandidate* PackedCandidate_ = dynamic_cast<const pat::PackedCandidate*>(unsubJet.daughter(i));
      //const auto& PackedCandidate_=s.get();
      if(!PackedCandidate_) continue;
      if(PackedCandidate_->pt() < min_candidate_pt_) continue; 
      
      // get the dr with the closest sv
      float drminpfcandsv_ = mindrsvpfcand(PackedCandidate_);
      
      size_t entryPNET= sortedcandindices.at(i);
      pfcand_pt_[entryPNET] = PackedCandidate_->pt();
      pfcand_eta_[entryPNET] = PackedCandidate_->eta();
      pfcand_phi_[entryPNET] = PackedCandidate_->phi();
      pfcand_mass_[entryPNET] = PackedCandidate_->mass();
      pfcand_energy_[entryPNET] = PackedCandidate_->energy();
      pfcand_calofraction_[entryPNET] = PackedCandidate_->caloFraction();
      pfcand_hcalfraction_[entryPNET] = PackedCandidate_->hcalFraction();

      pfcand_dxy_[entryPNET] = catchInfsAndBound(PackedCandidate_->dxy(),0,-50,50);
      pfcand_dxysig_[entryPNET]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(fabs(PackedCandidate_->dxy()/PackedCandidate_->dxyError()),0.,-2000,2000) : 0.;
      pfcand_dz_[entryPNET] = PackedCandidate_->dz();
      pfcand_dzsig_[entryPNET]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(fabs(PackedCandidate_->dz()/PackedCandidate_->dzError()),0.,-2000,2000) : 0.;
      
      TVector3 pfcand_momentum (PackedCandidate_->momentum().x(),PackedCandidate_->momentum().y(),PackedCandidate_->momentum().z());
      pfcand_pperp_ratio_[entryPNET]=jet_direction.Perp(pfcand_momentum)/pfcand_momentum.Mag();
      pfcand_ppara_ratio_[entryPNET]=jet_direction.Dot(pfcand_momentum)/pfcand_momentum.Mag();
      pfcand_deta_[entryPNET]=jet_direction.Eta()-pfcand_momentum.Eta();
      pfcand_dphi_[entryPNET]=jet_direction.DeltaPhi(pfcand_momentum);
      pfcand_etarel_[entryPNET]=reco::btau::etaRel(jetDir,PackedCandidate_->momentum());

      pfcand_frompv_[entryPNET]= PackedCandidate_->fromPV();
      pfcand_id_[entryPNET]= abs(PackedCandidate_->pdgId());
      pfcand_charge_[entryPNET]= PackedCandidate_->charge();

      const reco::Track* track = PackedCandidate_->bestTrack();      
      if(track){ 
	pfcand_track_qual_[entryPNET]= track->qualityMask();
	pfcand_track_chi2_[entryPNET]= track->normalizedChi2();
	pfcand_npixhits_[entryPNET]= track->hitPattern().numberOfValidPixelHits(); 
	pfcand_nstriphits_[entryPNET]= track->hitPattern().numberOfValidStripHits();
	pfcand_nlosthits_[entryPNET]= track->hitPattern().numberOfLostHits(reco::HitPattern::TRACK_HITS)+
	  track->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS)+
	  track->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_OUTER_HITS);


        const reco::Track & PseudoTrack =  PackedCandidate_->pseudoTrack();
	reco::TransientTrack transientTrack;
        transientTrack=builder->build(PseudoTrack);

	Measurement1D meas_ip3d    = IPTools::signedImpactParameter3D(transientTrack,jetRefTrackDir,pv).second;
	Measurement1D meas_jetdist = IPTools::jetTrackDistance(transientTrack,jetRefTrackDir,pv).second;
	Measurement1D meas_decayl  = IPTools::signedDecayLength3D(transientTrack,jetRefTrackDir,pv).second;

	pfcand_trackjet_d3d_[entryPNET]= meas_ip3d.value();
	pfcand_trackjet_d3dsig_[entryPNET]= meas_ip3d.significance();
	pfcand_trackjet_dist_[entryPNET]= -meas_jetdist.value();
	pfcand_trackjet_decayL_[entryPNET]= meas_decayl.value();
      }
      else{
	pfcand_track_qual_[entryPNET]= 0;
	pfcand_track_chi2_[entryPNET]= 0;
	pfcand_npixhits_[entryPNET]= 0;
	pfcand_nstriphits_[entryPNET]=0;
	pfcand_nlosthits_[entryPNET]=0;
	pfcand_trackjet_d3d_[entryPNET]=0;
	pfcand_trackjet_d3dsig_[entryPNET]=0;
	pfcand_trackjet_dist_[entryPNET]=0;
	pfcand_trackjet_decayL_[entryPNET]=0;
      }


      /// This might include more than PF candidates, e.g. Reco muons and could
      /// be double counting. Needs to be checked.!!!!
      ///
      /// Split to charged and neutral candidates
      


        if(PackedCandidate_->charge()!=0 ){

            size_t fillntupleentry= sortedchargedindices.at(i);
            if(fillntupleentry>=max_pfcand_) continue;

            //Cpfcan_pt_[fillntupleentry] = PackedCandidate_->pt();
            //Cpfcan_eta_[fillntupleentry] = PackedCandidate_->eta();
            //Cpfcan_phi_[fillntupleentry] = PackedCandidate_->phi();
            Cpfcan_ptrel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->pt()/jet_uncorr_pt,0,-1,0,-1);
            //Cpfcan_erel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->energy()/jet_uncorr_e,0,-1,0,-1);
            //Cpfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),jet.phi())),0,-2,0,-0.5);
	    //Cpfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),unsubJet.phi())),0,-2,0,-0.5);
            //Cpfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-jet.eta()),0,-2,0,-0.5);
	    //Cpfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-unsubJet.eta()),0,-2,0,-0.5);
            //Cpfcan_deltaR_[fillntupleentry] =catchInfsAndBound(reco::deltaR(*PackedCandidate_,jet),0,-0.6,0,-0.6);
	    //Cpfcan_deltaR_[fillntupleentry] =catchInfsAndBound(reco::deltaR(*PackedCandidate_,unsubJet),0,-0.6,0,-0.6);
            //Cpfcan_dxy_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->dxy()),0,-50,50);
            //Cpfcan_dxyerrinv_[fillntupleentry]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(1/PackedCandidate_->dxyError(),0,-1, 10000.) : -1;
            //Cpfcan_dxysig_[fillntupleentry]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(fabs(PackedCandidate_->dxy()/PackedCandidate_->dxyError()),0.,-2000,2000) : 0.;
            //Cpfcan_dz_[fillntupleentry] = PackedCandidate_->dz();
            //Cpfcan_dzsig_[fillntupleentry]= PackedCandidate_->hasTrackDetails() ? catchInfsAndBound(fabs(PackedCandidate_->dz()/PackedCandidate_->dzError()),0.,-2000,2000) : 0.;
            Cpfcan_VTX_ass_[fillntupleentry] = PackedCandidate_->pvAssociationQuality();

            //Cpfcan_fromPV_[fillntupleentry] = PackedCandidate_->fromPV();

            float tempdontopt=PackedCandidate_->vx();
            tempdontopt++;

            //Cpfcan_vertexChi2_[fillntupleentry]=PackedCandidate_->vertexChi2();
            //Cpfcan_vertexNdof_[fillntupleentry]=PackedCandidate_->vertexNdof();
            //divided
            //Cpfcan_vertexNormalizedChi2_[fillntupleentry]=PackedCandidate_->vertexNormalizedChi2();
            //Cpfcan_vertex_rho_[fillntupleentry]=catchInfsAndBound(PackedCandidate_->vertex().rho(),0,-1,50);
            //Cpfcan_vertex_phirel_[fillntupleentry]=reco::deltaPhi(PackedCandidate_->vertex().phi(),jet.phi());
            //Cpfcan_vertex_etarel_[fillntupleentry]=etasign*(PackedCandidate_->vertex().eta()-jet.eta());
            //Cpfcan_vertex_phirel_[fillntupleentry]=reco::deltaPhi(PackedCandidate_->vertex().phi(),unsubJet.phi());
            //Cpfcan_vertex_etarel_[fillntupleentry]=etasign*(PackedCandidate_->vertex().eta()-unsubJet.eta());
            //Cpfcan_vertexRef_mass_[fillntupleentry]=PackedCandidate_->vertexRef()->p4().M();

	    //TVector3 pfcand_momentum (PackedCandidate_->momentum().x(),PackedCandidate_->momentum().y(),PackedCandidate_->momentum().z());
	    //Cpfcan_pperp_ratio.push_back(jet_direction.Perp(pfcand_momentum)/pfcand_momentum.Mag());
	    //Cpfcan_ppara_ratio.push_back(jet_direction.Dot(pfcand_momentum)/pfcand_momentum.Mag());

            //Cpfcan_puppiw_[fillntupleentry] = PackedCandidate_->puppiWeight();


            /*
            reco::Track::CovarianceMatrix myCov = PseudoTrack.covariance ();
            //https://github.com/cms-sw/cmssw/blob/CMSSW_9_0_X/DataFormats/PatCandidates/interface/PackedCandidate.h#L394

            Cpfcan_dptdpt_[fillntupleentry] =    catchInfsAndBound(myCov[0][0],0,-1,1);
            Cpfcan_detadeta_[fillntupleentry]=   catchInfsAndBound(myCov[1][1],0,-1,0.01);
            Cpfcan_dphidphi_[fillntupleentry]=   catchInfsAndBound(myCov[2][2],0,-1,0.1);

            Cpfcan_dxydxy_[fillntupleentry] =    catchInfsAndBound(myCov[3][3],7.,-1,7); //zero if pvAssociationQuality ==7 ?
            Cpfcan_dzdz_[fillntupleentry] =      catchInfsAndBound(myCov[4][4],6.5,-1,6.5); //zero if pvAssociationQuality ==7 ?
            Cpfcan_dxydz_[fillntupleentry] =     catchInfsAndBound(myCov[3][4],6.,-6,6); //zero if pvAssociationQuality ==7 ?
            Cpfcan_dphidxy_[fillntupleentry] =   catchInfs(myCov[2][3],-0.03); //zero if pvAssociationQuality ==7 ?
            Cpfcan_dlambdadz_[fillntupleentry]=  catchInfs(myCov[1][4],-0.03); //zero if pvAssociationQuality ==7 ?
             */

            trackinfo.buildTrackInfo(PackedCandidate_,jetDir,jetRefTrackDir,pv);

            //Cpfcan_BtagPf_trackMomentum_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackMomentum(),0,0 ,1000);
            //Cpfcan_BtagPf_trackEta_[fillntupleentry]        =catchInfsAndBound(trackinfo.getTrackEta()   ,  0,-5,5);
            Cpfcan_BtagPf_trackEtaRel_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackEtaRel(),  0,-5,15);
            Cpfcan_BtagPf_trackPtRel_[fillntupleentry]      =catchInfsAndBound(trackinfo.getTrackPtRel(),   0,-1,4);
            Cpfcan_BtagPf_trackPPar_[fillntupleentry]       =catchInfsAndBound(trackinfo.getTrackPPar(),    0,-1e5,1e5 );
            Cpfcan_BtagPf_trackDeltaR_[fillntupleentry]     =catchInfsAndBound(trackinfo.getTrackDeltaR(),  0,-5,5 );
            //Cpfcan_BtagPf_trackPtRatio_[fillntupleentry]    =catchInfsAndBound(trackinfo.getTrackPtRatio(), 0,-1,10 );
            Cpfcan_BtagPf_trackPParRatio_[fillntupleentry]  =catchInfsAndBound(trackinfo.getTrackPParRatio(),0,-10,100);
            Cpfcan_BtagPf_trackSip3dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dVal(), 0, -1,1e5 );
            Cpfcan_BtagPf_trackSip3dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip3dSig(), 0, -1,4e4 );
            Cpfcan_BtagPf_trackSip2dVal_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dVal(), 0, -1,70 );
            Cpfcan_BtagPf_trackSip2dSig_[fillntupleentry]   =catchInfsAndBound(trackinfo.getTrackSip2dSig(), 0, -1,4e4 );
            //Cpfcan_BtagPf_trackDecayLen_[fillntupleentry]   =0;
            Cpfcan_BtagPf_trackJetDistVal_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistVal(),0,-20,1 );
            //Cpfcan_BtagPf_trackJetDistSig_[fillntupleentry] =catchInfsAndBound(trackinfo.getTrackJetDistSig(),0,-1,1e5 );
	    /*
            // TO DO: we can do better than that by including reco::muon informations
            Cpfcan_isMu_[fillntupleentry] = 0;
            if(abs(PackedCandidate_->pdgId())==13) {
                Cpfcan_isMu_[fillntupleentry] = 1;
            }
            // TO DO: we can do better than that by including reco::electron informations	    
            Cpfcan_isEl_[fillntupleentry] = 0;
            if(abs(PackedCandidate_->pdgId())==11) {
                Cpfcan_isEl_[fillntupleentry] = 1;

            }
	    */
            //Cpfcan_charge_[fillntupleentry] = PackedCandidate_->charge();
            //Cpfcan_lostInnerHits_[fillntupleentry] = catchInfs(PackedCandidate_->lostInnerHits(),2);
	    //Cpfcan_numberOfPixelHits_[fillntupleentry] = catchInfs(PackedCandidate_->numberOfPixelHits(),-1);

	    //std::cout << PackedCandidate_->lostInnerHits()<< " inner hits " <<std::endl;
	    //std::cout << PackedCandidate_->numberOfPixelHits()<< " Pixel hits + masked " <<std::endl;
	    //std::cout <<PackedCandidate_->pixelLayersWithMeasurement()<< " Pixel hits " <<std::endl;

	    Cpfcan_chi2_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? \
	      catchInfsAndBound(PackedCandidate_->pseudoTrack().normalizedChi2(),300,-1,300) : -1;
	    //For some reason this returns the quality enum not a mask.
	    Cpfcan_quality_[fillntupleentry] = PackedCandidate_->hasTrackDetails() ? 
	      PackedCandidate_->pseudoTrack().qualityMask() : (1 << reco::TrackBase::loose);
	    
            Cpfcan_drminsv_[fillntupleentry] = catchInfsAndBound(drminpfcandsv_,0,-0.4,0,-0.4);

        }
        else{// neutral candidates

            size_t fillntupleentry= sortedneutralsindices.at(i);
            if(fillntupleentry>=max_pfcand_) continue;

            //Npfcan_pt_[fillntupleentry] = PackedCandidate_->pt();
            //Npfcan_eta_[fillntupleentry] = PackedCandidate_->eta();
            //Npfcan_phi_[fillntupleentry] = PackedCandidate_->phi();
            Npfcan_ptrel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->pt()/jet_uncorr_pt,0,-1,0,-1);
            //Npfcan_erel_[fillntupleentry] = catchInfsAndBound(PackedCandidate_->energy()/jet_uncorr_e,0,-1,0,-1);
            //Npfcan_puppiw_[fillntupleentry] = PackedCandidate_->puppiWeight();
            //Npfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),jet.phi())),0,-2,0,-0.5);
	    //Npfcan_phirel_[fillntupleentry] = catchInfsAndBound(fabs(reco::deltaPhi(PackedCandidate_->phi(),unsubJet.phi())),0,-2,0,-0.5);
            //Npfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-jet.eta()),0,-2,0,-0.5);
	    //Npfcan_etarel_[fillntupleentry] = catchInfsAndBound(fabs(PackedCandidate_->eta()-unsubJet.eta()),0,-2,0,-0.5);
            Npfcan_deltaR_[fillntupleentry] = catchInfsAndBound(reco::deltaR(*PackedCandidate_,jet),0,-0.6,0,-0.6);
	    //Npfcan_deltaR_[fillntupleentry] = catchInfsAndBound(reco::deltaR(*PackedCandidate_,unsubJet),0,-0.6,0,-0.6);
            Npfcan_isGamma_[fillntupleentry] = 0;
            if(fabs(PackedCandidate_->pdgId())==22)  Npfcan_isGamma_[fillntupleentry] = 1;
            Npfcan_HadFrac_[fillntupleentry] = PackedCandidate_->hcalFraction();
            Npfcan_drminsv_[fillntupleentry] = catchInfsAndBound(drminpfcandsv_,0,-0.4,0,-0.4);
        }

    } // end loop over jet.numberOfDaughters()

    /*
    std::cout <<"numbers charged/neutrals"<<std::endl;
    std::cout << n_Cpfcand_ << std::endl;
    std::cout << n_Npfcand_ << std::endl;
    std::cout <<"charged IPs"<<std::endl;
    for(size_t i=0;i<n_Cpfcand_;i++){
        std::cout << Cpfcan_BtagPf_trackSip2dSig_[i] << " " << Npfcan_drminsv_[i] << " " << Npfcan_ptrel_[i]<<std::endl;
    }
    std::cout <<"neutrals minDR"<<std::endl;
    for(size_t i=0;i<n_Npfcand_;i++){
        std::cout << Npfcan_drminsv_[i] << " " << Npfcan_ptrel_[i]<<std::endl;
    }
     */

    nCpfcand_=n_Cpfcand_;
    nNpfcand_=n_Npfcand_;

    return true; //For making cuts
}


float ntuple_pfCands::mindrsvpfcand(const pat::PackedCandidate* pfcand) {

    float mindr_ = jetradius_;
    for (unsigned int i=0; i<secVertices()->size(); ++i) {
        if(!pfcand) continue;
        //if(!svs.at(i)) continue;
        float tempdr_ = reco::deltaR(secVertices()->at(i),*pfcand);
        if (tempdr_<mindr_) { mindr_ = tempdr_; }

    }
    return mindr_;
}
