
mkdir dist

mkdir dist\lib
mkdir dist\lib64
mkdir dist\bin
mkdir dist\bin64
mkdir dist\include
mkdir dist\include\nvtt

copy Release.Win32\bin\nvcompress.exe dist\bin
copy Release.Win32\bin\nvdecompress.exe dist\bin
copy Release.Win32\bin\nvtt.dll dist\bin
copy Release.Win32\bin\nvtt.pdb dist\bin
copy Release.Win32\lib\nvtt.lib dist\lib

copy Release.x64\bin\nvcompress.exe dist\bin64
copy Release.x64\bin\nvdecompress.exe dist\bin64
copy Release.x64\bin\nvtt.dll dist\bin64
copy Release.x64\bin\nvtt.pdb dist\bin64
copy Release.x64\lib\nvtt.lib dist\lib64

copy Release.Win32\include\nvtt\nvtt.h dist\include\nvtt\nvtt.h
