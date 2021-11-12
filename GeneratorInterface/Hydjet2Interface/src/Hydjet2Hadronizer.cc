/*
  Interface to the HYDJET++ (Hydjet2) event generator, produces HepMC events

  Author: Andrey Belyaev (Andrey.Belyaev@cern.ch)

*/

#include <TLorentzVector.h>
#include <TMath.h>
#include <TVector3.h>

#include "GeneratorInterface/Hydjet2Interface/interface/Hydjet2Hadronizer.h"
#include <cmath>
#include <fstream>
#include <iostream>

#include "FWCore/Concurrency/interface/SharedResourceNames.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "GeneratorInterface/Pythia6Interface/interface/Pythia6Declarations.h"
#include "GeneratorInterface/Pythia6Interface/interface/Pythia6Service.h"

#include "HepMC/GenEvent.h"
#include "HepMC/HeavyIon.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/PythiaWrapper6_4.h"
#include "HepMC/SimpleVector.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/HiGenData/interface/GenHIEvent.h"

#include <TStopwatch.h>
#include <time.h>

CLHEP::HepRandomEngine *hjRandomEngine;

using namespace edm;
using namespace std;
using namespace gen;
clock_t cl = clock();

TStopwatch timer;

bool ev = false;
namespace {
int convertStatusForComponents(int sta, int typ) {
  if (sta == 1 && typ == 0)
    return 6;
  if (sta == 1 && typ == 1)
    return 7;
  if (sta == 2 && typ == 0)
    return 16;
  if (sta == 2 && typ == 1)
    return 17;

  else
    return sta;
}
/*
int convertStatus(int st) {
  if (st <= 0)
    return 0;
  if (st <= 10)
    return 1;
  if (st <= 20)
    return 2;
  if (st <= 30)
    return 3;

  else
    return st;
}
*/
} // namespace

const std::vector<std::string> Hydjet2Hadronizer::theSharedResources = {edm::SharedResourceNames::kPythia6}; //, gen::FortranInstance::kFortranInstance};

//____________________________________________________________________________________________
Hydjet2Hadronizer::Hydjet2Hadronizer(const edm::ParameterSet &pset, edm::ConsumesCollector &&iC)
    : BaseHadronizer(pset),
      rotate_(pset.getParameter<bool>("rotateEventPlane")),
      evt(nullptr),
      nsub_(0),
      nhard_(0),
      nsoft_(0),
      phi0_(0.),
      sinphi0_(0.),
      cosphi0_(1.),
      fVertex_(nullptr),
      pythia6Service_(new Pythia6Service(pset))

