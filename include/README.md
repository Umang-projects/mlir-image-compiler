# 🏗️ `build/include/Image/` Deep Dive: Generated TableGen Artifacts

This directory contains the auto-generated C++ code blocks produced by **LLVM TableGen (`mlir-tablegen`)**. These `.inc` (Increment) files are the concrete implementation of the rules defined in our high-level `.td` files (`ImageOps.td` and `ImageDialect.td`).

Instead of manually writing thousands of lines of repetitive C++ parsing, printing, and verification logic, the build system (`ninja`) generates these files dynamically and splices them directly into our handwritten source files (`lib/` and `include/`).

---

## 📂 Structural Breakdown of Generated Files

### 1. `ImageDialect.h.inc` & `ImageDialect.cpp.inc` (The Dialect Hooks)

* **Purpose**: Provides the underlying class machinery for the `ImageDialect`.
* **What's inside `.h.inc`**: Declares the initialization functions and hooks required by `mlir::MLIRContext` to recognize `#image` as a valid dialect namespace.
* **What's inside `.cpp.inc`**: Contains the actual registration logic that loads our custom dialect components into the global compiler state.

### 2. `ImageOps.h.inc` (The Generated C++ Interface & Getters)

* **Purpose**: This file contains the macro-wrapped C++ class declarations for every single operation (`BrightenOp`, `BlurOp`, `MergeOp`).
* **Why it matters**: This is where TableGen generates the automatic C++ API that you interact with in your passes.

```cpp
::mlir::Value getInput();          // Fetches the dynamic image tensor operand
::mlir::FloatAttr getFactor();     // Fetches the compile-time brightness constant
```

### 3. `ImageOps.cpp.inc` (The Massive Heavy Machinery Boilerplate)

* **Purpose**: Contains the actual implementation definitions for parsing, printing, and structural verification of your operations.
* **Why it's a "Black Box"**: You never edit this manually. It contains the low-level code that verifies if an operation has the correct number of inputs, checks if types match, and converts the custom text representation of your MLIR code back into structural C++ compiler graph objects.

---

## 🧠 Deep-Dive: Resolving the Core Architectural Doubts

### 🔍 Breakthrough 1: How C++ Connects to These `.inc` Files

```cpp
// Inside include/Image/ImageOps.h
#define GET_OP_CLASSES
#include "Image/ImageOps.h.inc" // 🔥 The compiler injects all generated getters/setters right here!
```

### 🔍 Breakthrough 2: The Unified Argument Paradigm (`let arguments`)

```tablegen
let arguments = (ins
    AnyType:$input,
    F32Attr:$factor
);
```

TableGen automatically generates:

- `getInput()`
- `getFactor()`

---

## 🛠️ The Localized Compilation Cycle

```text
               +-----------------------------+
               | include/Image/ImageOps.td   |
               +-----------------------------+
                              |
               +--------------+--------------+
               |                             |
 (mlir-tablegen -gen-op-decls) (mlir-tablegen -gen-op-defs)
               |                             |
               v                             v
+-----------------------------+ +-----------------------------+
| build/.../ImageOps.h.inc    | | build/.../ImageOps.cpp.inc  |
+-----------------------------+ +-----------------------------+
               |                             |
    (Injected via macro)          (Injected via macro)
               |                             |
               v                             v
+-----------------------------+ +-----------------------------+
| include/Image/ImageOps.h    | |    lib/Image/ImageOps.cpp   |
+-----------------------------+ +-----------------------------+
```

1. **`CMakeFiles/` & `cmake_install.cmake`** track dependencies and timestamps.
2. During preprocessing, generated `.inc` files are injected into handwritten source files.

---

# 🛠️ Deep-Dive: CMake Internals & Dependency Tracking (`CMakeFiles` & `cmake_install.cmake`)

While TableGen outputs the concrete C++ target files (`.inc`), CMake generates internal orchestration files inside `build/include/Image/` to manage the build lifecycle, check timestamps, and handle target installation paths.

Understanding these files explains how the build system prevents redundant compilations and tracks raw execution dependencies.

