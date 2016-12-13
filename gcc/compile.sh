mkdir gcc_workspace

cp -r -f ./../FLVParserLib/src  gcc_workspace
cd gcc_workspace

#g++ ./src/GlobalUtils.cpp ./src/FLVParserLib.cpp  -fPIC -shared -o libFLVParser.so
g++ ./src/GlobalUtils.cpp ./src/FLVParserLib.cpp -shared -o libFLVParser.so