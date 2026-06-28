# Image_Compiler: An Out-of-Tree MLIR Infrastructure

**Image_Compiler** is a modular, out-of-tree MLIR (Multi-Level Intermediate Representation) dialect and optimization pipeline. Designed around the core principles used by industry-grade systems like OpenAI's Triton, LLVM, and IREE, this project showcases progressive lowering—transforming high-level domain-specific abstractions down to hardware-optimized forms.

The primary objective of this compiler infrastructure is to demonstrate how declarative pattern rewrites, customized dialect operations, and low-level memory layout control (via dialect transformation passes) eliminate computing overhead before hardware execution.

---

# 🏗️ Core Architecture & Lowering Philosophy

Modern AI frameworks suffer from compilation gaps when mapping high-level mathematical operations to hardware accelerators. This project demonstrates a progressive compilation pipeline through multiple optimization stages:

- **High-Level Image Dialect**
  - Captures image processing intent using custom operations such as `image.brighten` and future image-specific operators.

- **Canonicalization**
  - Eliminates redundant operations through declarative rewrite patterns.
  - Example: Removing identity brighten operations where the scaling factor equals `1.0`.

- **Progressive Lowering (Roadmap)**
  - Gradually transforms high-level tensor operations into lower-level MLIR dialects including MemRef, Affine, SCF, Vector, and LLVM.

---

# 📂 Project Directory Structure

Below is the complete architectural layout of the **Image_Compiler** infrastructure.

```text
Image_Compiler
├── CMakeLists.txt                      # Global build configuration
├── include/                            # Global Declarations & Interface Headers
│   ├── CMakeLists.txt
│   └── Image/
│       ├── CMakeLists.txt
│       ├── ImageDialect.h              # Custom Dialect C++ Declaration
│       ├── ImageDialect.td             # TableGen Dialect Definition
│       ├── ImageOps.h                  # Custom Operations Declarations
│       ├── ImageOps.td                 # TableGen Operations Definitions
│       └── ImagePasses.h               # Optimization Passes Interface
│
├── lib/                                # Logic & Pipeline Implementations
│   ├── CMakeLists.txt
│   └── Image/
│       ├── CMakeLists.txt
│       ├── ImageDialect.cpp            # Custom Dialect Registration
│       ├── ImageOps.cpp                # Canonicalization & Op Verification Logic
│       └── ImagePasses.cpp             # Greedy Pattern Rewrite and Pass Execution
│
├── tools/                              # Execution Drivers & Binary Executables
│   ├── CMakeLists.txt
│   └── image-opt/
│       ├── CMakeLists.txt
│       └── image-opt.cpp               # Main Compiler Driver Binary
│
└── test/                               # Integration & Functional Verification Tests
    ├── test_brighten.mlir              # Identity optimization validation
    ├── test_pipeline.mlir              # Pipeline flow validation
    └── test_read.mlir                  # Structural parsing validation
```

---

# 🔧 Component Breakdown

| Directory | Primary Purpose | Key Technologies | Documentation Status |
|-----------|-----------------|------------------|----------------------|
| **include/** | Defines dialect specifications, operation arguments, attributes, and pass interfaces. | MLIR TableGen, C++ Headers | Complete |
| **lib/** | Implements parsing, verification, canonicalization, and optimization passes. | MLIR Pattern Rewriter, C++ | Complete |
| **tools/** | Builds the standalone `image-opt` executable used to parse and optimize MLIR files. | MLIR Tooling | Complete |
| **test/** | Contains MLIR test programs validating parsing and optimization behavior. | MLIR, FileCheck | Complete |

---

# ⚡ Quick Build Guide

## Prerequisites

- LLVM/MLIR built from source
- C++17 compatible compiler
- CMake
- Ninja

---

## Configure

```bash
mkdir build
cd build

cmake -G Ninja .. \
  -DMLIR_DIR=/path/to/llvm-project/build/lib/cmake/mlir \
  -DLLVM_DIR=/path/to/llvm-project/build/lib/cmake/llvm
```

---

## Build

```bash
ninja
```

---

# 🚀 Running the Compiler

Execute the optimization pipeline using the standalone driver:

```bash
./tools/image-opt/image-opt \
    ../test/test_brighten.mlir \
    --image-optimize
```

# 📚 Learning Objectives

This project demonstrates the following MLIR concepts:

- Custom Dialect Development
- TableGen Code Generation
- Operation Verification
- Canonicalization
- Pattern Rewriting
- Greedy Rewrite Driver
- Compiler Pass Infrastructure
- Progressive Lowering
- Standalone MLIR Tool Development

---

# 📝 Development Notes

Each major directory contains its own localized documentation describing implementation details:

- `include/README.md`
- `lib/README.md`
- `tools/README.md`
- `test/README.md`

These documents explain the underlying source code, TableGen generation pipeline, compiler architecture, and optimization mechanisms implemented within each component.