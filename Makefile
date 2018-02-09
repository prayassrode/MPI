CXX=mpicxx
LD=mpicxx
CXXFLAGS=-O3 -std=c++11
CFLAGS=-O3
#ARCHIVES=libfunctions.a


matrix_multiplication: matrix_multiplication.o
	$(LD) $(LDFLAGS) matrix_multiplication.o -o matrix_multiplication
matmulTry: matmulTry.o
	$(LD) $(LDFLAGS) matmulTry.o -o matmulTry
matrix_seq: matrix_seq.o
	$(LD) $(LDFLAGS) matrix_seq.o -o matrix_seq


plots.pdf:
	./plots.sh


