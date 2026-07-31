# NOTVKCUBE

NOTVKCUBE is a GPU graphics test software developed for the PlayStation 3 (PS3) homebrew ecosystem. The project draws direct inspiration from the classic Mesa 3D `vkcube` demonstration. Since the PS3 does not natively support the Vulkan API, this implementation utilizes the low-level Tiny3D library from the open-source PSL1GHT SDK to render a spinning 3D cube on the PS3's RSX graphics processor using the PPE (PowerPC Processor Element).

## Features

- **Vibrant Multi-Color Gradients:** The cube is shaded using a per-vertex color array that generates smooth, colorful gradients across the faces.
- **Dynamic Lighting:** Real-time Gouraud shading calculated on the CPU (PPE) and applied via Tiny3D vertex colors.
- **Fixed-Function 3D Pipeline:** Utilizes row-vector matrix mathematics for translation and rotation on the PS3.
- **Input Integration:** Gamepad support allowing graceful exit to the XMB (XrossMediaBar) by pressing START, CROSS, or CIRCLE.

## Prerequisites

- **PS3 Toolchain**: `ps3dev` / PSL1GHT PPU toolchain (`ppu-gcc`, `make_self_npdrm`, `sprxlinker`, `pkg`). Automatically installed via `make prepare`.

## Quick Start & Building from Source

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/Cruslan/PS3-NotVKCube
   cd PS3-NotVKCube
   ```

2. **Prepare the PS3 SDK Environment**:
   Downloads and extracts the pre-compiled `ps3dev` PPU toolchain and PSL1GHT SDK archive directly into `./ps3dev` inside the project directory. This provides the `ppu-gcc` cross-compiler, Cell Broadband Engine PPU headers, system libraries (`sysUtil`, `Tiny3D`), and PKG packaging tools (`make_self_npdrm`, `fself`, `pkg`), enabling isolated building without requiring system-wide installations or root privileges:
   ```bash
   make prepare
   ```

3. **Build the Package**:
   Compile the binary and generate the signed installation packages:
   ```bash
   make
   ```

4. **Output Packages**:
   Upon completion, the build outputs the following installable files:
   - `notvkcube.elf`: The raw linked Executable and Linkable Format binary.
   - `notvkcube.self`: The signed PlayStation 3 executable for retail/CEX consoles.
   - `notvkcube.fake.self`: The fake-signed executable, compatible with RPCS3 emulator and DEX consoles.
   - `notvkcube.pkg`: Dual-compatible PKG installer for RPCS3 emulator and physical PS3 consoles (CFW / HEN).
   - `notvkcube.gnpdrm.pkg`: The finalized NPDRM PKG package.

## Credits & References

- **[Mesa 3D Graphics Library (vkcube)](https://gitlab.freedesktop.org/mesa/demos)**: Inspired by and based on the classic `vkcube` Vulkan demonstration project from the Mesa 3D project.
- **[PSL1GHT SDK & Tiny3D](https://github.com/ps3dev/PSL1GHT)**: PS3 open-source SDK and low-level 3D graphics rendering library.

## License

This project is released under the GNU General Public License v3.0 (GPLv3).
