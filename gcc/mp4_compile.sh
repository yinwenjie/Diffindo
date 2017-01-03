mkdir gcc_mp4

cp -r -f ./../MP4ParserLib/src gcc_mp4
cp -f ./MP4Parser.cpp ./gcc_mp4/src

cd gcc_mp4/src

g++ -std=c++11 ./Box.cpp ./MP4File.cpp ./GlobalUtils.cpp ./MP4ParserLib.cpp ./MP4Parser.cpp -o ./../MP4Parser