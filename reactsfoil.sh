readxfoilpolarsave.exe
./determinesclovercd.exe
paste parameters.txt clovercd.txt >> largefile.txt
awk '{gsub(" ",",",$0);print $0}' largefile.txt| sed "s/,$//g" >largefile.xls
