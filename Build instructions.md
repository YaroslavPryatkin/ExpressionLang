Just compile all .cpp files. 
From MSVC terminal:

```
 cd <folder containing /source> 

 cl.exe /EHsc /std:c++17 /I source source\*.cpp /Fo"build\\" /Fe:calculator.exe /O2
```

.bat makefile for Windows:
```
@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

cd <folder containing /source> 

if not exist build mkdir build

cl.exe /EHsc /std:c++17 /I source source\*.cpp ^
    /Fo"build\\" ^
    /Fe:calculator.exe ^
    /O2
```

Makefile for linux
```
CXX      := g++
CXXFLAGS := -std=c++17 -O2 
INCLUDES := -Isource
SRC      := source/*.cpp
BUILD    := build
TARGET   := calculator

all: $(TARGET)

$(TARGET):
	mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)
```