{
  // constructor
  fParams.doPrintInfo = false;
  fParams.allowEmptyEvent = false;
  fParams.fNevnt = 0;                                                     //not used in CMSSW
  fParams.femb = pset.getParameter<int>("embeddingMode");                 //
  fParams.fSqrtS = pset.getParameter<double>("fSqrtS");                   // C.m.s. energy per nucleon pair
  fParams.fAw = pset.getParameter<double>("fAw");                         // Atomic weigth of nuclei, fAw
  fParams.fIfb = pset.getParameter<int>("fIfb");                          // Flag of type of centrality generation, fBfix (=0 is fixed by fBfix, >0 distributed [fBfmin, fBmax])
  fParams.fBmin = pset.getParameter<double>("fBmin");                     // Minimum impact parameter in units of nuclear radius, fBmin
  fParams.fBmax = pset.getParameter<double>("fBmax");                     // Maximum impact parameter in units of nuclear radius, fBmax
  fParams.fBfix = pset.getParameter<double>("fBfix");                     // Fixed impact parameter in units of nuclear radius, fBfix
  fParams.fT = pset.getParameter<double>("fT");                           // Temperature at chemical freeze-out, fT [GeV]
  fParams.fMuB = pset.getParameter<double>("fMuB");                       // Chemical baryon potential per unit charge, fMuB [GeV]
  fParams.fMuS = pset.getParameter<double>("fMuS");                       // Chemical strangeness potential per unit charge, fMuS [GeV]
  fParams.fMuC = pset.getParameter<double>("fMuC");                       // Chemical charm potential per unit charge, fMuC [GeV] (used if charm production is turned on)
  fParams.fMuI3 = pset.getParameter<double>("fMuI3");                     // Chemical isospin potential per unit charge, fMuI3 [GeV]
  fParams.fThFO = pset.getParameter<double>("fThFO");                     // Temperature at thermal freeze-out, fThFO [GeV]
  fParams.fMu_th_pip = pset.getParameter<double>("fMu_th_pip");           // Chemical potential of pi+ at thermal freeze-out, fMu_th_pip [GeV]
  fParams.fTau = pset.getParameter<double>("fTau");                       // Proper time proper at thermal freeze-out for central collisions, fTau [fm/c]
  fParams.fSigmaTau = pset.getParameter<double>("fSigmaTau");             // Duration of emission at thermal freeze-out for central collisions, fSigmaTau [fm/c]
  fParams.fR = pset.getParameter<double>("fR");                           // Maximal transverse radius at thermal freeze-out for central collisions, fR [fm]
  fParams.fYlmax = pset.getParameter<double>("fYlmax");                   // Maximal longitudinal flow rapidity at thermal freeze-out, fYlmax
  fParams.fUmax = pset.getParameter<double>("fUmax");                     // Maximal transverse flow rapidity at thermal freeze-out for central collisions, fUmax
  fParams.frhou2 = pset.getParameter<double>("fRhou2");                   //parameter to swich ON/OFF = 0) rhou2
  fParams.frhou3 = pset.getParameter<double>("fRhou3");                   //parameter to swich ON/OFF(0) rhou3
  fParams.frhou4 = pset.getParameter<double>("fRhou4");                   //parameter to swich ON/OFF(0) rhou4
  fParams.fDelta = pset.getParameter<double>("fDelta");                   // Momentum azimuthal anizotropy parameter at thermal freeze-out, fDelta
  fParams.fEpsilon = pset.getParameter<double>("fEpsilon");               // Spatial azimuthal anisotropy parameter at thermal freeze-out, fEpsilon
  fParams.fv2 = pset.getParameter<double>("fKeps2");                      //parameter to swich ON/OFF(0) epsilon2 fluctuations
  fParams.fv3 = pset.getParameter<double>("fKeps3");                      //parameter to swich ON/OFF(0) epsilon3 fluctuations
  fParams.fIfDeltaEpsilon = pset.getParameter<double>("fIfDeltaEpsilon"); // Flag to specify fDelta and fEpsilon values, fIfDeltaEpsilon (=0 user's ones, >=1 calculated)
  fParams.fDecay = pset.getParameter<int>("fDecay");                      // Flag to switch on/off hadron decays, fDecay (=0 decays off, >=1 decays on)
  fParams.fWeakDecay = pset.getParameter<double>("fWeakDecay");           // Low decay width threshold fWeakDecay[GeV]: width<fWeakDecay decay off, width>=fDecayWidth decay on; can be used to switch off weak decays
  fParams.fEtaType = pset.getParameter<double>("fEtaType");               // Flag to choose longitudinal flow rapidity distribution, fEtaType (=0 uniform, >0 Gaussian with the dispersion Ylmax)
  fParams.fTMuType = pset.getParameter<double>("fTMuType");               // Flag to use calculated T_ch, mu_B and mu_S as a function of fSqrtS, fTMuType (=0 user's ones, >0 calculated)
  fParams.fCorrS = pset.getParameter<double>("fCorrS");                   // Strangeness supression factor gamma_s with fCorrS value (0<fCorrS <=1, if fCorrS <= 0 then it is calculated)
  fParams.fCharmProd = pset.getParameter<int>("fCharmProd");              // Flag to include thermal charm production, fCharmProd (=0 no charm production, >=1 charm production)
  fParams.fCorrC = pset.getParameter<double>("fCorrC");                   // Charmness enhancement factor gamma_c with fCorrC value (fCorrC >0, if fCorrC<0 then it is calculated)
  fParams.fNhsel = pset.getParameter<int>("fNhsel");                      //Flag to include jet (J)/jet quenching (JQ) and hydro (H) state production, fNhsel (0 H on & J off, 1 H/J on & JQ off, 2 H/J/HQ on, 3 J on & H/JQ off, 4 H off & J/JQ on)
  fParams.fPyhist = pset.getParameter<int>("fPyhist");                    // Flag to suppress the output of particle history from PYTHIA, fPyhist (=1 only final state particles; =0 full particle history from PYTHIA)
  fParams.fIshad = pset.getParameter<int>("fIshad");                      // Flag to switch on/off nuclear shadowing, fIshad (0 shadowing off, 1 shadowing on)
  fParams.fPtmin = pset.getParameter<double>("fPtmin");                   // Minimal pt of parton-parton scattering in PYTHIA event, fPtmin [GeV/c]
  fParams.fT0 = pset.getParameter<double>("fT0");                         // Initial QGP temperature for central Pb+Pb collisions in mid-rapidity, fT0 [GeV]
  fParams.fTau0 = pset.getParameter<double>("fTau0");                     // Proper QGP formation time in fm/c, fTau0 (0.01<fTau0<10)
  fParams.fNf = pset.getParameter<int>("fNf");                            // Number of active quark flavours in QGP, fNf (0, 1, 2 or 3)
  fParams.fIenglu = pset.getParameter<int>("fIenglu");                    // Flag to fix type of partonic energy loss, fIenglu (0 radiative and collisional loss, 1 radiative loss only, 2 collisional loss only)
  fParams.fIanglu = pset.getParameter<int>("fIanglu");                    // Flag to fix type of angular distribution of in-medium emitted gluons, fIanglu (0 small-angular, 1 wide-angular, 2 collinear).

  edm::FileInPath f1("GeneratorInterface/Hydjet2Interface/data/particles.data");
  strcpy(fParams.partDat, (f1.fullPath()).c_str());

  edm::FileInPath f2("GeneratorInterface/Hydjet2Interface/data/tabledecay.txt");
  strcpy(fParams.tabDecay, (f2.fullPath()).c_str());

  fParams.fPythiaTune = false;

  if (pset.exists("signalVtx"))
    signalVtx_ = pset.getUntrackedParameter<std::vector<double>>("signalVtx");

  if (signalVtx_.size() == 4) {
    if (!fVertex_)
      fVertex_ = new HepMC::FourVector();
    LogDebug("EventSignalVertex") << "Setting event signal vertex "
                                  << " x = " << signalVtx_.at(0) << " y = " << signalVtx_.at(1)
                                  << "  z= " << signalVtx_.at(2) << " t = " << signalVtx_.at(3) << endl;
    fVertex_->set(signalVtx_.at(0), signalVtx_.at(1), signalVtx_.at(2), signalVtx_.at(3));
  }

  // PYLIST Verbosity Level
  // Valid PYLIST arguments are: 1, 2, 3, 5, 7, 11, 12, 13
  pythiaPylistVerbosity_ = pset.getUntrackedParameter<int>("pythiaPylistVerbosity", 1);
  LogDebug("PYLISTverbosity") << "Pythia PYLIST verbosity level = " << pythiaPylistVerbosity_;
  //Max number of events printed on verbosity level
  maxEventsToPrint_ = pset.getUntrackedParameter<int>("maxEventsToPrint", 0);
  LogDebug("Events2Print") << "Number of events to be printed = " << maxEventsToPrint_;
  if (fParams.femb == 1) {
    fParams.fIfb = 0;
    src_ = iC.consumes<CrossingFrame<edm::HepMCProduct>>(
        pset.getUntrackedParameter<edm::InputTag>("backgroundLabel", edm::InputTag("mix", "generatorSmeared")));
  }

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: constructor ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

//std::cout << std::setfill ('*') << std::setw (10)<<std::endl;
//std::cout << "* This is HYDJET v."<<Hydjet2::GetVersion().at(0)<<"."<<Hydjet2::GetVersion().at(1)<< "."<<Hydjet2::GetVersion().at(2) << " *"<<std::endl;
//std::cout << std::setfill ('*') << std::setw (10)<<std::endl;



}
//__________________________________________________________________________________________
Hydjet2Hadronizer::~Hydjet2Hadronizer() {
  // destructor
  call_pystat(1);
  delete pythia6Service_;
}

