cmake_minimum_required(VERSION 3.15.0)
project(Image_Compiler LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)

# MLIR aur LLVM ko dhundho
find_package(MLIR REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)

# 🔥 [IMPORTANT FIX]: TableGen ko batao ki MLIR ki apni .td files kahan hain
foreach(dir ${MLIR_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
  list(APPEND LLVM_TABLEGEN_FLAGS "-I${dir}")
endforeach()

#ye wala .h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#ye wala .inc generated wala
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# 🌍 [FUTURE FIX]: Baki C++ files ke liye bhi MLIR ka rasta kholo
include_directories(${MLIR_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})

#idhr CMake..(ROOT) wale se data save hua ba aage ke mein use kr paaenge,internal cmkae... in data use karke apne files ko comile krega. mtb assan bhasha mein ROOT(CMAke.) ka pura datasaari paths,saari settings aaram se access kr skte hain.
# Abhi sirf include ko active rakha hai
add_subdirectory(include/Image)
add_subdirectory(lib)     # Isko comment kar do
add_subdirectory(tools)   # Isko comment kar do
