appveyor AddMessage "Test started"

cd ..
copy "..\x64\Release\MGen.exe" .
MGen.exe -test configs\GenRS1\Test.pl
MGen.exe -test configs\GenCF1\Test.pl
MGen.exe -test configs\GenCA1\Test.pl
MGen.exe -test configs\GenCP1\Test.pl
MGen.exe -test configs\GenCA2\Test.pl
MGen.exe -test configs\GenMP1\Test.pl

appveyor AddMessage "Test finished"
