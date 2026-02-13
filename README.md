# FBInk NIF

Elixir NIF bindings for [FBInk](https://github.com/NiLuJe/FBInk) (FrameBuffer eInker) — a C library for printing text and images to eInk Linux framebuffer devices.

This library provides a complete Elixir interface to FBInk, covering text rendering (bitmap and OpenType fonts), image display, drawing primitives, screen management, and platform-specific features for eInk e-readers.

## Supported Devices

- Kobo
- Kindle
- reMarkable
- PocketBook
- BQ Cervantes
- Tolino
- Generic Linux framebuffer devices

## Requirements

- Elixir ~> 1.14
- A C11-capable compiler
- [FBInk](https://github.com/NiLuJe/FBInk) built as a shared library (`libfbink.so`)

## Installation

Add `fbink_nif` to your `mix.exs` dependencies:

```elixir
def deps do
  [
    {:fbink_nif, github: "mlainez/fbink_nif"}
  ]
end
```

### FBInk Location

The build system needs access to the FBInk headers and library:

- **Host development**: Set the `FBINK_DIR` environment variable to point to your FBInk source directory. Defaults to `../FBInk` (sibling directory).
- **Nerves / cross-compilation**: When `CROSSCOMPILE` is set, `fbink.h` and `libfbink.so` are expected in the Buildroot sysroot. No extra configuration is needed.

Then fetch and compile:

```bash
mix deps.get
mix compile
```

## Quick Start

```elixir
# Open framebuffer and initialize
{:ok, fd} = FBInk.open()
config = %FBInk.Config{is_quiet: true}
{:ok, _} = FBInk.init(fd, config)

# Print text
{:ok, _} = FBInk.print(fd, "Hello from Elixir!", config)

# Print an image
{:ok, _} = FBInk.print_image(fd, "/path/to/image.png", 0, 0, config)

# Clean up
{:ok, _} = FBInk.close(fd)
```

### Auto-managed File Descriptor

You can pass `-1` (or `FBInk.Constants.fbfd_auto()`) instead of an explicit fd. FBInk will open and close the framebuffer device automatically per call. This is simpler but slower for multiple consecutive operations.

```elixir
config = %FBInk.Config{is_quiet: true}
{:ok, _} = FBInk.init(-1, config)
{:ok, _} = FBInk.print(-1, "Auto-fd mode", config)
```

## Features

### Text Rendering

- **Bitmap fonts** — 31 built-in fonts (IBM, Unscii, Terminus, Unifont, and more) via `FBInk.print/3`
- **OpenType/TrueType fonts** — Full OpenType rendering with configurable size, style, margins, and alignment via `FBInk.add_ot_font/2` and `FBInk.print_ot/4`

### Image Display

- Render PNG, JPEG, BMP, TGA, GIF, and PNM images via `FBInk.print_image/5`
- Raw pixel data rendering via `FBInk.print_raw_data/7`

### Drawing Primitives

- Fill rectangles with grayscale or RGBA colors
- Get and set individual pixels
- Pack pixel values for direct framebuffer writing

### Screen Management

- Clear screen (`FBInk.cls/2`)
- Screen refresh with waveform mode and dithering control
- Framebuffer dump and restore (with automatic memory management via NIF resources)
- Screen and region inversion
- Rotation helpers (native <-> canonical)

### Platform-Specific

- EPDC wakeup
- Sunxi pen mode and rotation enforcement
- MTK swipe animations, halftone, auto-REAGL, and pen mode
- Input device scanning

### Progress & Activity Bars

- Configurable progress bars via `FBInk.print_progress_bar/3`
- Activity indicators via `FBInk.print_activity_bar/3`

## Configuration

All functions that accept configuration take an `FBInk.Config` struct (or any map with matching keys). The default struct provides sane defaults. See `FBInk.Config` for the full list of 34 configurable options including:

- Positioning (row, column, offsets)
- Font selection and scaling
- Foreground/background colors
- Display flags (inverted, flashing, centered, mirrored, etc.)
- Alignment and padding
- Waveform mode and dithering mode
- Night mode

OpenType-specific configuration uses `FBInk.OTConfig` for margins, font size (pt/px), style, centering, and padding.

## Constants

All FBInk enumerations are exposed as Elixir modules under `FBInk.Constants`:

| Module | Description |
|--------|-------------|
| `FBInk.Constants.Font` | 31 built-in bitmap fonts |
| `FBInk.Constants.WaveformMode` | 34 eInk waveform modes |
| `FBInk.Constants.FGColor` / `BGColor` | Named foreground/background colors |
| `FBInk.Constants.Align` | Text alignment options |
| `FBInk.Constants.FontStyle` | OpenType font styles |
| `FBInk.Constants.DitheringMode` | Hardware dithering modes |
| `FBInk.Constants.PixelFormat` | Framebuffer pixel formats |
| `FBInk.Constants.InputDeviceType` | Input device types for scanning |

See `FBInk.Constants` for the complete list.

## Architecture

```
lib/
├── fbink.ex              # Main public API (45+ functions)
└── fbink/
    ├── nif.ex            # NIF function stubs and loader
    ├── config.ex         # FBInkConfig struct (34 fields)
    ├── ot_config.ex      # OpenType configuration struct
    ├── rect.ex           # Rectangle struct
    └── constants.ex      # All FBInk enums (20 sub-modules)

c_src/
└── fbink_nif.c           # C NIF implementation (63 NIF functions)
```

The C NIF layer handles all marshalling between Elixir maps/structs and FBInk's C structs, returns idiomatic `{:ok, value}` / `{:error, code}` tuples, and uses NIF resource types for safe memory management of framebuffer dumps.

## License

MIT License - Copyright (c) 2026 Marc Lainez. See [LICENSE](LICENSE) for details.
