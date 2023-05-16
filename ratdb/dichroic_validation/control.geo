// simplified geometry of benchtop setup used to measure transmission and reflection coefficients of dichroic filters

{
name: "GEO",
index: "world",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "",		// world volume has no mother
type: "box",
size: [20000.0, 20000.0, 20000.0],		// mm, half-length
material: "air",
}

{
name: "GEO",
index: "PMT",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",
type: "pmtarray",
pmt_model: "r1408",
pmt_detector_type: "idpmt",
sensitive_detector: "/mydet/pmt/inner",
efficiency_correction: 1.0, 
pos_table: "PMTINFO",		// table with positions of PMTs 
orientation: "point",		// aim all PMTs at a point
orient_point: [0.0, 0.0, 0.0],		//make that point the center 
}

{
name: "GEO",
index: "filter",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",
type: "box",
size: [20,1,20],
material: "air",
rotation: [0,0,-45],
// surface: "longpass_dichroic"
}


