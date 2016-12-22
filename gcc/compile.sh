mkdir gcc_workspace

cp -r -f ./../FLVParserLib/src  gcc_workspace
cp -f ./Parser.cpp ./gcc_workspace/src
cp -f ./Extractor.cpp ./gcc_workspace/src
cp -f ./FLVAccelerator.cpp ./gcc_workspace/src

cd gcc_workspace/src

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./Parser.cpp -o ./../FLVParser

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./Extractor.cpp -o ./../FLVExtractor

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./FLVAccelerator.cpp -o ./../FLVAccelerator