
PROGRAM=vcf2fas


CC=g++
LIBS=-lbpp-core -lbpp-seq


CFLAGS=-Wall -O3 -std=c++11

# use below if bio++ libs/includes are not in default location 
# IDIR =/home/Bruno/local/include/bpp
# LDIR =/home/Bruno/local/lib/bpp/
# CFLAGS=-I$(IDIR) -L$(LDIR) -Wall -O3 -std=c++11


$PROGRAM: source/main.cpp source/args.h source/args.cpp source/common.h source/common.cpp source/vcf.h source/vcf.cpp
	$(CC) $(CFLAGS) -o $(PROGRAM) source/main.cpp source/args.h source/args.cpp source/common.h source/common.cpp source/vcf.h source/vcf.cpp $(LIBS)
	

