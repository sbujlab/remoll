/*
 * remollGenBlackening.cc
 *
 *  Created on: July 25th, 2018
 *      Author: Audrey Farrell
 */

#include "remollGenBlackening.hh"
// Geant4 headers
#include "G4ParticleTable.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"

// ROOT headers
#include "TFile.h"
#include "TTree.h"
#include "remollEvent.hh"
#include "remollVertex.hh"
#include "remolltypes.hh"

G4Mutex inFileMutexBlk = G4MUTEX_INITIALIZER;
#define pi 3.141592653589793238462643383279502884L

remollGenBlackening::remollGenBlackening()
  : remollVEventGen("blackening"),
  fMinRad(600), fMaxRad(1200),
  fMinPhi(-180.0*deg), fMaxPhi(180.0*deg),
  fSeptFold(true)
{
  // Add to generic messenger
  fThisGenMessenger->DeclareMethod("minRad",&remollGenBlackening::SetMinRadius,"Minimum Radius for Blackening Study");
  fThisGenMessenger->DeclareMethod("maxRad",&remollGenBlackening::SetMaxRadius,"Maximum Radius for Blackening Study");
  fThisGenMessenger->DeclareMethod("minPhi",&remollGenBlackening::SetMinPhi,"Minimum Phi for Blackening Study");
  fThisGenMessenger->DeclareMethod("maxPhi",&remollGenBlackening::SetMaxPhi,"Maximum Phi for Blackening Study");
  fThisGenMessenger->DeclareMethod("useFirstSeptant", &remollGenBlackening::SetSeptantFolding,"Use only the first septant data, implies that you have phi-folded root output");
  G4cout << "Constructed remollGenBlackening" << G4endl;

  // Get needed information from TTrees for each scattering type 
  InitializeMollerVariables();
  InitializeElasticVariables();
  InitializeInelasticVariables();

  fMollerEntry = G4RandFlat::shoot(fMollerEntries);
  fElasticEntry = G4RandFlat::shoot(fElasticEntries);
  fInelasticEntry = G4RandFlat::shoot(fInelasticEntries);
}

remollGenBlackening::~remollGenBlackening()
{
  G4AutoLock inFileLock(&inFileMutexBlk);
  // Close file which deletes tree
  if (fMollerFile) {
    fMollerFile->Close();
    fMollerTree = 0;
  }
  if (fElasticFile) {
    fElasticFile->Close();
    fElasticTree = 0;
  }
  if (fInelasticFile) {
    fInelasticFile->Close();
    fInelasticTree = 0;
  }
}

void remollGenBlackening::SetSeptantFolding(G4bool sept){fSeptFold = sept;}
void remollGenBlackening::SetMinRadius(G4double min){fMinRad = min;}
void remollGenBlackening::SetMaxRadius(G4double max){fMaxRad = max;}
void remollGenBlackening::SetMinPhi(G4double min){fMinPhi = min;}
void remollGenBlackening::SetMaxPhi(G4double max){fMaxPhi = max;}

