#ifndef __REMOLLGENBEAM_HH 
#define __REMOLLGENBEAM_HH 
/*!
 * Boring beam event generator
 *
 * Seamus Riordan
 * July 9, 2013
 *
*/

#include "remollVEventGen.hh"

class remollGenBeam : public remollVEventGen {
    public:
	remollGenBeam();
	virtual ~remollGenBeam();
    void SetHitX(double x);
    void SetHitY(double y);
    void SetHitZ(double z);

    void SetTheta(double theta);
    void SetDeltaPhi(double dphi);

    void SetZOffset(double zOff);

    void SetPartName(G4String& name);

    private:
	void SamplePhysics(remollVertex *, remollEvent *);

	double fXhitPos;
	double fYhitPos;
	double fZhitPos;

    double fTheta;
    double fDeltaPhi;

    double fZoffset;

    G4String fParticleName;
};

#endif//__REMOLLGENBEAM_HH 
