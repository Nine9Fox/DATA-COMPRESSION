CXX = g++
CXXFLAGS = -Wall

TARGET = bwtsearch

SRCS = bwtsearch.cpp

HDRS = bwtsearch.h

$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(HDRS) -o $(TARGET)

clean:
	rm -f $(TARGET)
