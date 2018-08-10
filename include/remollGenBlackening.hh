/*
 * remollGenBlackening.hh
 *
 *  Created on: July 25th, 2018
 *      Author: Audrey Farrell
 */

#ifndef __REMOLLGENBLACKENING_HH
#define __REMOLLGENBLACKENING_HH

/*!
 * Generator specifically designed for quartz light guide blackening studies.
 *
 * This event generator reads events from moller, elastic, and inelastic
 * generator output files (ROOT) and randomly samples events from all three files.
 * Includes the ability to place hard cuts on hit radius and phi for control in 
 * blackening studies
 *
*/

// Base class headers
#include "remollVEventGen.hh"

// System headers
#include <vector>

#include "G4AutoLock.hh"
#include "Randomize.hh"

// Forward declarations
class TFile;
class TTree;
struct remollEvent_t;
struct remollGenericDetectorHit_t;

// Class definition
class remollGenBlackening : public remollVEventGen {

    public:
        remollGenBlackening();
        virtual ~remollGenBlackening();

        void InitializeMollerVariables();
        void InitializeElasticVariables();
        void InitializeInelasticVariables();
        void SetMinRadius(G4double min);
        void SetMaxRadius(G4double max);
        void SetMinPhi(G4double min);
        void SetMaxPhi(G4double max);
        void SetSeptantFolding(G4bool sept);
    private:
        //Bounds on radius and phi
        G4double fMinRad, fMaxRad;
        G4double fMinPhi, fMaxPhi;
        
        
        void SamplePhysics(remollVertex *, remollEvent *);
        void SampleMollerPhysics(remollEvent *);
        void SampleElasticPhysics(remollEvent *);
        void SampleInelasticPhysics(remollEvent *);
        // External event file and tree, entry number
        TFile* fMollerFile;
        TFile* fElasticFile;
        TFile* fInelasticFile;
        
        TTree* fMollerTree;
        TTree* fElasticTree;
        TTree* fInelasticTree;
        
        Double_t fMollerRate;
        Double_t fElasticRate;
        Double_t fInelasticRate;
        
        Int_t fMollerEntry, fMollerEntries;
        Int_t fElasticEntry, fElasticEntries;
        Int_t fInelasticEntry, fInelasticEntries;
        
        G4bool fSeptFold;

        // Event and hit structures
        remollEvent_t* fMollerEvent;
        std::vector<remollGenericDetectorHit_t>* fMollerHit;
        remollEvent_t* fElasticEvent;
        std::vector<remollGenericDetectorHit_t>* fElasticHit;
        remollEvent_t* fInelasticEvent;
        std::vector<remollGenericDetectorHit_t>* fInelasticHit;
};

#endif //__REMOLLGENBLACKENING_HH
