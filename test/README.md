## test/ Directory: Functional Verification & IR Validation

This directory contains the integration and regression tests for the Image_Compiler infrastructure. Unlike traditional software development that relies on unit-testing frameworks (like Google Test), compiler infrastructure testing heavily relies on Textual IR Verification using LLVM's `FileCheck` utility and testing harnesses.

The tests here ensure that our compiler parsed operations correctly, validates syntax constraints, and executes graph transformations (like identity elimination) without corrupting the intermediate representation.

---

## 📂 Detailed Test File Breakdown

The directory contains three strategic testing files, each targeting a distinct phase of our dialect lifecycle:

### 1. `test_read.mlir` (Syntactic & Structural Parsing Validation)

**Purpose**: Verifies that the `image-opt` driver can successfully lex, parse, and reconstruct our custom dialect operations without throwing a syntax error.

**What it validates**: It ensures that TableGen-generated parsers and printers in `ImageOps.cpp.inc` correctly read attributes (like `factor = 2.5 : f32`) and tensor shapes (like `tensor<256x256xf32>`).

**The Baseline Rule**: If this test fails, it means our custom C++ dialect registration or TableGen grammar configurations contain structural errors.

### 2. `test_brighten.mlir` (Optimization Pattern Validation)

**Purpose**: Directly tests the canonicalization optimization rule implemented inside `ImageOps.cpp` (`SimplifyRedundantBrighten`).

**What it validates**: It passes an input graph containing an `image.brighten` operation with a static factor of exactly `1.0`. It asserts that the optimization pass completely deletes the dead instruction and rewires the return pointer to point directly to the original input tensor.

### 3. `test_pipeline.mlir` (Pipeline Flow & Integration Validation)

**Purpose**: Tests the end-to-end pass execution flow.

**What it validates**: It checks if our pass (`--image-optimize`) can interact cleanly within a sequence of standard MLIR passes. It ensures that running multiple transformations concurrently does not result in memory faults, broken IR verification states, or symbol clashes inside the `MLIRContext`.

---

## 🧠 Core Engineering Concept: The FileCheck Assertion Model

The standard method of verifying an MLIR optimization pattern is Declarative String Matching via LLVM's `FileCheck`. Instead of checking internal binary states, we embed expectations directly inside the `.mlir` files as specialized comments (`// CHECK:`).

### Behind the Scenes Machinery

When running a test, the input is passed to `image-opt`, which outputs the optimized text stream. This text stream is piped directly into `FileCheck`. `FileCheck` scans the text to verify that the generated structure matches our assertions.

Here is how the verification logic looks inside `test_brighten.mlir`:

```mlir
// RUN: image-opt %s --image-optimize | FileCheck %s

// CHECK-LABEL: func.func @verify_identity_elimination
func.func @verify_identity_elimination(%arg0: tensor<256x256xf32>) -> tensor<256x256xf32> {
  
  // The pass should identify this factor as 1.0 and eliminate this whole line!
  // CHECK-NOT: "image.brighten"
  %0 = "image.brighten"(%arg0) <{factor = 1.0 : f32}> : (tensor<256x256xf32>) -> tensor<256x256xf32>
  
  // The return block should now directly emit the original argument pointer (%arg0)
  // CHECK: return %arg0 : tensor<256x256xf32>
  return %0 : tensor<256x256xf32>
}
```

### Key Declarations Explained:

* **CHECK-LABEL**: Isolates a specific function block boundary. This ensures that errors in one function do not accidentally mismatch patterns inside a completely different function.
* **CHECK-NOT**: Assertive negation. It declares a strict compiler failure if the specified string (the `image.brighten` op) survives the optimization pipeline.
* **CHECK**: Verifies sequential progression. It confirms that the compiler successfully re-routed the structural graph node to return `%arg0`.

---

## ⚙️ How to Execute the Tests Manually

To run these verification test suites manually during development, use your compiled binary driver out of your local build directory:

```bash
# Navigate to your build directory
cd build

# Test 1: Verify the compiler can parse your custom operations
./tools/image-opt/image-opt ../test/test_read.mlir

# Test 2: Verify that your identity optimization pass executes perfectly
./tools/image-opt/image-opt ../test/test_brighten.mlir --image-optimize
```

---

## 🗺️ Future Testing Roadmap (Adding Lit)

As the project scales into Phase 1 (ImageToLinalg) and beyond, we will introduce an automated test automation layer using LLVM's `lit` (LLVM Integrated Tester) tool.

**The Goal**: Instead of running files individually, `lit` will parse the `RUN:` directives at the top of every `.mlir` file automatically, spawn parallel isolated threads, evaluate compliance status across hundreds of test targets simultaneously, and print a consolidated status matrix (PASS / FAIL).