//_____________________________________________________________________
void Hydjet2Hadronizer::doSetRandomEngine(CLHEP::HepRandomEngine *v) {
  pythia6Service_->setRandomEngine(v);
  hjRandomEngine = v;
}

//______________________________________________________________________________________________________
bool Hydjet2Hadronizer::readSettings(int) {
  Pythia6Service::InstanceWrapper guard(pythia6Service_);
  pythia6Service_->setGeneralParams();

  //SERVICE.iseed_fromC
  fParams.fSeed = hjRandomEngine->CLHEP::HepRandomEngine::getSeed();
  LogInfo("Hydjet2Hadronizer|GenSeed") << "Seed for random number generation: "
                                       << hjRandomEngine->CLHEP::HepRandomEngine::getSeed();

  return kTRUE;
}

//______________________________________________________________________________________________________
bool Hydjet2Hadronizer::initializeForInternalPartons() {
  Pythia6Service::InstanceWrapper guard(pythia6Service_);

  // the input impact parameter (bxx_) is in [fm]; transform in [fm/RA] for hydjet usage
  const double ra = nuclear_radius();
  LogInfo("Hydjet2Hadronizer|RAScaling") << "Nuclear radius(RA) =  " << ra;
  fParams.fBmin /= ra;
  fParams.fBmax /= ra;
  fParams.fBfix /= ra;

  hj2 = new Hydjet2(fParams);

  return kTRUE;
}

