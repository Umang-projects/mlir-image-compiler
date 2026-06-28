## 🛠️ tools/ Directory: The Standalone Compiler Driver Binary

The `tools/` directory is the final compilation aggregation point of the Image_Compiler infrastructure. It compiles the main command-line utility binary: `image-opt`.

This executable mirrors core LLVM/MLIR modular driver tools (like `mlir-opt` or LLVM's `opt`). Its sole purpose is to parse textual intermediate representation (`.mlir`), build an in-memory Abstract Syntax Tree (AST), execute our registered custom optimization passes, and output the transformed, high-performance IR.

---

## 📂 The Directory Blueprint

The tool tree is structured cleanly to separate binary wrapper logic from global build scripts:

```
tools/
├── CMakeLists.txt              # Top-Level Tools Router
└── image-opt/
    ├── CMakeLists.txt          # Target Executable Construction & Link Recipes
    └── image-opt.cpp           # The Main Entry Program (`main()`)
```

---

### 1. Top-Level Router: `tools/CMakeLists.txt`

Like the implementation layer, this file simply links the main tool subdirectories into the global build graph:

```cmake
add_subdirectory(image-opt)
```

---

## 🔍 Deep-Dive: `tools/image-opt/CMakeLists.txt`

This configuration file tells CMake how to convert our raw entry point `image-opt.cpp` into a compiled binary utility, hook up system architectures, and statically link our entire dialect implementation library.

Here is the exact structural anatomy of this CMake script:

```cmake
add_wm_executable(image-opt
  image-opt.cpp
)

target_link_libraries(image-opt
  PRIVATE
  MLIRImageCompilerLib     # 🔥 Links our entire handwritten custom compiler library
  MLIROptLib              # Pulls in standard mlir-opt core execution engine symbols
  MLIRIR                  # Supplies global context configurations
)

llvm_update_compile_flags(image-opt)
```

---

## 🛠️ Key Mechanics Explained:

### 1. Linking our Engine: `MLIRImageCompilerLib`

This is where all our hard work in the `lib/` directory pays off. By statically linking `MLIRImageCompilerLib`, we copy the byte definitions of our custom dialect (`ImageDialect`), operation constraints (`ImageOps`), and pattern optimization engines (`ImagePasses`) directly into the final `image-opt` executable.

### 2. Leveraging the Ecosystem: `MLIROptLib`

Writing a compiler from scratch usually means writing your own textual parser, lexer, file reader, command-line flag parser, and error diagnostic logger. MLIR eliminates this massive wheel-reinvention.

By linking `MLIROptLib`, our custom executable instantly inherits the entire enterprise-grade parsing harness (`MlirOptMain`) from LLVM. It automatically gives our binary standard tool capabilities like handling file inputs via the terminal, syntax coloring, error tracking, and standard flags (like `--pass-pipeline` or `--verify-diagnostics`).

### 3. Flag Hardening: `llvm_update_compile_flags(...)`

This is a proprietary LLVM macro. It enforces strict structural compilation safety features directly onto our binary target. It injects specific platform architecture flags, configures optimal stack layout parameters, enables RTTI constraints where needed, and sets optimal debugging symbol scopes to match the host LLVM installation perfectly.

---

## 🧠 Core Engineering Concept: The Tool Registry Lifecycle

A major conceptual challenge when constructing a custom MLIR driver is understanding why the tool crashes or ignores custom flags even after successful compilation.

### The Pitfall

If you build a standard `main()` function and call `mlir::MlirOptMain()`, the binary will launch, but running `./image-opt input.mlir --image-optimize` will throw an error: *Unknown command line flag* or *Unknown dialect 'image'*.

### The Solution Hidden in the Driver

The core MLIR engine is a completely blank canvas by default. It has no hardcoded awareness of our out-of-tree project. For the executable binary to recognize our code boundaries, we must explicitly inject our symbols into the global registry inside `image-opt.cpp` right before firing up the main compiler loop:

```cpp
int main(int argc, char **argv) {
    mlir::DialectRegistry registry;

    // 1. Manually insert our custom Dialect into the active workspace boundary
    registry.insert<image::ImageDialect>();

    // 2. Register standard dependencies (like func) so we can map structural functions
    registry.insert<mlir::func::FuncDialect>();

    // 3. Register our custom passes globally so the flag parser exposes them
    image::registerImagePasses();

    // 4. Pass control entirely to the LLVM/MLIR parsing engine
    return mlir::asMainReturnCode(
        mlir::MlirOptMain(argc, argv, "Image Compiler Optimization Driver\n", registry)
    );
}
```

---

## 🔄 The Tool Execution Pipeline

When you run a command in your terminal, the files configured by this directory process data through a strict lifecycle:

```
[ Text File: test.mlir ] 
          │
          ▼
┌────────────────────────────────────────────────────────┐
│ 1. image-opt (Binary spawned via MlirOptMain harness)   │
├────────────────────────────────────────────────────────┤
│ 2. Registry Check: Verifies 'image' namespace matches  │
│    the mapped configurations in ImageDialect.cpp.o     │
├────────────────────────────────────────────────────────┤
│ 3. Flag Parsing: Detects '--image-optimize' and boots  │
│    the Pass pipeline from ImagePasses.cpp.o            │
└────────────────────────────────────────────────────────┘
          │
          ▼
[ Clean, Optimized Text IR printed to stdout / file ]
```