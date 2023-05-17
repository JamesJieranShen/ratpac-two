// simplified geometry of benchtop setup used to measure transmission and reflection coefficients of dichroic filters

{
name: "GEO",
index: "world",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "",		// world volume has no mother
type: "box",
size: [2000.0, 1000.0, 1000.0],		// mm, half-length
material: "air",
}

{
name: "GEO",
index: "coatedBox",
valid_begin: [0, 0],
valid_end: [0, 0],
mother: "world",		// world volume has no mother
type: "box",
size: [1000.0, 1000.0, 1000.0],		// mm, half-length
position: [-1000.0, 0, 0],
material: "air",
surface: "shortpass_dichroic"
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
pos_table: "PMTINFO_RIGHT",		// table with positions of PMTs 
orientation: "point",		// aim all PMTs at a point
orient_point: [0.0, 0.0, 0.0],		//make that point the center 
}