//__________________________________________________________________________________________
bool Hydjet2Hadronizer::generatePartonsAndHadronize() {
  Pythia6Service::InstanceWrapper guard(pythia6Service_);

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: EVENT ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;
  // generate single event
  if (fParams.femb == 1) {
    const edm::Event &e = getEDMEvent();
    HepMC::GenVertex *genvtx = nullptr;
    const HepMC::GenEvent *inev = nullptr;
    Handle<CrossingFrame<HepMCProduct>> cf;
    e.getByToken(src_, cf);
    MixCollection<HepMCProduct> mix(cf.product());
    if (mix.size() < 1) {
      throw cms::Exception("MatchVtx") << "Mixing has " << mix.size() << " sub-events, should have been at least 1"
                                       << endl;
    }
    const HepMCProduct &bkg = mix.getObject(0);
    if (!(bkg.isVtxGenApplied())) {
      throw cms::Exception("MatchVtx") << "Input background does not have smeared vertex!" << endl;
    } else {
      inev = bkg.GetEvent();
    }

    genvtx = inev->signal_process_vertex();

    if (!genvtx)
      throw cms::Exception("MatchVtx") << "Input background does not have signal process vertex!" << endl;

    double aX, aY, aZ, aT;

    aX = genvtx->position().x();
    aY = genvtx->position().y();
    aZ = genvtx->position().z();
    aT = genvtx->position().t();

    if (!fVertex_) {
      fVertex_ = new HepMC::FourVector();
    }
    LogInfo("MatchVtx") << " setting vertex "
                        << " aX " << aX << " aY " << aY << " aZ " << aZ << " aT " << aT << endl;
    fVertex_->set(aX, aY, aZ, aT);

    const HepMC::HeavyIon *hi = inev->heavy_ion();

    if (hi) {
      fParams.fBfix = (hi->impact_parameter()) / nuclear_radius();
      phi0_ = hi->event_plane_angle();
      sinphi0_ = sin(phi0_);
      cosphi0_ = cos(phi0_);
    } else {
      LogWarning("EventEmbedding") << "Background event does not have heavy ion record!";
    }

  } else if (rotate_)
    rotateEvtPlane();

  nsoft_ = 0;
  nhard_ = 0;

  // generate one HYDJET event
  int ntry = 0, xxx=0;

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: generating event ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

  while (nsoft_ == 0 && nhard_ == 0) {
    if (ntry > 100) {
      LogError("Hydjet2EmptyEvent") << "##### HYDJET2: No Particles generated, Number of tries =" << ntry;
      // Throw an exception. Use the EventCorruption exception since it maps onto SkipEvent
      // which is what we want to do here.
      std::ostringstream sstr;
      sstr << "Hydjet2HadronizerProducer: No particles generated after " << ntry << " tries.\n";
      edm::Exception except(edm::errors::EventCorruption, sstr.str());
      throw except;
    } else {

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: generating event core::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

      hj2->GenerateEvent(fParams.fBfix);

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: generating event core - done::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

      if (hj2->IsEmpty()){
       xxx++;  
       continue;
      }

      nsoft_ = hj2->GetNhyd();
      nsub_ = hj2->GetNjet();
      nhard_ = hj2->GetNpyt();

      //100 trys
      ++ntry;
    }
  }

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: generating event - done ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;


  std::cout<< "Waisted events: "<< xxx << " ntry: "<< ntry<<std::endl;
  if (ev == 0) {
    Sigin = hj2->GetSigin();
    Sigjet = hj2->GetSigjet();
  }
  ev = 1;

  if (fParams.fNhsel < 3)
    nsub_++;

  // event information
  HepMC::GenEvent *evt = new HepMC::GenEvent();

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: getting particles ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

  if (nhard_ > 0 || nsoft_ > 0)
    get_particles(evt);

  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: getting particles - done ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

  evt->set_signal_process_id(pypars.msti[0]); // type of the process
  evt->set_event_scale(pypars.pari[16]);      // Q^2
  add_heavy_ion_rec(evt);

  if (fVertex_) {
    // generate new vertex & apply the shift
    // Copy the HepMC::GenEvent
    std::unique_ptr<edm::HepMCProduct> HepMCEvt(new edm::HepMCProduct(evt));
    HepMCEvt->applyVtxGen(fVertex_);
    evt = new HepMC::GenEvent((*HepMCEvt->GetEvent()));
  }

  HepMC::HEPEVT_Wrapper::check_hepevt_consistency();
  LogDebug("HEPEVT_info") << "Ev numb: " << HepMC::HEPEVT_Wrapper::event_number() << " Entries number: " << HepMC::HEPEVT_Wrapper::number_entries() << " Max. entries " << HepMC::HEPEVT_Wrapper::max_number_entries() << std::endl;

  event().reset(evt);
  
  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: EVENT - done ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

  return kTRUE;
}

