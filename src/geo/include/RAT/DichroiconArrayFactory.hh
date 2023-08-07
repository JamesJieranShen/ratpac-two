/**********************************************************************
* Filename:   DichroiconArrayFactory.hh
* Author:     James Shen <jierans@sas.upenn.edu>
* Date:       8/2/23 2:57 PM
**********************************************************************/

#ifndef __RAT_DichroiconArrayFactory__
#define __RAT_DichroiconArrayFactory__

#include <RAT/GeoFactory.hh>
#include <G4VisAttributes.hh>

namespace RAT {

class DichroiconArrayFactory : public GeoFactory{
 public:
  DichroiconArrayFactory() : GeoFactory("DichroiconArray"){};
  void ConstructDichroicons(DBLinkPtr table,
                            const std::vector<G4ThreeVector> &pos,
                            const std::vector<G4ThreeVector> &dir
  );
  G4VPhysicalVolume *Construct(DBLinkPtr table) override;


 protected:
  static G4VisAttributes *GetColor(const std::vector<double>& color);
  static void SetVis(G4LogicalVolume *volume, const std::vector<double>& color);
};

}  // namespace RAT

#endif  // __RAT_DichroiconArrayFactory__
