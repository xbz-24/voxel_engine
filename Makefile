CXX = g++
CXXFLAGS = -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

TARGET = main
SOURCE = main.cpp

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) -o $(TARGET) $(SOURCE) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
