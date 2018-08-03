#include "remollGenBeam.hh"

#include "remollEvent.hh"
#include "remollVertex.hh"
#include "remollBeamTarget.hh"

#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PhysicalConstants.hh"
#include "G4GenericMessenger.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "remolltypes.hh"

#include <math.h>

remollGenBeam::remollGenBeam()
: remollVEventGen("beam"),
    fXhitPos(0.0), fYhitPos(0.0),
    fZhitPos(0.0), fZoffset(-500),
    fTheta(0), fDeltaPhi(0),
    fParticleName("e-")
{
    fApplyMultScatt = true;
    
    fThisGenMessenger->DeclareMethod("hitx",&remollGenBeam::SetHitX,"x coordinate of hit position for the beam");
    fThisGenMessenger->DeclareMethod("hity",&remollGenBeam::SetHitY,"y coordinate of hit position for the beam");
    fThisGenMessenger->DeclareMethod("hitz",&remollGenBeam::SetHitZ,"z coordinate of hit position for the beam");
    
    fThisGenMessenger->DeclareMethod("theta",&remollGenBeam::SetTheta,"angle of beam with respect to z-axis");
    fThisGenMessenger->DeclareMethod("zOffset", &remollGenBeam::SetZOffset,"distance from particle origin to hit in z-direction");
    
    fThisGenMessenger->DeclareMethod("partName",&remollGenBeam::SetPartName,"name of particle to shoot");
}

remollGenBeam::~remollGenBeam() { }

void remollGenBeam::SetHitX(double x){ fXhitPos = x; }
void remollGenBeam::SetHitY(double y){ fYhitPos = y; }
void remollGenBeam::SetHitZ(double z){ fZhitPos = z; }

void remollGenBeam::SetZOffset(double zOff){ fZoffset = zOff; } 

void remollGenBeam::SetTheta(double theta){ fTheta = theta; }
void remollGenBeam::SetDeltaPhi(double dphi){ fDeltaPhi = dphi; }

void remollGenBeam::SetPolarizationX(double sx){ fXPolarization = sx; }
void remollGenBeam::SetPolarizationY(double sy){ fYPolarization = sy; }
void remollGenBeam::SetPolarizationZ(double sz){ fZPolarization = sz; }

void remollGenBeam::SetPartName(G4String& name){ fParticleName = name; }

void remollGenBeam::SamplePhysics(remollVertex * /*vert*/, remollEvent *evt)
{
    double vX, vY, vZ;
    double pX, pY, pZ;

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle(fParticleName);

    // Get initial beam energy instead of using other sampling
    double E = fBeamTarg->fBeamEnergy;
    double m = particle->GetPDGMass();
    double p = sqrt(E*E - m*m);
    

    
    pX = cos(fDeltaPhi)*sin(fTheta)*p;
    pY = sin(fDeltaPhi)*sin(fTheta)*p;
    pZ = cos(fTheta)*p;
    
    evt->fBeamMomentum = p*G4ThreeVector(pX, pY, pZ);
    evt->fBeamE = E;

    // Calculate origin position based on hit coordinates and angles.
    
    vZ = fZhitPos - fZoffset;

    double radius = fZoffset*tan(fTheta);
    
    vX = fXhitPos - radius*cos(fDeltaPhi);
    vY = fYhitPos - radius*sin(fDeltaPhi);
    
    // Override target sampling z
    evt->fVertexPos.setX( vX );
    evt->fVertexPos.setY( vY );
    evt->fVertexPos.setZ( vZ );

    evt->ProduceNewParticle( G4ThreeVector(0.0, 0.0, 0.0), 
	    evt->fBeamMomentum, 
	    fParticleName,
            evt->fBeamPolarization);

    evt->SetEffCrossSection(0.0);
    evt->SetAsymmetry(0.0);

    evt->SetQ2(0.0);
    evt->SetW2(0.0);
}
