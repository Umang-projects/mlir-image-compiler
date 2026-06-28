# 1. TableGen ko batao ki kis file ko convert karna hai
set(LLVM_TARGET_DEFINITIONS ImageOps.td)

# 2. Dialect ka .inc banao
mlir_tablegen(ImageDialect.h.inc -gen-dialect-decls)
mlir_tablegen(ImageDialect.cpp.inc -gen-dialect-defs)

# 3. Operations ka .inc banao
mlir_tablegen(ImageOps.h.inc -gen-op-decls)
mlir_tablegen(ImageOps.cpp.inc -gen-op-defs)

# 4. In sab files ko ek "Target/Package" mein pack kar do
# Is package ka naam humne 'ImageOpsIncGen' rakha hai. 
# Lib folder baad mein isko use karega.
add_public_tablegen_target(ImageOpsIncGen)
