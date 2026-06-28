// include/Image/ImageOps.td
include "ImageDialect.td"
include "mlir/Interfaces/SideEffectInterfaces.td" // Optimization safe banane ke liye

def BrightenOp : Op<Image_Dialect, "brighten", [Pure]> {
  // Input operand aur attribute dono ko ek hi jagah (arguments mein) rakho:
  let arguments = (ins
      AnyType:$input,
      F32Attr:$factor
  );

  let results = (outs F32Tensor:$output);
}
