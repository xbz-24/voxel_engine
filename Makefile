CXX = g++
CXXFLAGS = -I/opt/homebrew/include -I/Applications/Autodesk/FBX\ SDK/2020.3.7/include -std=c++11
LDFLAGS = -L/opt/homebrew/lib -L/Applications/Autodesk/FBX\ SDK/2020.3.7/lib/clang/release -lglfw -lfbxsdk -lxml2 -lz -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

TARGET = main
SOURCES = main.cpp fbx_simple.cpp

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) -o $(TARGET) $(SOURCES) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
