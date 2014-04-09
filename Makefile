OBJS =	EditDistance.o \
		Dawg.o

OPT = -O3
CXX = clang++
CXXFLAGS = $(OPT) -std=c++11 -stdlib=libc++

editdist: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o editdist

.phony: clean
clean:
	rm -f *.o

.phony: run
run: editdist
	time ./editdist
