/**********************************************************************
 * Filename:   DichroiconArrayFactory.cc
 * Author:     James Shen <jierans@sas.upenn.edu>
 * Date:       8/2/23 2:57 PM
 **********************************************************************/
#include <G4GDMLParser.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4Material.hh>
#include <G4PVPlacement.hh>
#include <RAT/DichroiconArrayFactory.hh>
#include <RAT/Log.hh>
#include <RAT/Materials.hh>

namespace RAT {
G4VPhysicalVolume *DichroiconArrayFactory::Construct(DBLinkPtr table) {
  std::string volume_name = table->GetS("index");
  std::string mother_name = table->GetS("mother");
  // setup position arrays
  std::string pos_table_name = table->GetS("pos_table");
  DBLinkPtr pos_table = DB::Get()->GetLink(pos_table_name);
  const std::vector<double> &pos_x = pos_table->GetDArray("x");
  const std::vector<double> &pos_y = pos_table->GetDArray("y");
  const std::vector<double> &pos_z = pos_table->GetDArray("z");
  std::vector<G4ThreeVector> pos(pos_x.size());
  for (int i = 0; i < pos_x.size(); i++) {
    pos[i] = G4ThreeVector(pos_x[i], pos_y[i], pos_z[i]);
  }
  // setup orientation arrays
  bool orient_manual = false;
  try {
    std::string orient_str = table->GetS("orientation");
    if (orient_str == "manual")
      orient_manual = true;
    else if (orient_str == "point")
      orient_manual = false;
    else
      Log::Die("Dichroicon Factory error: Unknown orientation " + orient_str);
  } catch (DBNotFoundError &e) {
  }
  std::vector<G4ThreeVector> dir(pos.size());
  if (!orient_manual) {
    const std::vector<double> &orient_point_array = table->GetDArray("orient_point");
    if (orient_point_array.size() != 3) Log::Die("Dichroicon Factory Error: orient_point must have 3 values");
    const G4ThreeVector orient_point(orient_point_array[0], orient_point_array[1], orient_point_array[2]);
    for (int i = 0; i < pos.size(); i++) {
      dir[i] = (orient_point - pos[i]).unit();
    }
  } else {
    const std::vector<double> &dir_x = pos_table->GetDArray("dir_x");
    const std::vector<double> &dir_y = pos_table->GetDArray("dir_y");
    const std::vector<double> &dir_z = pos_table->GetDArray("dir_z");
    Log::Assert(pos_x.size() == pos_y.size() && pos_x.size() == pos_z.size(),
                "Dichroicon position arrays must be the same length");
    Log::Assert(pos.size() == dir_x.size() && pos.size() == dir_y.size() && pos.size() == dir_z.size(),
                "Dichroicon direction arrays must be the same length");
    for (int i = 0; i < pos_x.size(); i++) {
      dir[i] = G4ThreeVector(dir_x[i], dir_y[i], dir_z[i]).unit();
    }
  }
  // compute positional offset
  double offset = 0;
  try {
    offset = table->GetD("offset");
  } catch (DBNotFoundError &e) {
  }
  for (int i = 0; i < pos.size(); i++) {
    pos[i] += offset * dir[i];
  }

  ConstructDichroicons(table, pos, dir);
  return nullptr;
}

void DichroiconArrayFactory::ConstructDichroicons(DBLinkPtr table, const std::vector<G4ThreeVector> &pos,
                                                const std::vector<G4ThreeVector> &dir) {
  Log::Assert(pos.size() == dir.size(), "Dichroicon position and direction arrays must be the same length");
  const std::string volumeName = table->GetIndex();
  const std::vector<std::string> base_volumes = table->GetSArray("base_volumes");
  const std::string motherName = table->GetS("mother");
  G4Material *filter_material = G4Material::GetMaterial(table->GetS("filter_material"));
  G4Material *base_material = G4Material::GetMaterial(table->GetS("base_material"));
  const std::string surface_name = table->GetS("surface");
  G4VPhysicalVolume *motherPhys = FindPhysMother(motherName);
  if (motherPhys == nullptr) Log::Die("Dichroicon mother physical volume not found: " + motherName);
  G4GDMLParser parser;
  const G4String gdml_file = std::getenv("RATSHARE") + std::string("/ratdb/") + table->GetS("gdml_file");
  //  parser.SetOverlapCheck(true);
  parser.Read(gdml_file);
  G4LogicalVolume *gdml_worldLV = parser.GetWorldVolume()->GetLogicalVolume();
  const size_t n_gdml_daughters = gdml_worldLV->GetNoDaughters();
  // loop through pos and dir
  for (int i_dichroicon = 0; i_dichroicon < pos.size(); i_dichroicon++) {
    // compute required rotation
    const G4ThreeVector &current_pmt_pos = pos[i_dichroicon];
    const G4ThreeVector &current_pmt_dir = dir[i_dichroicon];
    double angle_y = (-1.0) * atan2(current_pmt_dir.x(), current_pmt_dir.z());
    double angle_x = atan2(current_pmt_dir.y(),
                           sqrt(current_pmt_dir.x() * current_pmt_dir.x() + current_pmt_dir.z() * current_pmt_dir.z()));
    auto *rotation = new G4RotationMatrix();
    rotation->rotateY(angle_y);
    rotation->rotateX(angle_x);
    // loop through gdml daughters
    for (int i_component = 0; i_component < n_gdml_daughters; i_component++) {
      G4VPhysicalVolume *daughter = gdml_worldLV->GetDaughter(i_component);
      // create copy of daughter

      G4String name = daughter->GetName();
      G4LogicalVolume *daughter_lv = daughter->GetLogicalVolume();
      name = name.substr(3);
      G4String placed_name;
      bool is_base = std::find(base_volumes.begin(), base_volumes.end(), name) != base_volumes.end();
      if (is_base) {
        daughter_lv->SetMaterial(base_material);
        SetVis(daughter_lv, table->GetDArray("base_color"));
        placed_name = "dichroicon_base";
      } else {
        daughter_lv->SetMaterial(filter_material);
        SetVis(daughter_lv, table->GetDArray("filter_color"));
        new G4LogicalSkinSurface(name + "_surface", daughter_lv, Materials::optical_surface[surface_name]);
        placed_name = "dichroic_filter";
      }
      G4VPhysicalVolume *daughterPhys =
          new G4PVPlacement(rotation, current_pmt_pos, placed_name, daughter_lv, motherPhys, false, 0);
    }
  }
}

void DichroiconArrayFactory::SetVis(G4LogicalVolume *volume, const std::vector<double>& color) {
  G4VisAttributes *att = GetColor(color);
  //  att->SetForceSolid(true);
  volume->SetVisAttributes(att);
}

// This really should be available to all factories
G4VisAttributes *DichroiconArrayFactory::GetColor(const std::vector<double> &color) {
  if (color.size() == 4) {
    return new G4VisAttributes(G4Color(color[0], color[1], color[2], color[3]));
  } else if (color.size() == 3) {
    return new G4VisAttributes(G4Color(color[0], color[1], color[2]));
  } else {
    return new G4VisAttributes(G4Color());
  }
}

}  // namespace RAT