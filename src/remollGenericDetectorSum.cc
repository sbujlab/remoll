#include "remollGenericDetectorSum.hh"

G4ThreadLocal G4Allocator<remollGenericDetectorSum>* remollGenericDetectorSumAllocator = 0;

remollGenericDetectorSum::remollGenericDetectorSum(int detid, int copyid)
: fDetID(detid),fCopyID(copyid) {
  fEdep   = 0.0;
}

remollGenericDetectorSum::~remollGenericDetectorSum() { }

remollGenericDetectorSum::remollGenericDetectorSum(const remollGenericDetectorSum &right)
: G4VHit(right) {
  // copy constructor
  fDetID  = right.fDetID;
  fCopyID = right.fCopyID;
  fEdep   = right.fEdep;
}

const remollGenericDetectorSum& remollGenericDetectorSum::operator=(const remollGenericDetectorSum &right){
  (*this) = right;
  return *this;
}

G4int remollGenericDetectorSum::operator==(const remollGenericDetectorSum &right) const {
  return (this==&right) ? 1 : 0;
}
