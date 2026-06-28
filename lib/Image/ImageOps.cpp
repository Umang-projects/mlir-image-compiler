#include "Image/ImageOps.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"

using namespace mlir;
using namespace image;

// TableGen se saari Ops ki implementation classes yahan generate ho jayengi
#define GET_OP_CLASSES
#include "Image/ImageOps.cpp.inc"
