#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

// 1. Apne dialect aur passes ke headers include karo
#include "Image/ImageDialect.h"
#include "Image/ImagePasses.h" // 👈 Custom passes ka header!

int main(int argc, char **argv) {
	  // 2. Pehle standard passes register karo
	    mlir::registerAllPasses();

	        // 3. AB APNE CUSTOM PASS KO REGISTER KARO! 👈 (SABSE ZAROORI)
	          image::registerImagePasses(); 

	            mlir::DialectRegistry registry;
	              mlir::registerAllDialects(registry);

	                  // 4. Apne dialect ko registry mein insert karo (agar pehle se na kiya ho)
	                    registry.insert<image::ImageDialect>(); 

	                      return mlir::asMainReturnCode(
	                      	      mlir::MlirOptMain(argc, argv, "Image optimizer driver\n", registry));
	                      	      }
