rm airfoilnamesvoid.txt
for FILE in training/*.dat
do
    cp $FILE ./profile.dat
    out="$(basename $FILE)"
    echo $out >> airfoilnamesvoid.txt
    ./doallvoid.sh
done
