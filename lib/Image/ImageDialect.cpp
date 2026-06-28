#include "Image/ImageDialect.h"
#include "Image/ImageOps.h"

using namespace mlir;
using namespace image;

// 1. TableGen ki generated dialect definitions ko yahan pull karo
#include "Image/ImageDialect.cpp.inc"

// 2. Dialect ka initialization logic jahan saare Ops register hote hain
void ImageDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "Image/ImageOps.cpp.inc"
  >();
}
