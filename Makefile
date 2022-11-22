CXXFLAGS=-std=c++17 -fopenmp
CXX=g++
all:
	$(CXX) $(CXXFLAGS) "slay_test/slay_test.cpp" -o slay
	chmod +x slay
	./slay "slay_test/data1.txt"
clean:
	$(RM) slay