void remollGenBlackening::InitializeMollerVariables()
{
  G4AutoLock inFileLock(&inFileMutexBlk);
  G4cout << "Getting the external file for moller scattering" << G4endl;

  if (fMollerFile) {
    fMollerFile->Close();
    fMollerFile = 0;
  }
  G4String filename;
  //if the given phi range is small, use the septant folded root files
  if (fSeptFold){
    filename = "remollout_moller_sept.root";
    G4cout << "Using Septant folded input file" << G4endl;
  }else{
    filename = "remollout_moller.root";
  }
  fMollerFile = new TFile(filename);
  if (! fMollerFile) {
    G4cerr << "Could not open external event file " << filename << G4endl;
    return;
  }

  // Try to find tree in file
  fMollerFile->GetObject("T",fMollerTree);
  if (! fMollerTree) {
    G4cerr << "Could not find tree T in event file (SetGenExternalFile)" << filename << G4endl;
    return;
  }
  inFileLock.unlock();

  // Get number of entries
  fMollerEntries = fMollerTree->GetEntries();

  // Initialize tree
  if (fMollerTree->GetBranch("hit")) {
    fMollerTree->SetBranchAddress("hit", &fMollerHit);
  } else {
    G4cerr << "Could not find branch hit in event file " << filename << G4endl;
    return;
  }
  if (fMollerTree->GetBranch("ev")) {
    fMollerTree->SetBranchAddress("ev", &fMollerEvent);
  } else {
    G4cerr << "Could not find branch ev in event file " << filename << G4endl;
    return;
  }
  if (fMollerTree->GetBranch("rate")) {
    fMollerTree->SetBranchAddress("rate",&fMollerRate);
  } else {
    G4cerr << "Could not find branch rate in event file " << filename << G4endl;
    return;
  }
  fMollerTree->GetEntry(0);
}
void remollGenBlackening::InitializeElasticVariables()
{
  G4AutoLock inFileLock(&inFileMutexBlk);
  G4cout << "Getting the external file for elastic scattering" << G4endl;

  if (fElasticFile) {
    fElasticFile->Close();
    fElasticFile = 0;
  }

  G4String filename;
  //if the given phi range is small, use the septant folded root files
  if (fSeptFold)
    filename = "remollout_elastic_sept.root";
  else
    filename = "remollout_elastic.root";
  // Try to open filename
  fElasticFile = new TFile(filename);
  if (! fElasticFile) {
    G4cerr << "Could not open external event file " << filename << G4endl;
    return;
  }

  // Try to find tree in file
  fElasticFile->GetObject("T",fElasticTree);
  if (! fElasticTree) {
    G4cerr << "Could not find tree T in event file (SetGenExternalFile)" << filename << G4endl;
    return;
  }
  inFileLock.unlock();

  // Get number of entries
  fElasticEntries = fElasticTree->GetEntries();

  // Initialize tree
  if (fElasticTree->GetBranch("hit")) {
    fElasticTree->SetBranchAddress("hit", &fElasticHit);
  } else {
    G4cerr << "Could not find branch hit in event file " << filename << G4endl;
    return;
  }
  if (fElasticTree->GetBranch("ev")) {
    fElasticTree->SetBranchAddress("ev", &fElasticEvent);
  } else {
    G4cerr << "Could not find branch ev in event file " << filename << G4endl;
    return;
  }
  if (fElasticTree->GetBranch("rate")) {
    fElasticTree->SetBranchAddress("rate",&fElasticRate);
  } else {
    G4cerr << "Could not find branch rate in event file " << filename << G4endl;
    return;
  }
  fElasticTree->GetEntry(0);
}

void remollGenBlackening::InitializeInelasticVariables()
{
  G4AutoLock inFileLock(&inFileMutexBlk);
  G4cout << "Getting the external file for inelastic scattering" << G4endl;

  if (fInelasticFile) {
    fInelasticFile->Close();
    fInelasticFile = 0;
  }

  G4String filename;
  //if the given phi range is small, use the septant folded root files
  if (fSeptFold)
    filename = "remollout_inelastic_sept.root";
  else
    filename = "remollout_inelastic.root";
  // Try to open filename
  fInelasticFile = new TFile(filename);
  if (! fInelasticFile) {
    G4cerr << "Could not open external event file " << filename << G4endl;
    return;
  }

  // Try to find tree in file
  fInelasticFile->GetObject("T",fInelasticTree);
  if (! fInelasticTree) {
    G4cerr << "Could not find tree T in event file (SetGenExternalFile)" << filename << G4endl;
    return;
  }
  inFileLock.unlock();

  // Get number of entries
  fInelasticEntries = fInelasticTree->GetEntries();

  // Initialize tree
  if (fInelasticTree->GetBranch("hit")) {
    fInelasticTree->SetBranchAddress("hit", &fInelasticHit);
  } else {
    G4cerr << "Could not find branch hit in event file " << filename << G4endl;
    return;
  }
  if (fInelasticTree->GetBranch("ev")) {
    fInelasticTree->SetBranchAddress("ev", &fInelasticEvent);
  } else {
    G4cerr << "Could not find branch ev in event file " << filename << G4endl;
    return;
  }
  if (fInelasticTree->GetBranch("rate")) {
    fInelasticTree->SetBranchAddress("rate",&fInelasticRate);
  } else {
    G4cerr << "Could not find branch rate in event file " << filename << G4endl;
    return;
  }
  fInelasticTree->GetEntry(0);
}

