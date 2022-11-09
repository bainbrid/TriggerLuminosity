export PATH=$HOME/.local/bin:/cvmfs/cms-bril.cern.ch/brilconda/bin:$PATH

echo "LUMI CALC FOR GOLDEN JSON:"
eval 'brilcalc lumi --normtag jsons/current/normtag_BRIL.json -i jsons/current/Golden.json -u /fb'
