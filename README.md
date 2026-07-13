# NOTVKCUBE

NOTVKCUBE is a GPU graphics test software developed for the PlayStation 3 (PS3) homebrew ecosystem. The project draws direct inspiration from the classic Vulkan VKCube demonstration. Since the PS3 does not natively support the Vulkan API, this implementation utilizes the low-level Tiny3D library from the open-source PSL1GHT SDK to render a spinning 3D cube on the PS3's RSX graphics processor using the PPE (PowerPC Processor Element).

## Features

- **Vibrant Multi-Color Gradients:** The cube is shaded using a per-vertex color array that generates smooth, colorful gradients across the faces.
- **Dynamic Lighting:** Real-time Gouraud shading calculated on the CPU (PPE) and applied via Tiny3D vertex colors.
- **Fixed-Function 3D Pipeline:** Utilizes row-vector matrix mathematics for translation and rotation on the PS3.
- **Input Integration:** Gamepad support allowing graceful exit to the XMB (XrossMediaBar) by pressing START, CROSS, or CIRCLE.

## Prerequisites

To compile this project, you need the PSL1GHT SDK toolchain. The repository includes a pre-packaged compiler environment inside the local `ps3dev` directory.

## Compiling

To build the project and generate the final binaries (`.elf`, `.self`, and `.pkg`), execute the following command:

```bash
make
```

If you need to force a clean build without invoking forbidden file deletion commands, run:

```bash
make -B
```

## Generated Outputs

Upon a successful build, the following files will be produced:

- `notvkcube.elf`: The raw linked Executable and Linkable Format binary.
- `notvkcube.self`: The signed PlayStation 3 executable for retail/CEX consoles.
- `notvkcube.fake.self`: The fake-signed executable, compatible with RPCS3 emulator and DEX consoles.
- `notvkcube.pkg`: The retail package file ready to be installed on homebrew-enabled PS3 consoles.
- `notvkcube.gnpdrm.pkg`: The finalized geohot-style signed package file.
