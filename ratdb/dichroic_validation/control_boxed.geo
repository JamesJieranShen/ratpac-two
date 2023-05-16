// simplified geometry of benchtop setup used to measure transmission and reflection coefficients of dichroic filters

{
name: "GEO",
index: "world",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "",		// world volume has no mother
type: "box",
size: [1000.0, 500.0, 500.0],		// mm, half-length
material: "air",
}

{
name: "GEO",
index: "left",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",		// world volume has no mother
type: "box",
size: [490.0, 490.0, 490.0],		// mm, half-length
position: [-490.0, 0, 0],
material: "air",
// surface: "longpass_dichroic"
}

{
name: "GEO",
index: "PMT_Right",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",
type: "pmtarray",
pmt_model: "r1408",
pmt_detector_type: "idpmt",
sensitive_detector: "/mydet/pmt/inner",
efficiency_correction: 1.0, 
pos_table: "PMTINFO_RIGHT",		// table with positions of PMTs 
orientation: "point",		// aim all PMTs at a point
orient_point: [0.0, 0.0, 0.0],		//make that point the center 
}

{
name: "GEO",
index: "PMT_Left",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "left",
type: "pmtarray",
pmt_model: "r1408",
pmt_detector_type: "idpmt",
sensitive_detector: "/mydet/pmt/inner",
efficiency_correction: 1.0, 
pos_table: "PMTINFO_LEFT",		// table with positions of PMTs 
orientation: "point",		// aim all PMTs at a point
orient_point: [0.0, 0.0, 0.0],		//make that point the center 
}



//{
//name: "GEO",
//index: "filter",
//valid_begin: [0, 0],
//valid_end: [0, 0],
//mother: "world",
//type: "box",
//size: [20,1,20],
//material: "air",
//rotation: [0,0,-45],
//surface: "longpass_dichroic"
//}


