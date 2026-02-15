CC ?= g++
CUDA_PATH ?=
CUDA_HOME ?=
NVCC ?=

ifeq ($(strip $(NVCC)),)
  ifneq ($(strip $(CUDA_PATH)),)
    NVCC := $(CUDA_PATH)/bin/nvcc
  else ifneq ($(strip $(CUDA_HOME)),)
    NVCC := $(CUDA_HOME)/bin/nvcc
  else
    NVCC := $(shell command -v nvcc 2>/dev/null)
  endif
endif

ifeq ($(strip $(NVCC)),)
  NVCC := /usr/local/cuda/bin/nvcc
endif

ifeq ($(strip $(CUDA_PATH)),)
  ifneq ($(strip $(CUDA_HOME)),)
    CUDA_PATH := $(CUDA_HOME)
  else
    CUDA_PATH := $(abspath $(dir $(NVCC))/..)
  endif
endif

ifeq ($(wildcard $(NVCC)),)
  $(error "nvcc not found. Set CUDA_PATH or NVCC to a valid CUDA installation")
endif

ifeq ($(wildcard $(CUDA_PATH)/include/cuda_runtime.h),)
  ifneq ($(wildcard /usr/local/cuda/include/cuda_runtime.h),)
    CUDA_PATH := /usr/local/cuda
  endif
endif

ifeq ($(wildcard $(CUDA_PATH)/include/cuda_runtime.h),)
  $(error "cuda_runtime.h not found. Set CUDA_PATH/CUDA_HOME to your CUDA root")
endif

CUDA_LIB_DIRS := $(CUDA_PATH)/lib64 $(CUDA_PATH)/targets/x86_64-linux/lib
CUDA_LIB_FLAGS := $(foreach d,$(CUDA_LIB_DIRS),-L$(d))
CUDA_RPATH_FLAGS := $(foreach d,$(CUDA_LIB_DIRS),-Wl,-rpath,$(d))

CCFLAGS := -O3 -I$(CUDA_PATH)/include
NVCCFLAGS := -O3 -gencode=arch=compute_89,code=compute_89 -gencode=arch=compute_86,code=compute_86 -gencode=arch=compute_75,code=compute_75 -gencode=arch=compute_61,code=compute_61
LDFLAGS := $(CUDA_LIB_FLAGS) $(CUDA_RPATH_FLAGS) -lcudart -pthread

CPU_SRC := RCKangaroo.cpp GpuKang.cpp Ec.cpp utils.cpp
GPU_SRC := RCGpuCore.cu

CPP_OBJECTS := $(CPU_SRC:.cpp=.o)
CU_OBJECTS := $(GPU_SRC:.cu=.o)

TARGET := rckangaroo

all: $(TARGET)

$(TARGET): $(CPP_OBJECTS) $(CU_OBJECTS)
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

clean:
	rm -f $(CPP_OBJECTS) $(CU_OBJECTS) $(TARGET)
