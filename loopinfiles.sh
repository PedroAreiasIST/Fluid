rm airfoilnames.txt
rm largefile.txt
for FILE in training/*.dat
do
    cp $FILE ./profile.dat
    out="$(basename $FILE)"
    echo $out >> airfoilnames.txt
    ./doall.sh
done
