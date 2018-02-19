#include "remollEventAction.hh"
#include "remollGenericDetectorHit.hh"
#include "remollGenericDetectorSum.hh"
#include "remollPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"

#include "remollIO.hh"
#include "remollEvent.hh"

#include "G4Threading.hh"
#include "G4AutoLock.hh"

#include <iostream>
#include <fstream>
using namespace std;

ofstream Secondmyfile1;
ofstream Secondmyfile2;
ofstream Secondmyfile3;
ofstream Secondmyfile4;
ofstream Secondmyfile5;
ofstream Secondmyfile6;
ofstream Secondmyfile7;
ofstream Secondmyfile8;
ofstream Secondmyfile9;

namespace { G4Mutex remollEventActionMutex = G4MUTEX_INITIALIZER; }

remollEventAction::remollEventAction()
: fPrimaryGeneratorAction(0),fEventSeed("") { }

remollEventAction::~remollEventAction() { }

void remollEventAction::BeginOfEventAction(const G4Event* event) {

    Secondmyfile1.open ("parent_position_output.txt", ios::app);
    Secondmyfile1 << event->GetEventID()+1 << " \n";
    Secondmyfile1.close();
    Secondmyfile2.open ("1st_daughter_position_output.txt", ios::app);
    Secondmyfile2 << event->GetEventID()+1 << " \n";
    Secondmyfile2.close();
    Secondmyfile3.open ("2nd_daughter_position_output.txt", ios::app);
    Secondmyfile3 << event->GetEventID()+1 << " \n";
    Secondmyfile3.close();
    Secondmyfile4.open ("3rd_daughter_position_output.txt", ios::app);
    Secondmyfile4 << event->GetEventID()+1 << " \n";
    Secondmyfile4.close();
    Secondmyfile5.open ("4th_daughter_position_output.txt", ios::app);
    Secondmyfile5 << event->GetEventID()+1 << " \n";
    Secondmyfile5.close();
    Secondmyfile6.open ("5th_daughter_position_output.txt", ios::app);
    Secondmyfile6 << event->GetEventID()+1 << " \n";
    Secondmyfile6.close();
    Secondmyfile7.open ("6th_daughter_position_output.txt", ios::app);
    Secondmyfile7 << event->GetEventID()+1 << " \n";
    Secondmyfile7.close();
    Secondmyfile8.open ("7th_daughter_position_output.txt", ios::app);
    Secondmyfile8 << event->GetEventID()+1 << " \n";
    Secondmyfile8.close();
    Secondmyfile9.open ("8th_daughter_position_output.txt", ios::app);
    Secondmyfile9 << event->GetEventID()+1 << " \n";
    Secondmyfile9.close();
 
 }

void remollEventAction::EndOfEventAction(const G4Event* aEvent)
{
  // We collect all interaction with remollIO in this thread for as
  // little locking as possible. This means that all the thread local
  // information must be retrieved from here.

  // Lock mutex
  G4AutoLock lock(&remollEventActionMutex);
  remollIO* io = remollIO::GetInstance();

  // Store random seed
  //fEventSeed = aEvent->GetRandomNumberStatus();
  io->SetEventSeed(fEventSeed);

  // Get primary event action information
  const remollEvent* event = fPrimaryGeneratorAction->GetEvent();
  io->SetEventData(event);

  int goodParticle = 0;	
  int collimatorHit = 0;
  int generation = 0;
  double finalEnergy, finalMomentum;
  // Traverse all hit collections, sort by output type
  G4HCofThisEvent *HCE = aEvent->GetHCofThisEvent();
  for (int hcidx = 0; hcidx < HCE->GetCapacity(); hcidx++) {
    G4VHitsCollection* thiscol = HCE->GetHC(hcidx);
    if (thiscol){ // This is NULL if nothing is stored

      // Dynamic cast to test types, process however see fit and feed to IO
      
      ////  Generic Detector Hits ///////////////////////////////////
      if (remollGenericDetectorHitCollection *thiscast =
          dynamic_cast<remollGenericDetectorHitCollection*>(thiscol)) {
        for (unsigned int hidx = 0; hidx < thiscast->GetSize(); hidx++) {
          io->AddGenericDetectorHit((remollGenericDetectorHit *)
              thiscast->GetHit(hidx));
        }
      }

      ////  Generic Detector Sum ////////////////////////////////////
      if (remollGenericDetectorSumCollection *thiscast =
          dynamic_cast<remollGenericDetectorSumCollection*>(thiscol)) {
        for (unsigned int hidx = 0; hidx < thiscast->GetSize(); hidx++) {
          io->AddGenericDetectorSum((remollGenericDetectorSum *)
              thiscast->GetHit(hidx));
        }
      }

    }
  }


  // Fill tree and reset buffers
  io->FillTree();
  io->Flush();
	Secondmyfile1.open ("parent_position_output.txt", ios::app);
	// if (goodParticle == 1 && collimatorHit == 1) {  // Use collimatorHit==1 if we want to ensure that Moller scattered particles going through the acceptance defining collimator are the only things getting through.
	if (goodParticle == 1 && generation == 1 ) {  
	//if (detectorHit == 1) {
//		Secondmyfile1 << -29 << "     \t" << -29 << "     \t" << -29 << "     \t" << -29 << "     \t" << -29 << "\n";
		Secondmyfile1 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile1 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
//		Secondmyfile1 << -37 << "     \t" << -37 << "     \t" << -37 << "     \t" << -37 << "     \t" << -37 << "\n";
		Secondmyfile1 << -37 << "\t" << -37 << "\t" << -37 << "\n";
//		Secondmyfile1 << finalEnergy << "     \t" << finalMomentum << "     \t" << " \n";
//		-37 means bad: -29 means good particle.
	}
	Secondmyfile1.close();

	Secondmyfile2.open ("1st_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 2) {  
		Secondmyfile2 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile2 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile2 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile2.close();

	Secondmyfile3.open ("2nd_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 3) {  
		Secondmyfile3 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile3 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile3 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile3.close();
	
	Secondmyfile4.open ("3rd_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 4) {  
		Secondmyfile4 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile4 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile4 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile4.close();

	Secondmyfile5.open ("4th_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 5) {  
		Secondmyfile5 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile5 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile5 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile5.close();

	Secondmyfile6.open ("5th_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 6) {  
		Secondmyfile6 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile6 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile6 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile6.close();

	Secondmyfile7.open ("6th_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 7) {  
		Secondmyfile7 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile7 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile7 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile7.close();

	Secondmyfile8.open ("7th_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 8) {  
		Secondmyfile8 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile8 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile8 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile8.close();

	Secondmyfile9.open ("8th_daughter_position_output.txt", ios::app);
	if (goodParticle == 1 && generation == 9) {  
		Secondmyfile9 << -29 << "\t" << -29 << "\t" << -29 << "\n";
		Secondmyfile9 << finalEnergy << "\t" << finalMomentum << "\t" << " \n";
	} else {
		Secondmyfile9 << -37 << "\t" << -37 << "\t" << -37 << "\n";
	}
	Secondmyfile9.close();


  return;

}
