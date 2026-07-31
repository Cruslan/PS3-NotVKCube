# Changelog

All notable changes to the NOTVKCUBE project will be documented in this file.

## [v1.1.0] - 2026-08-01

### 🚀 Features & Improvements
- **Automated PS3 SDK Setup (`make prepare`)**: Integrated a zero-friction SDK preparation workflow into the build system. Running `make prepare` automatically downloads and extracts the pre-compiled `ps3dev` PPU toolchain directly into `./ps3dev`, eliminating the need for system-wide toolchain installation or root privileges.
- **Enhanced Build Pipeline & NPDRM Packaging**: Updated the `Makefile` and packaging pipeline with `sfo.xml` support, `ppu-strip`, `sprxlinker`, and `make_self_npdrm`. The build process now generates retail-signed PKG files (`notvkcube.pkg`, `notvkcube.gnpdrm.pkg`) compatible with physical PS3 consoles (CFW/HEN) as well as fake-signed binaries (`notvkcube.fake.self`) for DEX consoles and the RPCS3 emulator.
- **GPL-3.0 Relicensing**: Standardized the project license to the GNU General Public License v3.0 (GPL-3.0) for consistency across the PS3 homebrew ecosystem.
- **Attribution & Documentation**: Updated `README.md` with explicit credits and references to the Mesa 3D project's classic `vkcube` demonstration tool, along with comprehensive installation instructions.
