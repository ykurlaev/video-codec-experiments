#!/bin/sh

Release/Codec.exe -e test.yuv 640 346 test.compressed 50 0 -q
Release/Codec.exe -d test.compressed out.yuv -q
du -b test.compressed
Release/PSNR.exe 640 346 test.yuv out.yuv

Release/Codec.exe -e test.yuv 640 346 test.compressed 64 1 -q
Release/Codec.exe -d test.compressed out.yuv -q
du -b test.compressed
Release/PSNR.exe 640 346 test.yuv out.yuv

Release/Codec.exe -e ../test.yuv 640 346 ../test.compressed 50 0 -q
Release/Codec.exe -d ../test.compressed ../out.yuv -q
du -b ../test.compressed
Release/PSNR.exe 640 346 ../test.yuv ../out.yuv

Release/Codec.exe -e ../test.yuv 640 346 ../test.compressed 64 1 -q
Release/Codec.exe -d ../test.compressed ../out.yuv -q
du -b ../test.compressed
Release/PSNR.exe 640 346 ../test.yuv ../out.yuv
