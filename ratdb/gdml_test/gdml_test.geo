// Simplified SNO detector geometry

{
name: "GEO",
index: "world",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "", // world volume has no mother
type: "box",
size: [20000.0, 20000.0, 20000.0], // mm, half-height
material: "water",
color: [0.67, 0.29, 0.0],
invisible: 1,
}

{
name: "GEO",
index: "dichroicons",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",
type: "DichroiconArray",
gdml_file: "gdml/dichroicon_tessellated.gdml",
filter_material: "air",
base_material:   "acrylic_black",
surface: "longpass_dichroic",
filter_color: [0.67, 0.29, 0.0, 0.2],
base_color: [0.4, 0.4, 0.4, 1.0],
base_volumes: ["DICHROICON_BASE"],

pos_table: "PMTINFO_sides",
orientation: "manual",
offset: -500,
}


