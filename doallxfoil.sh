rm *polar*.txt
rm *dump*.txt
cstparam.exe
xfoil < xfoilinput.txt
./reactsfoil.sh