void remollGenBlackening::SamplePhysics(remollVertex* /* vert */, remollEvent* evt)
{
  //Determine which scattering type to sample from
  double rndm = G4RandFlat::shoot(0.0,1.0);

  fMollerTree->GetEntry(fMollerEntry);
  fElasticTree->GetEntry(fElasticEntry);
  fInelasticTree->GetEntry(fInelasticEntry);

  double totRate = fMollerRate + fElasticRate + fInelasticRate;
  if (rndm < fMollerRate/totRate){
    SampleMollerPhysics(evt);
  }else if (rndm < (fMollerRate + fElasticRate)/totRate){
    SampleElasticPhysics(evt);
  }else{
    SampleInelasticPhysics(evt);
  }
}

void remollGenBlackening::SampleMollerPhysics(remollEvent* evt)
{
  // Loop until we find at least one event with some particles
  int number_of_particles = 0;
  do {

    // Read next event from tree and increment
    if (fMollerEntry >= fMollerEntries)
      fMollerEntry = 0;
    fMollerTree->GetEntry(fMollerEntry++);

    // Weighting completely handled by event file
    evt->SetEffCrossSection(fMollerEvent->xs*microbarn);
    evt->SetQ2(fMollerEvent->Q2);
    evt->SetW2(fMollerEvent->W2);
    evt->SetAsymmetry(fMollerEvent->A*ppb);

    // Loop over all hits in this event
    for (size_t i = 0; i < fMollerHit->size(); i++) {
      // Create local copy of this hit
      remollGenericDetectorHit_t hit = fMollerHit->at(i);

      // Select only the requested detector ID and hit area
      if (hit.det != 28 || hit.pid != 11 || hit.r < fMinRad || hit.r > fMaxRad || atan2(hit.y,hit.x) < fMinPhi || atan2(hit.y,hit.x) > fMaxPhi) 
        continue;
      // Get particle name
      G4ParticleTable* particletable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particle = particletable->FindParticle(hit.pid);
      G4String particlename = particle->GetParticleName();

      double deltaPhi = CLHEP::RandFlat::shoot(-2,2);
      double vX, vY, vZ;
      double zOffset = 500;

      vZ = hit.z - zOffset;

      double theta = acos(hit.pz/hit.p);

      double radius = zOffset*tan(theta);

      vX = hit.x - radius*cos(deltaPhi);
      vY = hit.y - radius*sin(deltaPhi);

      evt->fVertexPos.setX( vX );
      evt->fVertexPos.setY( vY );
      evt->fVertexPos.setZ( vZ );
      // Throw new particle
      evt->ProduceNewParticle(
          G4ThreeVector(0, 0, 0),
          G4ThreeVector(hit.px, hit.py, hit.pz),
          particlename);
      number_of_particles++;
    }

  } while (number_of_particles == 0);
}

