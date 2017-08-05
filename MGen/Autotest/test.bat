appveyor AddMessage "Test started"

cd ..
copy "..\x64\Release\MGen.exe" .
MGen.exe -test configs\GenCA1\Test.pl

appveyor AddMessage "Test finished"
