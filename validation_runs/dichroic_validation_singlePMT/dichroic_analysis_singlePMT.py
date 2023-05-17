import ROOT
import rat
import pickle
import sys
import numpy as np
import glob
import multiprocessing

def readFile(inFname):
    assert inFname.endswith('.root')
    outFname = inFname.replace('.root', '.pkl')
    print("-"*50)
    print(f"{inFname} => {outFname}", flush=True)
    photons = []

    for ievt, ds in enumerate(rat.dsreader(inFname)):
        # if ievt % 10 == 0:
        #     print(f"Reading {ievt}th Event", flush=True)
        mc = ds.GetMC()
        nPMTs = mc.GetMCPMTCount()
        for iPmt in range(nPMTs):
            mcpmt = mc.GetMCPMT(iPmt)
            photonCount = mcpmt.GetMCPhotonCount()
            for iPhoton in range(photonCount):
                wvl = mcpmt.GetMCPhoton(iPhoton).GetLambda() * 1e6 # mm->nm
                photons.append(wvl)

    photons = np.array(photons)
    print("Total Number of Photons: ", len(photons), flush=True)
    with open(outFname, 'wb') as f:
        pickle.dump(photons, f)

if __name__ == "__main__":
    rootFiles = glob.glob('data/*.root')
    with multiprocessing.Pool(processes=multiprocessing.cpu_count() - 2) as pool:
        pool.map(readFile, rootFiles)
    
    