//________________________________________________________________
bool Hydjet2Hadronizer::declareStableParticles(const std::vector<int> &_pdg) {
  std::vector<int> pdg = _pdg;
  for (size_t i = 0; i < pdg.size(); i++) {
    int pyCode = pycomp_(pdg[i]);
    std::ostringstream pyCard;
    pyCard << "MDCY(" << pyCode << ",1)=0";
    std::cout << pyCard.str() << std::endl;
    call_pygive(pyCard.str());
  }
  return true;
}
//________________________________________________________________
bool Hydjet2Hadronizer::hadronize() { return false; }
bool Hydjet2Hadronizer::decay() { return true; }
bool Hydjet2Hadronizer::residualDecay() { return true; }
void Hydjet2Hadronizer::finalizeEvent() {}
void Hydjet2Hadronizer::statistics() {}
const char *Hydjet2Hadronizer::classname() const { return "gen::Hydjet2Hadronizer"; }

//________________________________________________________________
void Hydjet2Hadronizer::rotateEvtPlane() {
  const double pi = 3.14159265358979;
  phi0_ = 2. * pi * gen::pyr_(nullptr) - pi;
  sinphi0_ = sin(phi0_);
  cosphi0_ = cos(phi0_);
}

//_____________________________________________________________________
bool Hydjet2Hadronizer::get_particles(HepMC::GenEvent *evt) {
  cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: gp init::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

  //int Ntot = hj2->GetNtot();
  LogDebug("SubEvent") << " Number of sub events " << nsub_;
  LogDebug("Hydjet") << " Number of hard events " << hj2->GetNjet();
  LogDebug("Hydjet") << " Number of hard particles " << nhard_;
  LogDebug("Hydjet") << " Number of soft particles " << nsoft_;
  LogDebug("Hydjet") << " nhard_ + nsoft_ = " << nhard_ + nsoft_ << " Ntot = " << hj2->GetNtot() << endl;

  int ihy = 0;
  //int isub = -1;
  int isub_l = -1;
  int stab = 0;

  vector<HepMC::GenParticle *> primary_particle(hj2->GetNtot());
  vector<HepMC::GenParticle *> particle(hj2->GetNtot());

  HepMC::GenVertex *sub_vertices = new HepMC::GenVertex(HepMC::FourVector(0, 0, 0, 0), 0); // just initialization

  //std::vector<int> iJet;
  //iJet = hj2->GetiJet();
  //std::vector<int> pythiaStatus;
  //pythiaStatus = hj2->GetPythiaStatus();
  //std::vector<int> pdg;
  //pdg = hj2->GetPdg();
  //std::vector<int> MotherIndex;
  //MotherIndex = hj2->GetMotherIndex();
  //std::vector<int> FirstDaughterIndex;
  //FirstDaughterIndex = hj2->GetFirstDaughterIndex();
  //std::vector<int> LastDaughterIndex;
  //LastDaughterIndex = hj2->GetLastDaughterIndex();
  //std::vector<double> X;
  //X = hj2->GetX();
  //std::vector<double> Y;
  //Y = hj2->GetY();
  //std::vector<double> Z;
  //Z = hj2->GetZ();
  //std::vector<int> ifFinal;
  //ifFinal = hj2->GetFinal();

//TBenchmark time;
TStopwatch timeAll;

 cl = clock();
  if ( cl != (clock_t)-1 )
        std::cout<<"::: gp loop ::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;


timer.Stop();
timer.Reset();

  while (ihy < hj2->GetNtot()) {


    //time.Start("gpl");
    //time.Reset();
    //time.Start("gpl0");
    //time.Stop("gpl0");

    //isub = hj2->GetiJet().at(ihy);

    //time.Stop();


//cl = clock();
//  if ( cl != (clock_t)-1 )
//        std::cout<<std::fixed<<std::setprecision(10)<<"::: gpl 1  ::: "<< cl << std::endl;
//    time.Stop("gpl0");
   // time.Start("gpl1");

    if ((hj2->GetiJet().at(ihy)) != isub_l) {
      sub_vertices = new HepMC::GenVertex(HepMC::FourVector(0, 0, 0, 0), hj2->GetiJet().at(ihy));
      evt->add_vertex(sub_vertices);
      if (!evt->signal_process_vertex())
        evt->set_signal_process_vertex(sub_vertices);
      isub_l = hj2->GetiJet().at(ihy);
    }

    if ((hj2->GetFinal().at(ihy)) == 1) //convertStatus(hj2->GetPythiaStatus().at(ihy)) == 1)
      stab++;
/*    LogDebug("Hydjet_array") << ihy << " MULTin ev.:" << hj2->GetNtot() << " SubEv.#" << isub << " Part #" << ihy + 1
                             << ", PDG: " << pdg.at(ihy) << " (st. " << convertStatus(pythiaStatus.at(ihy))
                             << ") mother=" << MotherIndex.at(ihy) + 1 << ", childs ("
                             << FirstDaughterIndex.at(ihy) + 1 << "-" << LastDaughterIndex.at(ihy) + 1 << "), vtx ("
                             << X.at(ihy) << "," << Y.at(ihy) << "," << Z.at(ihy) << ") "
                             << std::endl;
*/


//cl = clock();
//  if ( cl != (clock_t)-1 )
//        std::cout<<std::fixed<<std::setprecision(10)<<"::: gpl 2  ::: "<< cl << std::endl;

   // time.Stop("gpl1");
   // time.Start("gpl2");


//    time.Start(kFALSE);
    if ((hj2->GetMotherIndex().at(ihy)) <= 0) {
      primary_particle.at(ihy) = build_hyjet2(ihy, ihy + 1);
      sub_vertices->add_particle_out(primary_particle.at(ihy));
      LogDebug("Hydjet_array") << " ---> " << ihy + 1 << std::endl;
    } else {
      particle.at(ihy) = build_hyjet2(ihy, ihy + 1);
      int mid = hj2->GetMotherIndex().at(ihy);
      int mid_t = mid;
  //  time.Stop();

//cl = clock();
//  if ( cl != (clock_t)-1 )
//        std::cout<<std::fixed<<std::setprecision(10)<<"::: gpl 3  ::: "<< (long long)cl << std::endl;

  //  time.Stop("gpl2");
  //  time.Start("gpl3");


      while ((mid < ihy) && ((hj2->GetPdg().at(ihy)) < 100) && ((hj2->GetFirstDaughterIndex().at(ihy)) == ihy))
        mid++;

//cl = clock();
//  if ( cl != (clock_t)-1 )
//        std::cout<<std::fixed<<std::setprecision(10)<<"::: gpl 4  ::: "<< cl << std::endl;
  //  time.Stop("gpl3");
  //  time.Start("gpl4");


      if ((hj2->GetPdg().at(ihy)) < 100)
        mid = mid_t;

      HepMC::GenParticle *mother = primary_particle.at(mid);

//TBenchmark time;
//  time.Start("time");
//  std::cout<<"CPU time = "<<time.GetCpuTime("time")<<", Real time = "<<time.GetRealTime("time")<<std::endl; 

  //  time.Stop("gpl4");
  //  time.Start("gpl5");

	//    time.Start(kFALSE); //timeAll.Start(kFALSE);
	      HepMC::GenVertex *prods = build_hyjet2_vertex(ihy, (hj2->GetiJet().at(ihy)));
	//timeAll.Stop();

	//time.Stop();
	//time.Stop("time");
	//  std::cout<<"CPU time = "<<time.GetCpuTime("time")<<", Real time = "<<time.GetRealTime("time")<<std::endl;  

	  //  time.Stop("gpl5");
	  //  time.Start("gpl6");


	      if (!mother) {
		mother = particle.at(mid);
		primary_particle.at(mid) = mother;
	      }

	      HepMC::GenVertex *prod_vertex = mother->end_vertex();
	      if (!prod_vertex) {
		prod_vertex = prods;
		prod_vertex->add_particle_in(mother);
		LogDebug("Hydjet_array") << " <--- " << mid + 1 << std::endl;
		evt->add_vertex(prod_vertex);
		prods = nullptr;
	      }
	      prod_vertex->add_particle_out(particle.at(ihy));
	      LogDebug("Hydjet_array") << " ---" << mid + 1 << "---> " << ihy + 1 << std::endl;
	      if (prods)
		delete prods;
	    }

	//cl = clock();
	//  if ( cl != (clock_t)-1 )
	//        std::cout<<std::fixed<<std::setprecision(10)<<"::: gpl 5  ::: "<< cl << std::endl;
	   // time.Stop("gpl6");

	    ihy++;

	   // time.Stop("gpl");
	//timeAll.Stop();
	  }

	  //std::cout<<"time = "<<timer.CpuTime()<<std::endl;
	  //std::cout<<"timeAll = "<<timeAll.CpuTime()<<std::endl;


	  //std::cout<<"gpl = "<<time.GetCpuTime("gpl")<<", Real time = "<<time.GetRealTime("gpl")<<std::endl;
	  //std::cout<<"gpl0 = "<<time.GetCpuTime("gpl0")<<", Real time = "<<time.GetRealTime("gpl0")<<std::endl;
	  //std::cout<<"gpl1 = "<<time.GetCpuTime("gpl1")<<", Real time = "<<time.GetRealTime("gpl1")<<std::endl;  
	  //std::cout<<"gpl2 = "<<time.GetCpuTime("gpl2")<<", Real time = "<<time.GetRealTime("gpl2")<<std::endl;       
	  //std::cout<<"gpl3 = "<<time.GetCpuTime("gpl3")<<", Real time = "<<time.GetRealTime("gpl3")<<std::endl;       
	  //std::cout<<"gpl4 = "<<time.GetCpuTime("gpl4")<<", Real time = "<<time.GetRealTime("gpl4")<<std::endl;       
	  //std::cout<<"gpl5 = "<<time.GetCpuTime("gpl5")<<", Real time = "<<time.GetRealTime("gpl5")<<std::endl;       
	  //std::cout<<"gpl6 = "<<time.GetCpuTime("gpl6")<<", Real time = "<<time.GetRealTime("gpl6")<<std::endl;       

	  cl = clock();
	  if ( cl != (clock_t)-1 )
		std::cout<<"::: gp loop - done::: "<< (double)cl / (double)CLOCKS_PER_SEC<< std::endl;

	LogDebug("Hydjet_array") << " MULTin ev.:" << hj2->GetNtot() << ", last index: " << ihy - 1
				   << ", stable particles: " << stab << std::endl;

	  return kTRUE;
	}

	//___________________________________________________________________
	HepMC::GenParticle *Hydjet2Hadronizer::build_hyjet2(int index, int barcode) {
	  // Build particle object corresponding to index in hyjets (soft+hard)
	/*
	  std::vector<double> vPx;
	  vPx = hj2->GetPx();

	  std::vector<double> vPy;
	  vPy = hj2->GetPy();

	  std::vector<double> vPz;
	  vPz = hj2->GetPz();

	  std::vector<double> vE;
	  vE = hj2->GetE();

	  std::vector<int> vPdg;
	  vPdg = hj2->GetPdg();

	  std::vector<int> vFinal;
	  vFinal = hj2->GetFinal();

	  std::vector<int> vType;
	  vType = hj2->GetType();
	*/
	  double px0 = (hj2->GetPx()).at(index);//vPx[index];//(hj2->GetPx()).at(index);
	  double py0 = (hj2->GetPy()).at(index);//vPy[index];//(hj2->GetPy()).at(index);

	  double px = px0 * cosphi0_ - py0 * sinphi0_;
	  double py = py0 * cosphi0_ + px0 * sinphi0_;

	  HepMC::GenParticle *p = new HepMC::GenParticle(HepMC::FourVector(px,				// px
									   py,          		// py
									   (hj2->GetPz()).at(index),//vPz.at(index),//(hj2->GetPz()).at(index),    // pz
									   (hj2->GetE()).at(index)),//vE.at(index)),//(hj2->GetE()).at(index)),    // E
									   (hj2->GetPdg()).at(index),//vPdg.at(index),//(hj2->GetPdg()).at(index),   // id
									   convertStatusForComponents(
											(hj2->GetFinal()).at(index),//vFinal.at(index),//(hj2->GetFinal()).at(index), 
											(hj2->GetType()).at(index)//vType.at(index)//(hj2->GetType()).at(index)
									   ) // status
	  );

	  p->suggest_barcode(barcode);
	  return p;
	}

	//___________________________________________________________________
	HepMC::GenVertex *Hydjet2Hadronizer::build_hyjet2_vertex(int i, int id) {
	  // build verteces for the hyjets stored events
	 //   timer.Start(kFALSE);
	 /*  
	  std::vector<double> vX;
	//    timer.Start(kFALSE);

	  vX = hj2->GetX();
	//timer.Stop();

	  std::vector<double> vY;
	  vY = hj2->GetY();

	timer.Stop();


	  std::vector<double> vZ;
	  vZ = hj2->GetZ();

	  std::vector<double> vT;
	  vT = hj2->GetT();
	*/
	//timer.Stop();
	  double x0 = (hj2->GetX()).at(i);//vX[i];//(hj2->GetX()).at(i);
	  double y0 = (hj2->GetY()).at(i);//vY[i];//(hj2->GetY()).at(i);

	//timer.Stop();

	  double x = x0 * cosphi0_ - y0 * sinphi0_;
	  double y = y0 * cosphi0_ + x0 * sinphi0_;
	  double z = (hj2->GetZ()).at(i);//vZ[i];//(hj2->GetZ()).at(i);
	  double t = (hj2->GetT()).at(i);//vT[i];//(hj2->GetT()).at(i);

	  HepMC::GenVertex *vertex = new HepMC::GenVertex(HepMC::FourVector(x, y, z, t), id);

	//timer.Stop();

	  return vertex;
	}

	//_____________________________________________________________________
	void Hydjet2Hadronizer::add_heavy_ion_rec(HepMC::GenEvent *evt) {
	  // heavy ion record in the final CMSSW Event
	  //double npart = hj2->GetNpart();
	  int nproj = static_cast<int>((hj2->GetNpart()) / 2);
	  int ntarg = static_cast<int>((hj2->GetNpart()) - nproj);
	  //double Bgen = hj2->GetBgen();
	  //int Nbcol = hj2->GetNbcol();

	  HepMC::HeavyIon *hi = new HepMC::HeavyIon(nsub_,                             // Ncoll_hard/N of SubEvents
						    nproj,                             // Npart_proj
						    ntarg,                             // Npart_targ
						    hj2->GetNbcol(),                   // Ncoll
						    0,                                 // spectator_neutrons
						    0,                                 // spectator_protons
						    0,                                 // N_Nwounded_collisions
						    0,                                 // Nwounded_N_collisions
						    0,                                 // Nwounded_Nwounded_collisions
						    hj2->GetBgen() * nuclear_radius(), // impact_parameter in [fm]
						    phi0_,                             // event_plane_angle
						    hj2->GetPsiv3(),                   // eccentricity <<<---- psi for v3!!!
						    Sigin                              // sigma_inel_NN
	  );

	  evt->set_heavy_ion(*hi);
	  delete hi;
	}
