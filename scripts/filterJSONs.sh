echo "########## THE FOLLOWING RUNS AND LUMI SECTIONS DO NOT APPEAR IN THE (manually) filtered GOLDEN JSON ##########"
for FILE in jsons/current/L1_*_HLT_*_Excl.json; do
echo $FILE
eval 'compareJSON.py --sub $FILE jsons/Golden_JSONs/Golden.json'
done

echo "########## NOW TAKING THE 'AND' OF EACH JSON AND THE (manually) filtered GOLDEN JSON AND WRITING TO '<file>_golden.json' ... ##########"
for FILE in jsons/current/L1_*_HLT_*_Excl.json; do
FILE_GOLDEN=${FILE//'.json'/}'_Golden.json'
echo $FILE_GOLDEN
eval 'compareJSON.py --and $FILE jsons/Golden_JSONs/Golden.json ${FILE_GOLDEN}'
done
