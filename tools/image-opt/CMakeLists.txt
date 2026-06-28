# Naya executable (tool) declare karo
add_executable(image-opt image-opt.cpp)

# LLVM aur MLIR libraries ko link karo jo MlirOptMain ke liye chahiye
target_link_libraries(image-opt
  PRIVATE
      MLIRIR
          MLIRParser
              MLIRSupport
                  MLIROptLib
                      MLIRRegisterAllDialects  # 👈 YE LINE ADD KARO
                          MLIRRegisterAllPasses    # 👈 YE LINE ADD KARO
                              MLIRImageCompilerLib
                              )

