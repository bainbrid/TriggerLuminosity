export PATH=$HOME/.local/bin:/cvmfs/cms-bril.cern.ch/brilconda/bin:$PATH
for FILE in jsons/current/L1_*_HLT_*_Excl_Final.json; do
#for FILE in jsons/current/L1_*_HLT_*_Incl_Final.json; do
echo $FILE
#eval 'brilcalc lumi --normtag jsons/Golden_JSONs/normtag_BRIL.json -i $FILE -u /fb | tail -7 | head -5'
eval 'brilcalc lumi --normtag jsons/Golden_JSONs/normtag_BRIL.json -i $FILE -u /fb | tail -4 | head -1'
done