---

## 🔍 Detailed Breakdown of Build System Artifacts

### 1. The `CMakeFiles/` Directory (The Dependency Brain)

This is not a generic scratchpad; it is the exact engine room where CMake and Ninja track state. If you peek inside `build/include/Image/CMakeFiles/`, you will find internal dependency graphs and rules.

#### Why is it there? (The Reason)

When you run `ninja`, the build tool needs to answer one question instantly: **"Has `ImageOps.td` or `ImageDialect.td` changed since the last compile?"** Instead of scanning massive files every time, CMake writes tiny timestamp and configuration files inside this directory to cache the last known state of the compilation environment.

#### Key Mechanics Inside:

* **Dependency Stamping**: It maps out that `ImageOps.h.inc` strictly relies on `ImageOps.td`. If the modification time of `ImageOps.td` is identical to the logged timestamp inside `CMakeFiles/`, Ninja completely bypasses the execution of `mlir-tablegen`. This keeps incremental compile times under a few milliseconds.
* **Compiler Sanity Rules**: It tracks which version of the compiler and flags generated these headers to prevent binary mismatches if you switch build environments or LLVM targets.

---

### 2. `cmake_install.cmake` (The Deployment Script)

This file is a dynamically generated auto-contained script that executes during the installation phase (e.g., when running `ninja install`).

#### Why is it there? (The Reason)

When you build an out-of-tree MLIR project, you don't just want to run it out of your local `build/` folder forever. Eventually, you might want to expose your custom `Image` dialect as an SDK or package so other custom compilers or runtime systems (like a custom Triton-based deployment harness) can link against it.

#### Key Mechanics Inside:

* It reads the distribution configurations from your root build script and translates them into raw system directives.
* If instructed, it knows exactly how to safely package and route `ImageDialect.h`, `ImageOps.h`, and the generated `.inc` files to destination system paths like `/usr/local/include/Image/` while preserving file execution permissions.

---

## 🔄 The Step-by-Step Compilation Process Lifecycle

Here is exactly how a build instruction flows through these specific files behind the scenes when you type `ninja`:

```text
                  +───────────────────────────────+
                  |      User types 'ninja'       |
                  +───────────────────────────────+
                                  │
                                  ▼
         +──────────────────────────────────────────────────+
         | Ninja checks build.ninja & inspects the local    |
         | build/include/Image/CMakeFiles/ state directory  |
         +──────────────────────────────────────────────────+
                                  │
                  ┌───────────────┴───────────────┐
                  ▼                               ▼
        [TIMESTAMPS MATCH]              [TIMESTAMPS MISMATCH]
     (No changes to .td files)          (You edited ImageOps.td)
                  │                               │
                  ▼                               ▼
+───────────────────────────────────+ +───────────────────────────────────+
| Skip TableGen entirely!           | | 1. Ninja invalidates cache inside |
| Instantly move to linking stage.  | |    CMakeFiles/ folder.            |
| Time saved: 100% 🎉               | | 2. Spawns mlir-tablegen binary.   |
+───────────────────────────────────+ | 3. Regenerates the .inc files.    |
                                      | 4. Logs new timestamps inside     |
                                      |    CMakeFiles/ for next time.     |
                                      +───────────────────────────────────+

```

### The Detailed Steps:

1. **The Evaluation**: Ninja hooks into the rules cached inside `CMakeFiles/` to compare the system timestamps of `include/Image/ImageOps.td` against the existing `build/include/Image/ImageOps.h.inc`.
2. **The Cache Hitting Mechanism**: If you only edited a file inside `lib/Image/ImagePasses.cpp`, CMake recognizes that the TableGen definitions are untouched. It marks the targets inside `build/include/Image/` as **UP-TO-DATE**, blocking TableGen from wasting processing cycles regenerating text boilerplate.
3. **The Target Registration**: Once the compilation cycle clears, `cmake_install.cmake` is updated to record the exact path configurations of the newly baked components, ensuring that your compiler binary remains perfectly synchronized with its structural environment headers.