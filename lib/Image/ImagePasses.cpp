#include "Image/ImageOps.h"
#include "Image/ImagePasses.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinTypes.h"
// Make sure this is included for WalkResult
#include "mlir/IR/Visitors.h" 

using namespace mlir;

namespace {
struct ImageOptimizePass : public PassWrapper<ImageOptimizePass, OperationPass<func::FuncOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ImageOptimizePass)

  StringRef getArgument() const final { return "image-optimize"; }

  void runOnOperation() override {
    func::FuncOp function = getOperation();
    bool changed = true;
    
    while (changed) {
      changed = false;
      
      // ✅ Walk lambda ab 'WalkResult' return karega
      function.walk([&](Operation *op) -> WalkResult {
        if (auto brighten = dyn_cast<image::BrightenOp>(op)) {
          
          float factor = brighten.getFactor().convertToFloat(); // Ya getValueAsDouble()
          
          if (factor == 1.0f) {
            brighten.getOutput().replaceAllUsesWith(brighten.getInput());
            
            // ✅ .erase() ki jagah ->erase()
            brighten->erase();
            changed = true;
            
            // ✅ Walk yahi rok do taaki use-after-free crash na ho
            return WalkResult::interrupt();
          }
        }
        // Agar op brighten nahi hai, ya 1.0 nahi hai, toh aage badho
        return WalkResult::advance();
      });
    }
  }
};
} // namespace

namespace image {
  void registerImagePasses() {
    PassRegistration<ImageOptimizePass>();
  }
}
