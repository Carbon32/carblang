## Introduction

### What is Carblang?

Carblang is an interpreted, general-purpose programming language developed primarily as a learning project to explore the internals of interpreters and virtual machines.

**Carblang is not recommended for production use.** Its primary purpose is educational. I started by reading *Crafting Interpreters* and gradually expanded the language by experimenting with new features and capabilities. While the overall architecture may not be ideal (in my defense, I never expected the project to grow this much), Carblang has evolved to support a variety of programming paradigms, data structures, and built-in functionalities.

### Features

Carblang includes a wide range of features, including:

- **Object-oriented programming:** Support for OOP, although some minor features are still missing.
- **Rich data types and structures:** A broad selection of built-in types and data structures, each with its own set of methods.
- **Control flow:** Essential control flow constructs, including ternary operators.
- **Optionally typed with partial static checking:** This provides a great deal of flexibility.
- **Bytecode virtual machine:** A bytecode-based VM featuring several optimizations.
- **Built-in libraries:** Utilities for a variety of tasks, including operating system operations, regular expressions, and JSON/CSV data manipulation.
- **GUI development:** Built-in support for developing graphical user interfaces (using raylib).
- **HTML generation:** Functionality for constructing HTML pages directly from Carblang.

For a more comprehensive overview of the language's capabilities, explore the [examples](./examples) directory, which contains practical demonstrations of its features.

## Getting Started

### Building and Running

Getting started with Carblang is straightforward.

**1. Clone the repository**

First, clone the repository using Git:

```bash
git clone https://github.com/Carbon32/carblang
cd carblang
```

**2. Build the project**

Use the appropriate build script for your operating system:

- **Windows:** Run `wbuild.bat`.
- **Linux:** Run `lbuild.sh`.

The build process relies on CMake.

**3. Configure your environment**

Once the build is complete, add the directory containing the generated executable to your system's `PATH` environment variable.

**4. Run Carblang**

After configuring your `PATH`, you should be able to invoke Carblang directly from your terminal:

```bash
carblang
```

---

*Carblang is an experimental programming language built to explore interpreter design, bytecode execution, and language implementation.*