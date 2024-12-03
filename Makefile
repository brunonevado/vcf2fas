
PROGRAM=vcf2fas


CC=g++
CFLAGS=-Wall -O3 -std=c++17

$PROGRAM: source/main.cpp source/args.h source/args.cpp source/common.h source/common.cpp source/vcf.h source/vcf.cpp source/fasta.h source/fasta.cpp
	$(CC) $(CFLAGS) -o $(PROGRAM) source/main.cpp source/args.h source/args.cpp source/common.h source/common.cpp source/vcf.h source/vcf.cpp source/fasta.h source/fasta.cpp
	

