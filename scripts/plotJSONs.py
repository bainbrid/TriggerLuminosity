from array import array
import glob
import json
from FWCore.PythonUtilities.LumiList import LumiList
from ROOT import TH2F, TAxis, TCanvas, gROOT, kTRUE, gStyle, TColor, Int_t, Double_t
import numpy as np
gROOT.SetBatch(kTRUE)
gStyle.SetOptStat(0)

# Init
L1_thresholds = []
HLT_thresholds = []
runs = []
lumis = []
data = {}

# JSON files
names = [ name for name in glob.glob('jsons/current/L1_*_HLT_*_Excl_Final.json')]

# Two iterations through JSON files ...
# First: compile list of thresholds, runs, lumis
# Second: compile dict used to produce the 2D plot

for iname,name in enumerate(names):
    # Open file, extract LumiList
    jsonDict = None
    with open(name,'r') as input_file: jsonDict = json.load(input_file)
    localLumi = LumiList(compactList=jsonDict)
    # Store variables
    L1_threshold = float(name.split("L1_")[1].split("_HLT")[0].replace("p","."))
    L1_thresholds.append(L1_threshold)
    HLT_threshold = float(name.split("HLT_")[1].split("_Excl_Final.json")[0].replace("p","."))
    HLT_thresholds.append(HLT_threshold)
    runs.extend(list(localLumi.getRuns()))
    lumis.extend([ lumi for (run,lumi) in localLumi.getLumis()])
    for (run,lumi) in localLumi.getLumis(): 
        if int(run) not in data.keys(): data[int(run)] = {}
        if int(lumi) not in data[int(run)].keys() or L1_threshold < data[int(run)][int(lumi)]: data[int(run)][int(lumi)] = L1_threshold 
        #print(int(run),int(lumi),data[int(run)][int(lumi)])

# Identify unique values and sort
L1_thresholds = sorted(list(set(L1_thresholds)))
HLT_thresholds = sorted(list(set(HLT_thresholds)))
runs = sorted(list(set(runs)))
lumis = sorted(list(set(lumis)))
print("L1_thresholds:",L1_thresholds)
print("HLT_thresholds:",HLT_thresholds)
print("runs:",runs)
print("lumis:",lumis)
#print("data:",data)

# Create histogram
histo = TH2F(
    "histo",
    "",
    lumis[-1],1.,lumis[-1]+1., # x-axis: lumis
    len(runs),0.,len(runs)*1.  # y-axis: runs
)

# Populate histogram
for irun,run in enumerate(runs):
    for ilumi,lumi in enumerate(lumis):
        value = data[int(run)][int(lumi)] if int(run) in list(data.keys()) and int(lumi) in list(data[int(run)].keys()) else 0.
        histo.SetBinContent(ilumi+1,irun+1,value)
        #print(irun,run,ilumi,lumi,value,histo.GetBinContent(ilumi+1,irun+1))
for irun,run in enumerate(runs): histo.GetYaxis().SetBinLabel(irun+1,run)

# Define colour palette
# https://ultrahigh.org/2007/08/making-pretty-root-color-palettes/
def set_palette(name="default", ncontours=999):
    """Set a color palette from a given RGB list
    stops, red, green and blue should all be lists of the same length
    see set_decent_colors for an example"""
    if name == "gray" or name == "grayscale":
        stops = [0.00, 0.34, 0.61, 0.84, 1.00]
        red   = [1.00, 0.84, 0.61, 0.34, 0.00]
        green = [1.00, 0.84, 0.61, 0.34, 0.00]
        blue  = [1.00, 0.84, 0.61, 0.34, 0.00]
    # elif name == "whatever":
        # (define more palettes)
    else:
        # default palette, looks cool
        #stops = [0.00, 0.34, 0.61, 0.84, 1.00]
        #red   = [0.00, 0.00, 0.87, 1.00, 0.51]
        #green = [0.00, 0.81, 1.00, 0.20, 0.00]
        #blue  = [0.51, 1.00, 0.12, 0.00, 0.00]
        # green, yellow, orange, red
        #stops = [0.00, 0.20, 0.80, 1.00] 
        #red   = [0.00, 1.00, 0.80, 1.00]
        #green = [0.80, 1.00, 0.20, 0.00]
        #blue  = [0.00, 0.00, 0.00, 0.00]
        # yellow, green, blue
        #stops = [0.00, 0.50, 1.00] 
        #red   = [0.00, 0.00, 0.00]
        #green = [0.80, 0.40, 0.00]
        #blue  = [0.00, 0.40, 0.80]
        # yellow, orange, red, dark red
        stops = [0.00, 0.25, 0.50, 0.51, 1.00] 
        red   = [1.00, 1.00, 1.00, 0.90, 0.10]
        green = [1.00, 0.80, 0.00, 0.00, 0.00]
        blue  = [0.00, 0.00, 0.00, 0.00, 0.00]
    s = array('d', stops)
    r = array('d', red)
    g = array('d', green)
    b = array('d', blue)
    npoints = len(s)
    TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
    gStyle.SetNumberContours(ncontours)
# Set number of stops
set_palette(ncontours=12)

#gStyle.SetPalette() #

# Set z axis range
histo.SetMinimum(5.5)
histo.SetMaximum(11.5)

# Draw / save histogram
c1 = TCanvas("canvas","",600,1200)
histo.Draw("colz")
c1.SaveAs("output.pdf")
