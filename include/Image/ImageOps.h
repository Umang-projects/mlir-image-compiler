#ifndef IMAGE_OPS_H
#define IMAGE_OPS_H

#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpDefinition.h"
#include "Image/ImageDialect.h" // Apne dialect ko include karo
#include "mlir/Interfaces/SideEffectInterfaces.h" // Iski wajah se 'MemoryEffects' wala error aaya tha
#include "mlir/Bytecode/BytecodeOpInterface.h" 

// MAGIC TRICK: Vending machine ko order diya "Sirf C++ Classes (Structure) nikalna!"
#define GET_OP_CLASSES 
#include "Image/ImageOps.h.inc"

#endif // IMAGE_OPS_H
