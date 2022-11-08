########################
# Run with python3 !!! #
########################

print("########## MERGE THE INDIVIDUAL JSON FILES PRODUCED BY ALL CRAB JOBS INTO A SINGLE JSON PER TRIGGER ##########")

import os
import glob
import json
import sys
base = os.environ.get('CMSSW_RELEASE_BASE')
arch = os.environ.get('SCRAM_ARCH')
path = base+'/bin/'+arch
print("Retrieving mergeJSON.py from",path)
sys.path.append(path)
from mergeJSON import *

#input='/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/TriggerLuminosity_2022Sep29/ParkingDoubleElectronLowMass0/crab_Run2022Dv1_part0/220930_093243/0000/'
#files = [ input+"output_{:.0f}.json".format(x) for x in range(1,17) ]

input='/eos/cms/store/group/phys_bphys/DiElectronX/test/trigger/'
#input+='TriggerLuminosity_2022Sep30/ParkingDoubleElectronLowMass*/crab_Run2022*_part*/220930_*/000*/output_*.json'
#input+='TriggerLuminosity_2022Oct02/ParkingDoubleElectronLowMass*/crab_Run2022*_part*/221002_*/000*/output_*.json'
input+='TriggerLuminosity_2022Nov07/ParkingDoubleElectronLowMass*/crab_Run2022*_part*/221107_*/000*/output_*.json'

files = [ name for name in glob.glob(input)]
#for filename in files: print(file)
print("len(files)",len(files))

dct = {}
for ifilename,filename in enumerate(files):
    print("Merging file \""+filename+"\" (number "+str(ifilename)+" out of "+str(len(files))+") ...")
    jsonFile = open(filename,'r')
    jsonDict = json.load(jsonFile)
    for (trigger,runs_lumis) in jsonDict.items():
        #if trigger != "": continue #@@
        lumi_list = LumiList(compactList=runs_lumis)
        #if "357900" not in lumi_list.getRuns(): continue #@@
        if trigger not in dct.keys(): dct[str(trigger)]  = lumi_list
        else:                         dct[str(trigger)] |= lumi_list
    jsonFile.close()
    del(jsonFile)

print()
print("FINAL MERGED JSON:")
print()

for idx,(trigger,lumiList) in enumerate(dct.items()): 
    print(trigger+":")
    print(str(lumiList))
    with open('jsons/current/{:s}.json'.format(trigger), 'w') as output: json.dump(lumiList.getCompactList(), output)


# Build list of trigger paths
#triggers = []
#for filename in files:
#    #print("filename:",filename)
#    jsonFile = open(filename,'r')
#    jsonDict = json.load(jsonFile)
#    #print("keys:",jsonDict.keys())
#    for entry in jsonDict.keys() :
#        if str(entry) not in triggers : triggers.append(str(entry))
#print("triggers:",triggers)
#print()

        #if trigger not in dct.keys(): dct[str(trigger)] = LumiList()
        #print("trigger:",trigger," runs_lumis:",runs_lumis)
        #runs_lumis_unpacked = {}
        #for run,lumis in runs_lumis.items(): runs_lumis_unpacked[str(run)] = [int(y) for x in lumis for y in range(x[0],x[1]+1)]

        #runs_lumis_unpacked = {}
        #for run,lumis in runs_lumis.items(): runs_lumis_unpacked[run] = [y for x in lumis for y in range(x[0],x[1]+1)]
        #localList = LumiList(runsAndLumis=runs_lumis_unpacked)


            #localList = LumiList(runsAndLumis=runs_lumis_unpacked)
            #print("localList:",localList)
            #print("dct[trigger] before:",dct[str(trigger)])
            #finalList = dct[str(trigger)]
            #finalList = finalList | lumi_list
            

#            localList = LumiList(runsAndLumis=runs_lumis_unpacked)
#            #print("localList:",localList)
#            #print("dct[trigger] before:",dct[str(trigger)])
#            finalList = dct[str(trigger)]
#            finalList = finalList | localList
#            dct[str(trigger)] = finalList
#
#            runs_lumis_unpacked = {}
#            for run,lumis in runs_lumis.items(): runs_lumis_unpacked[run] = [y for x in lumis for y in range(x[0],x[1]+1)]
#            localList = LumiList(runsAndLumis=runs_lumis_unpacked)
#            #print("localList:",localList)
#            #print("dct[trigger] before:",dct[str(trigger)])
#            finalList = dct[str(trigger)]
#            finalList = finalList | localList
#            dct[str(trigger)] = finalList

            #print("dct[trigger] after:",dct[str(trigger)])

#        for (run,lumis) in run_lumis.items():
#            if run not in dct[trigger].keys(): dct[str(trigger)][str(run)] = LumiList()
#            print("dct[trigger][run] before:",dct[str(trigger)][str(run)])
#            #filterRuns(localList, minRun, maxRun)
#            dct[str(trigger)][str(run)] |= localList
#            #print("dct[trigger] after:",dct[trigger])

#        if trigger not in dct.keys(): dct[str(trigger)] = {}
#        for (run,lumis) in run_lumis.items():
#            if run not in dct[trigger].keys(): dct[str(trigger)][str(run)] = LumiList()
#            print("dct[trigger][run] before:",dct[str(trigger)][str(run)])
#            localList = LumiList(runsAndLumis=lumis)
#            #filterRuns(localList, minRun, maxRun)
#            dct[str(trigger)][str(run)] |= localList
#            #print("dct[trigger] after:",dct[trigger])
