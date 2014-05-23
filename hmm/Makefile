CUDA_SDK_PATH := /opt/pgi/linux86-64/2013/cuda/CUDA-x86-SDK
CUDA_LIB_PATH := /usr/local/cuda-5.5/lib64
CUDA_TK_INC_PATH := /usr/local/cuda-5.5/include
CUDA_TK_LIB_PATH := /usr/local/cuda-5.5/lib64


LIB = -L$(CUDA_SDK_PATH)/lib -L$(CUDA_LIB_PATH) -lcudart \
	-lm -L$(CUDA_TK_LIB_PATH) -lcublas

INC = -I$(CUDA_SDK_PATH)/common/inc -I$(CUDA_TK_INC_PATH)

all: hmm.o hmm_test.o hmm_fo_c.o hmm_bwa_c.o \
	hmm_bwa_cuda.o \
	test_fo_c test_bwa_c test_bwa_cuda

clean:
	rm -rf build/*

hmm.o: src/main/hmm.h src/main/hmm.c
	gcc -c src/main/hmm.c -o build/hmm.o
hmm_test.o: src/main/hmm_test.h src/main/hmm_test.c
	gcc -c src/main/hmm_test.c -o build/hmm_test.o
hmm_fo_c.o: src/main/hmm_fo_c.h src/main/hmm_fo_c.c
	gcc -c src/main/hmm_fo_c.c -o build/hmm_fo_c.o $(LIB)
hmm_bwa_c.o: src/main/hmm_bwa_c.h src/main/hmm_bwa_c.c
	gcc -c src/main/hmm_bwa_c.c -o build/hmm_bwa_c.o $(LIB)
hmm_bwa_cuda.o: src/main/hmm_bwa_cuda.h src/main/hmm_bwa_cuda.cu
	nvcc $(INC) $(LIB) -c src/main/hmm_bwa_cuda.cu -o build/hmm_bwa_cuda.o

test_fo_c: src/test/test_fo_c.c
	gcc src/test/test_fo_c.c -o build/test_fo_c build/hmm.o \
		build/hmm_test.o build/hmm_fo_c.o $(LIB)

test_bwa_c: src/test/test_bwa_c.c
	gcc src/test/test_bwa_c.c -o build/test_bwa_c build/hmm.o \
		build/hmm_test.o build/hmm_bwa_c.o $(LIB)

test_bwa_cuda: src/test/test_bwa_cuda.c
	gcc src/test/test_bwa_cuda.c -o build/test_bwa_cuda build/hmm.o \
		build/hmm_test.o build/hmm_bwa_cuda.o $(LIB)


