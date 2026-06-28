# Ek nayi MLIR dialect library banao jiska naam 'MLIRImageCompilerLib' rakh rahe hain
add_mlir_dialect_library(MLIRImageCompilerLib
  ImageDialect.cpp
  ImageOps.cpp
  ImagePasses.cpp

  # 🔥 CRITICAL: Yeh batata hai ki pehle TableGen chalna chahiye fir ye library compile hogi
  DEPENDS
  ImageOpsIncGen

  # MLIR ki core IR library se isko link karo
  LINK_LIBS
  MLIRIR
)
