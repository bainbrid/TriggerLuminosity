export PATH=$HOME/.local/bin:/cvmfs/cms-bril.cern.ch/brilconda/bin:$PATH

echo "LUMI CALC FOR GOLDEN JSON:"
eval 'brilcalc lumi --normtag jsons/Golden_JSONs/normtag_BRIL.json -i jsons/Golden_JSONs/Golden.json -u /fb'
