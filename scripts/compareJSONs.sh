echo "########## CHECK FOR OVERLAPS (I.E. 'AND') BETWEEN EACH JSON FILE... ##########"
IFILE1=0
for FILE1 in jsons/current/L1_*_HLT_*_golden.json; do
    IFILE2=0
    for FILE2 in jsons/current/L1_*_HLT_*_golden.json; do
	if [[ "$IFILE2" -gt "$IFILE1" ]]; then
	    echo "CHECKING: IFILE1: " $IFILE1 " FILE1: " $FILE1 "IFILE2: " $IFILE2 " FILE2: " $FILE2 " ..."
	    eval 'compareJSON.py --and $FILE1 $FILE2'
	fi
	IFILE2=$(( IFILE2 + 1 ))
    done
    IFILE1=$(( IFILE1 + 1 ))
done
