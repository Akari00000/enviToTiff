#enviToTiff
这是enviToTiff 2.0版，使用TXT做日志进行检索。使用说明在enviToTiff运行逻辑.jpg里
实现的功能是，读取ENVI图像（.hdr+.img），转换成tiff图像

ps：
1. 1.0版本的逻辑目前没有维护，使用时请自行修改路径。主要修改mainenvitotiff函数涉及到的函数的路径，因为在修改2.0版时，部分函数被我设置成2.0专用的路径
2.本tiff格式，DE数量为15，使用时请确认自己的图像DE的数量
3.数据源（.img）目前只支持2字节和4字节的数据
4.默认为小端存储，原库的大小端转换算法去除
5.路径说明：默认所有数据的存储在*/ENVI文件夹里，默认ENVI文件夹下有LG、ENVI、TIFF三个文件夹，默认三个文件夹里都有LOG文件夹，默认ENVI和TIFF的文件夹里都有SAVE文件夹，SAVE文件夹用于存储已经处理过的log的文件名（作为对比数组）使用

本库改自https://github.com/TerrencePai/TifReadSave/blob/main/tiffReadSave.h
