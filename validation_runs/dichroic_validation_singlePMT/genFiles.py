## Generate necessary files to do multiple AOI Runs
import numpy as np
from textwrap import dedent
import argparse
import os

AOI_deg = np.array([15, 30, 45, 60, 75])

def generate_pmtinfo_entry(name, pmt_x, pmt_y):
    return dedent(f"""\
    {{
        name: "{name}",
        valid_begin : [0, 0],
        valid_end : [0, 0],

        x: [{pmt_x},], 
        y: [{pmt_y},],
        z: [0,],
    }}
    """)

def generate_pmt_position_table(fname):
    print(f"Generating PMT Position Table and writing to {os.path.abspath(fname)}")
    pmtDistance = 800.0 # mm
    with open(fname, 'w') as f:
        for aoi_deg in AOI_deg:
            aoi_rad = np.deg2rad(aoi_deg)
            pmt_x = pmtDistance * np.cos(aoi_rad)
            pmt_y = -pmtDistance * np.sin(aoi_rad)
            pmtinfo_entry = generate_pmtinfo_entry(f"PMTINFO_{aoi_deg}DEG", pmt_x, pmt_y)
            f.write(pmtinfo_entry)
            f.write("\n\n")

def generate_macros():
    print(f"Generating Macros in the local directory, as well as the necessary subdirectories")
    if not os.path.exists("macros"):
        os.makedirs("macros")
        print("Created macros directory since it doesn't exist")
    if not os.path.exists("data"):
        os.makedirs("data")
        print("Created data directory since it doesn't exist")
    macro_template = dedent("""\
        /glg4debug/glg4param omit_muon_processes 1.0
        /glg4debug/glg4param omit_hadronic_processes 1.0

        /rat/db/set DETECTOR experiment "dichroic_validation"
        /rat/db/set DETECTOR geo_file "dichroic_validation/longpass_boxed.geo"
        /rat/db/set GEO[PMT] pos_table "{0}"

        /run/initialize

        /rat/proc count
        /rat/procset update 100

        /rat/proclast outroot
        /rat/procset file "{1}"
        /tracking/storeTrajectory 1

        /generator/add combo gun2:point
        /generator/vtx/set opticalphoton {2} {3} 0 0 1.54875e-06 6.195e-06 0 0 0 10000
        /generator/pos/set {4} {5} 0

        /generator/rate/set 100
        /run/beamOn 1000
    """)
    for aoi_deg in AOI_deg:
        # Parameters are pmtinfo_entry_name, output_fname, mom_x, mom_y, vtx_x, vtx_y
        # The vertex is always 1mm away from the origin
        pmtinfo_entry_name = f"PMTINFO_{aoi_deg}DEG"
        aoi_rad = np.deg2rad(aoi_deg)
        ## Transmission
        output_fname = f"data/transmission_{aoi_deg}deg.root"
        mom_x = np.cos(aoi_rad)
        mom_y = -np.sin(aoi_rad)
        vtx_x = -np.cos(aoi_rad)
        vtx_y = np.sin(aoi_rad)
        macro_t = macro_template.format(pmtinfo_entry_name, 
                                        output_fname, 
                                        mom_x, mom_y, 
                                        vtx_x, vtx_y)
        with open(f"macros/transmission_{aoi_deg}deg.mac", 'w') as f:
            print(f"Writing to macros/transmission_{aoi_deg}deg.mac")
            f.write(macro_t)
        
        ## Reflection
        output_fname = f"data/reflection_{aoi_deg}deg.root"
        mom_x = -np.cos(aoi_rad)
        mom_y = -np.sin(aoi_rad)
        vtx_x = np.cos(aoi_rad)
        vtx_y = np.sin(aoi_rad)
        macro_r = macro_template.format(pmtinfo_entry_name,
                                        output_fname,
                                        mom_x, mom_y,
                                        vtx_x, vtx_y)
        with open(f"macros/reflection_{aoi_deg}deg.mac", 'w') as f:
            print(f"Writing to macros/reflection_{aoi_deg}deg.mac")
            f.write(macro_r)

        ## Normalization
        output_fname = f"data/normalization_{aoi_deg}deg.root"
        mom_x = np.cos(aoi_rad)
        mom_y = -np.sin(aoi_rad)
        vtx_x = np.cos(aoi_rad)
        vtx_y = -np.sin(aoi_rad)
        macro_n = macro_template.format(pmtinfo_entry_name,
                                        output_fname,
                                        mom_x, mom_y,
                                        vtx_x, vtx_y)
        with open(f"macros/normalization_{aoi_deg}deg.mac", 'w') as f:
            print(f"Writing to macros/normalization_{aoi_deg}deg.mac")
            f.write(macro_n)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate necessary files to do multiple AOI Runs")
    parser.add_argument("--pmtinfo", action="store_true", help="Generate PMT Position Table")
    parser.add_argument("--macros", action="store_true", help="Generate Macros")
    args = parser.parse_args()
    print("Generating AOI Runs for the following angles:")
    print(AOI_deg)
    if args.pmtinfo:
        generate_pmt_position_table("../../ratdb/dichroic_validation/PMTINFO_AOI.ratdb")
    if args.macros:
        generate_macros()
    if not args.pmtinfo and not args.macros:
        print("No arguments passed, doing nothing")