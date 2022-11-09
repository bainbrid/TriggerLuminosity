echo "########## COMBINE THE FOLLOWING JSONS INTO A SINGLE JSON ##########"
OUTPUT='jsons/current/trigger_OR.json'
IFILE=0
for FILE in jsons/current/L1_*_HLT_*_Excl_Golden.json; do
    if [[ "$IFILE" -eq 0 ]]; then
	cp $FILE $OUTPUT
    else
	echo $IFILE $FILE
	eval 'compareJSON.py --or $FILE $OUTPUT $OUTPUT'
    fi
    IFILE=$(( IFILE + 1 ))
done

echo "########## DETERMINE LUMINOSITY FOR THE COMBINED JSON ##########"
eval 'brilcalc lumi --normtag jsons/current/normtag_BRIL.json -i $OUTPUT -u /fb | tail -8 | head -6'
