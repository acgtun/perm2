####################################################
#                                                  #
#  seed-extension-aligner                          #
#  Haifeng Chen (haifengc at usc dot edu)          #
#  University of Southern California               #
#  Feb 5, 2014                                     #
#                                                  #
####################################################


CC := gcc
CXX := g++
CXXFLAGS := -O3 -Wall -fmessage-length=0

TARGET := aligner

SRCS := hash.cpp iofile.cpp option.cpp match.cpp seed.cpp refin.cpp\
	seed-extension-aligner.cpp bandedGlobalAlignment.cpp bitscode.cpp

OBJS := $(SRCS:.cpp=.o)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<
                
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o *.exe
                                        
