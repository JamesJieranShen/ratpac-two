import ROOT
import rat
import pickle
import sys
import numpy as np
import glob

def readFile(inFname, outFname):
    print("-"*50)
    print(f"{inFname} => {outFname}")
    photons = []

    for ievt, ds in enumerate(rat.dsreader(inFname)):
        if ievt % 10 == 0:
            print(f"Reading {ievt}th Event")
        mc = ds.GetMC()
        nPMTs = mc.GetMCPMTCount()
        for iPmt in range(nPMTs):
            mcpmt = mc.GetMCPMT(iPmt)
            photonCount = mcpmt.GetMCPhotonCount()
            for iPhoton in range(photonCount):
                wvl = mcpmt.GetMCPhoton(iPhoton).GetLambda() * 1e6 # mm->nm
                photons.append(wvl)

    photons = np.array(photons)
    print("Total Number of Photons: ", len(photons))
    with open(outFname, 'wb') as f:
        pickle.dump(photons, f)

if __name__ == "__main__":
    rootFiles = glob.glob('data/*.root')
    for rootFile in rootFiles:
        pklFile = rootFile.replace('.root', '.pkl')
        readFile(rootFile, pklFile)



