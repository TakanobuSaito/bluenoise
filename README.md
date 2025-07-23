# Blue Noise Generator (with Source Code)

## Features

- **Fast creation, saving, and loading** of blue noise matrices
  - It can generate 8192x8192 blue noise matrix within a few hours.
- **API compatible** with standard random number generators (`rand`, `srand`, `random`, etc.)
- **Ordered dithering functions** using blue noise
- **Comparison dithering functions**:
  - Ordered dithering uses white noise
  - Dithering uses error diffusion methods(Floyd & Steinberg)
- **Includes test data and test programs**

## Usage

- Build bnmaskutf8lib project
  - This is a satic link library(C++ class library).
- Build bnmaskutf8dll project
  - This is a dynamic link library that refers static link library.
- Build bnmaskutf8 project(C++ test program)
  - This is a test program that referes static link library or dynamic link library.
  - If preprocessor definition "DYNAMIC_LINK" is defined. This program uses dynamic link library. It uses bnmaskutf8lib.dll and bnmaskutf8dll.dll. Put both files in executable file path.
  - If preprocessor definition "DYNAMIC_LINK" is **not** defined. This program uses static link library. It uses bnmaskutf8lib.dll. Put a file in executable file path.
- Python test programs.
  - They use bnmaskutf8lib.dll and bnmaskutf8dll.dll. Put both files in executable file path.
  - They need opencv ,matplotlib and numpy.

## Link

- About bluenoise
  - (Japanese Version)https://takanobusaito.github.io/bnmanual/bluenoise.html
  - (English Version )https://takanobusaito.github.io/bnmanual/bluenoiseen.html
- Class Library Reference（Static link library）
  - (Japanese Version)https://takanobusaito.github.io/bnmanual/specclass.html
  - (English Version )https://takanobusaito.github.io/bnmanual/specclassen.html
- Dynamic Link Library Reference
  - (Japanese Version)https://takanobusaito.github.io/bnmanual/specdll.html
  - (English Version )https://takanobusaito.github.io/bnmanual/specdllen.html
