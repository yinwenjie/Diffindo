mkdir gcc_workspace

cp -r -f ./../FLVParserLib/src  gcc_workspace
cp -f ./ConsoleTest.cpp ./gcc_workspace/src

cd gcc_workspace/src

g++ -std=c++11 ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./ConsoleTest.cpp -o ./../FLVParser