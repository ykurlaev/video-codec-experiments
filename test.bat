@echo off
for /l %%i in (1, 1, 100) do (
    Release\Codec.exe -e test.yuv 720 486 test.compressed %%i 0 -q
    Release\Codec.exe -d test.compressed out.yuv -q
    Release\PSNR.exe 720 486 test.yuv out.yuv
    for %%F in (test.compressed) do echo %%~zF
)
for /l %%i in (1, 1, 255) do (
    Release\Codec.exe -e test.yuv 720 486 test.compressed %%i 1 -q
    Release\Codec.exe -d test.compressed out.yuv -q
    Release\PSNR.exe 720 486 test.yuv out.yuv
    for %%F in (test.compressed) do echo %%~zF
)
