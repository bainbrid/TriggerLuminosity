echo "########## COMBINE THE FOLLOWING JSONS INTO A SINGLE JSON ##########"
OUTPUT='jsons/current/trigger_OR.json'
IFILE=0
for FILE in jsons/current/L1_*_HLT_*_golden.json; do
    if [[ "$IFILE" -eq 0 ]]; then
	cp $FILE $OUTPUT
    fi
    echo $IFILE $FILE
    eval 'compareJSON.py --or $FILE $OUTPUT $OUTPUT'
    IFILE=$(( IFILE + 1 ))
done

echo "########## DETERMINE LUMINOSITY FOR THE COMBINED JSON ##########"
eval 'brilcalc lumi --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_BRIL.json -i $OUTPUT -u /fb | tail -8 | head -6'
