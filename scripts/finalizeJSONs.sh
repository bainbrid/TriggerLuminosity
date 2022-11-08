# These are the jsons, NOT necessarily ordered by threshold ....
files=(jsons/current/L1_*_HLT_*_golden.json)

for i in ${!files[@]}; do
    FILE_TOTAL=${files[$i]//'_golden.json'/}'_final.json'
    cp ${files[$i]} $FILE_TOTAL
    if [[ $i -gt 0 ]]; then
	echo "index $i, file1 ${files[$i]}, file2 ${files[$i-1]}"
	#eval 'compareJSON.py --or ${files[$i]} ${files[$i-1]}'
	echo "$FILE_TOTAL" 
	#eval 'compareJSON.py --sub ${files[$i]} ${files[$i-1]} ${files[$i]}_total'
    fi
done
