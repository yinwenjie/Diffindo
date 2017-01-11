mkdir gcc_flv

cp -r -f ./../FLVParserLib/src gcc_flv
cp -f ./FLVParser.cpp ./gcc_flv/src
cp -f ./FLVExtractor.cpp ./gcc_flv/src
cp -f ./FLVAccelerator.cpp ./gcc_flv/src

cd gcc_flv/src

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./FLVParser.cpp -o ./../FLVParser

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./FLVExtractor.cpp -o ./../FLVExtractor

g++ -std=c++11 ./AudioTag.cpp ./VideoTag.cpp ./FLVTag.cpp ./FLVBody.cpp ./GlobalUtils.cpp ./FLVParserLib.cpp ./FLVAccelerator.cpp -o ./../FLVAccelerator