void remollGenBlackening::SampleElasticPhysics(remollEvent* evt)
{
  // Loop until we find at least one event with some particles
  int number_of_particles = 0;
  do {

    // Read next event from tree and increment
    if (fElasticEntry >= fElasticEntries)
      fElasticEntry = 0;
    fElasticTree->GetEntry(fElasticEntry++);

    // Weighting completely handled by event file
    evt->SetEffCrossSection(fElasticEvent->xs*microbarn);
    evt->SetQ2(fElasticEvent->Q2);
    evt->SetW2(fElasticEvent->W2);
    evt->SetAsymmetry(fElasticEvent->A*ppb);

    // Loop over all hits in this event
    for (size_t i = 0; i < fElasticHit->size(); i++) {
      // Create local copy of this hit
      remollGenericDetectorHit_t hit = fElasticHit->at(i);

      // Select only the requested detector ID and hit area
      if (hit.det != 28 || hit.pid != 11 || hit.r < fMinRad || hit.r > fMaxRad || atan2(hit.y,hit.x) < fMinPhi || atan2(hit.y,hit.x) > fMaxPhi) 
        continue;
      // Get particle name
      G4ParticleTable* particletable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particle = particletable->FindParticle(hit.pid);
      G4String particlename = particle->GetParticleName();

      double deltaPhi = CLHEP::RandFlat::shoot(-2,2);
      double vX, vY, vZ;
      double zOffset = 500;

      vZ = hit.z - zOffset;

      double theta = acos(hit.pz/hit.p);

      double radius = zOffset*tan(theta);

      vX = hit.x - radius*cos(deltaPhi);
      vY = hit.y - radius*sin(deltaPhi);

      evt->fVertexPos.setX( vX );
      evt->fVertexPos.setY( vY );
      evt->fVertexPos.setZ( vZ );
      // Throw new particle
      evt->ProduceNewParticle(
          G4ThreeVector(0, 0, 0),
          G4ThreeVector(hit.px, hit.py, hit.pz),
          particlename);
      number_of_particles++;
    }

  } while (number_of_particles == 0);
}

void remollGenBlackening::SampleInelasticPhysics(remollEvent* evt)
{
  // Loop until we find at least one event with some particles
  int number_of_particles = 0;
  do {

    // Read next event from tree and increment
    if (fInelasticEntry >= fInelasticEntries)
      fInelasticEntry = 0;
    fInelasticTree->GetEntry(fInelasticEntry++);

    // Weighting completely handled by event file
    evt->SetEffCrossSection(fInelasticEvent->xs*microbarn);
    evt->SetQ2(fInelasticEvent->Q2);
    evt->SetW2(fInelasticEvent->W2);
    evt->SetAsymmetry(fInelasticEvent->A*ppb);

    // Loop over all hits in this event
    for (size_t i = 0; i < fInelasticHit->size(); i++) {
      // Create local copy of this hit
      remollGenericDetectorHit_t hit = fInelasticHit->at(i);

      // Select only the requested detector ID and hit area
      if (hit.det != 28 || hit.pid != 11 || hit.r < fMinRad || hit.r > fMaxRad || atan2(hit.y,hit.x) < fMinPhi || atan2(hit.y,hit.x) > fMaxPhi) 
        continue;
      // Get particle name
      G4ParticleTable* particletable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particle = particletable->FindParticle(hit.pid);
      G4String particlename = particle->GetParticleName();

      double deltaPhi = CLHEP::RandFlat::shoot(-2,2);
      double vX, vY, vZ;
      double zOffset = 500;

      vZ = hit.z - zOffset;

      double theta = acos(hit.pz/hit.p);

      double radius = zOffset*tan(theta);

      vX = hit.x - radius*cos(deltaPhi);
      vY = hit.y - radius*sin(deltaPhi);

      evt->fVertexPos.setX( vX );
      evt->fVertexPos.setY( vY );
      evt->fVertexPos.setZ( vZ );
      // Throw new particle
      evt->ProduceNewParticle(
          G4ThreeVector(0, 0, 0),
          G4ThreeVector(hit.px, hit.py, hit.pz),
          particlename);
      number_of_particles++;
    }

  } while (number_of_particles == 0);
}
