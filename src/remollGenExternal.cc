/*
 * remollGenExternal.cc
 *
 *  Created on: Mar 17, 2017
 *      Author: wdconinc
 */

#include "remollGenExternal.hh"

// Geant4 headers
#include "G4ParticleTable.hh"
#include "G4GenericMessenger.hh"

// ROOT headers
#include "TFile.h"
#include "TTree.h"

#include "remollEvent.hh"
#include "remollVertex.hh"
#include "remolltypes.hh"

remollGenExternal::remollGenExternal()
: remollVEventGen("external"),
  fFile(0), fTree(0),
  fEntry(0), fEntries(0),
  fEvent(0), fHit(0),
  fDetectorID(0)
{
  // Add to generic messenger
  fThisGenMessenger->DeclareMethod("file",&remollGenExternal::SetGenExternalFile,"External generator event filename");
  fThisGenMessenger->DeclareMethod("detid",&remollGenExternal::SetGenExternalDetID,"External generator detector ID");
}

remollGenExternal::~remollGenExternal()
{
  // Close file which deletes tree
  if (fFile) {
    fFile->Close();
    fTree = 0;
  }
}

void remollGenExternal::SetGenExternalFile(G4String& filename)
{
  // Close previous file
  if (fFile) {
    fFile->Close();
    fFile = 0;
  }

  // Try to open filename
  fFile = new TFile(filename);
  if (! fFile) {
    G4cerr << "Could not open external event file " << filename << G4endl;
    return;
  }

  // Try to find tree in file
  fFile->GetObject("T",fTree);
  if (! fTree) {
    G4cerr << "Could not find tree T in event file " << filename << G4endl;
    return;
  }

  // Get number of entries
  fEntries = fTree->GetEntries();

  // Initialize tree
  if (fTree->GetBranch("hit")) {
    fTree->SetBranchAddress("hit", &fHit);
  } else {
    G4cerr << "Could not find branch hit in event file " << filename << G4endl;
    return;
  }
  if (fTree->GetBranch("ev")) {
    fTree->SetBranchAddress("ev", &fEvent);
  } else {
    G4cerr << "Could not find branch ev in event file " << filename << G4endl;
    return;
  }
}

void remollGenExternal::SamplePhysics(remollVertex *vert, remollEvent *evt)
{
  // Check whether three exists
  if (! fTree) {
    G4cerr << "Could not find tree T in event file" << G4endl;
    return;
  }

  // Loop until we find at least one event with some particles
  int number_of_particles = 0;
  do {

    // Read next event from tree and increment
    fTree->GetEntry(fEntry++);
    // Keep simulating the last event
    if (fEntry >= fEntries) {
      fEntry--;
      G4cerr << "Reached last event and will keep simulating it..." << G4endl;
    }

    // Weighting completely handled by event file
    evt->SetEffCrossSection(fEvent->xs*microbarn);
    evt->SetQ2(fEvent->Q2);
    evt->SetW2(fEvent->W2);
    evt->SetAsymmetry(fEvent->A*ppb);

    // Loop over all hits in this event
    for (size_t i = 0; i < fHit->size(); i++) {
      // Create local copy of this hit
      remollGenericDetectorHit_t hit = fHit->at(i);

      // Select only the requested detector ID
      if (hit.det != fDetectorID) continue;

      // Get particle name
      G4ParticleTable* particletable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particle = particletable->FindParticle(hit.pid);
      G4String particlename = particle->GetParticleName();

      // Throw new particle
      evt->ProduceNewParticle(
          G4ThreeVector(hit.x, hit.y, hit.z),
          G4ThreeVector(hit.px, hit.py, hit.pz),
          particlename);
      number_of_particles++;
    }

  } while (number_of_particles == 0);

}
