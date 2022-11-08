echo "########## THE FOLLOWING RUNS AND LUMI SECTIONS DO NOT APPEAR IN THE (manually) filtered GOLDEN JSON ##########"
for FILE in jsons/current/L1_*_HLT_*.json; do
echo $FILE
eval 'compareJSON.py --sub $FILE jsons/current/Cert_Collisions2022_355100_361091_Golden_filtered.json'
done

echo "########## NOW TAKING THE 'AND' OF EACH JSON AND THE (manually) filtered GOLDEN JSON AND WRITING TO '<file>_golden.json' ... ##########"
for FILE in jsons/current/L1_*_HLT_*.json; do
FILE_GOLDEN=${FILE//'.json'/}'_golden.json'
echo $FILE_GOLDEN
eval 'compareJSON.py --and $FILE jsons/current/Cert_Collisions2022_355100_361091_Golden_filtered.json ${FILE_GOLDEN}'
done
