# These are the jsons ordered by threshold ....

FILES=(jsons/current/L1_*_HLT_*_Excl_Golden.json)
FILES=(`printf '%s\n' "${FILES[@]}"|sort -V`)

FILE_TOTAL="jsons/current/running_total.json"
rm $FILE_TOTAL # ensure starting from new empty file
for IDX in ${!FILES[@]}; do
    if [[ "$IDX" -eq 0 ]]; then
	cp ${FILES[$IDX]} $FILE_TOTAL
    else
	echo "index $IDX, file ${FILES[$IDX]} ..."
	eval 'compareJSON.py --or ${FILES[$IDX]} $FILE_TOTAL $FILE_TOTAL'
    fi
    FILE_OUTPUT=${FILES[$IDX]//'_Excl_Golden.json'/}'_Incl_Final.json'
    cp $FILE_TOTAL $FILE_OUTPUT
done
rm $FILE_TOTAL
