#FLV ToolKit version Alpha 0.1

# Compiler：
- Win32: gcc version 5.1.0 (tdm64-1)
- OS X: 

# Components:

- FLVParser: 解析FLV文件信息，输出FLV Tag/VideoAudio Tag/Payload Info信息至log.txt；
- FLVAccelerator: 修改FLV Timestamp，实现加速或减速播放；
- FLVExtractor: 按时间戳或FLV Tag索引截取部分FLV文件，或提取其中H.264视频码流;

# Paramater Format

## 1. FLVParser

调用格式：FLVParser inputFile.flv [log_level];

参数说明：

1. inputFile.flv: 输入文件路径。
2. log_level: 输出log的级别。0 - FLV Tag level; 1 - Video/Audio Tag level; 2 - Tag Payload Level;

## 2. FLVAccelerator

调用格式：FLVAccelerator inputFile.flv outputFile.flv acceleratorFactor;

参数说明：

1. inputFile.flv: 输入文件路径。
2. outputFile.flv: 输出文件路径。
3. acceleratorFactor: 变速参数，> 1表示加速，< 1表示减速。

## 3. FLVExtractor

调用格式： FLVExtractor inputFile.flv outputFile.flv option [range_start] [range_end]

参数说明

1. inputFile.flv: 输入文件路径。
2. outputFile.flv: 输出文件路径。
3. option [range_start] [range_end]：提取数据选项；

option：

1. -v: 复制没有音频只有视频tag的视频拷贝；
2. -raw\_h264: 提取出视频tag中包含的H.264格式视频流；功能类似于：ffmpeg.exe -i inputfile -vcodec copy -an -bsf:v h264\_mp4toannexb outputfile.h264；
3. -idx\_range: 根据FLV Tag索引范围提取输入文件的子文件；例：FLVExtractor inputFile.flv outputFile.flv -idx\_range 0 100
4. -ts\_range: 根据FLV Tag时间戳范围提取输入文件的子文件；例：FLVExtractor inputFile.flv outputFile.flv -ts\_range 0 100