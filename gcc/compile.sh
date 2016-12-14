mkdir gcc_workspace

cp -r -f ./../FLVParserLib/src  gcc_workspace
cp -f ./ConsoleTest.cpp ./gcc_workspace/src

cd gcc_workspace/src

g++ ./GlobalUtils.cpp ./FLVParserLib.cpp ./ConsoleTest.cpp -o ./../FLVParser