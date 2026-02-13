defmodule FBInk do
  @moduledoc """
  Elixir NIF bindings for FBInk (FrameBuffer eInker).

  FBInk is a C library for printing text and images to eInk Linux framebuffer
  devices. It supports Kobo, Kindle, reMarkable, PocketBook, and BQ Cervantes
  e-readers, as well as generic Linux framebuffer devices.

  ## Quick Start

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

  ## Auto-managed fd

  You can use `FBInk.Constants.fbfd_auto()` (-1) instead of an explicit fd.
  FBInk will open and close the framebuffer device automatically per call.
  This is simpler but slower for multiple consecutive operations.

      config = %FBInk.Config{is_quiet: true}
      {:ok, _} = FBInk.init(-1, config)
      {:ok, _} = FBInk.print(-1, "Auto-fd mode", config)

  ## Nerves Integration

  This library is designed to work with Nerves. Add it to your `mix.exs` deps:

      {:fbink_nif, path: "../fbink_nif"}

  Set the `FBINK_DIR` environment variable if the FBInk source is not in the
  default sibling directory location. For cross-compilation, ensure your Nerves
  toolchain's CC is available and libfbink is built for the target architecture.

  ## Configuration

  All FBInk functions that accept configuration take an `FBInk.Config` struct
  (or any map with matching keys). See `FBInk.Config` for the full list of
  options. Zero-initialization (the default struct) provides sane defaults.
  """

  alias FBInk.NIF

  # ---------------------------------------------------------------------------
  # Type definitions
  # ---------------------------------------------------------------------------

  @type fbfd :: integer()
  @type config :: FBInk.Config.t() | map()
  @type ot_config :: FBInk.OTConfig.t() | map()
  @type rect :: FBInk.Rect.t() | map()
  @type dump_ref :: reference()
  @type ok_int :: {:ok, integer()} | {:error, integer()}

  # ---------------------------------------------------------------------------
  # Helpers
  # ---------------------------------------------------------------------------

  defp to_config_map(%FBInk.Config{} = c), do: Map.from_struct(c)
  defp to_config_map(%{} = m), do: m

  defp to_ot_config_map(%FBInk.OTConfig{} = c), do: Map.from_struct(c)
  defp to_ot_config_map(%{} = m), do: m

  defp to_rect_map(%FBInk.Rect{} = r), do: Map.from_struct(r)
  defp to_rect_map(%{} = m), do: m
  defp to_rect_map(nil), do: nil

  defp bool_to_int(true), do: 1
  defp bool_to_int(false), do: 0
  defp bool_to_int(val) when is_integer(val), do: val

  # ---------------------------------------------------------------------------
  # Info Functions
  # ---------------------------------------------------------------------------

  @doc """
  Return the FBInk library version string.

  ## Example

      FBInk.version()
      #=> "v1.25.0"
  """
  @spec version() :: charlist()
  def version, do: NIF.nif_version()

  @doc """
  Return the build target platform as an integer.

  See `FBInk.Constants.Target` for the mapping.
  """
  @spec target() :: non_neg_integer()
  def target, do: NIF.nif_target()

  @doc """
  Return a bitmask of enabled features in this build.

  See `FBInk.Constants.Feature` for the flag definitions.

  ## Example

      features = FBInk.features()
      has_image = Bitwise.band(features, FBInk.Constants.Feature.image()) != 0
  """
  @spec features() :: non_neg_integer()
  def features, do: NIF.nif_features()

  # ---------------------------------------------------------------------------
  # Lifecycle
  # ---------------------------------------------------------------------------

  @doc """
  Open the framebuffer device and return a file descriptor.

  Returns `{:ok, fd}` on success. The fd should be passed to subsequent
  FBInk calls and closed with `close/1` when done.

  ## Example

      {:ok, fd} = FBInk.open()
  """
  @spec open() :: {:ok, fbfd()} | {:error, integer()}
  def open, do: NIF.nif_open()

  @doc """
  Close a previously opened framebuffer file descriptor.
  """
  @spec close(fbfd()) :: ok_int()
  def close(fbfd), do: NIF.nif_close(fbfd)

  @doc """
  Initialize FBInk for the given framebuffer fd.

  This must be called at least once before any drawing operation.
  It identifies the device, probes the framebuffer, and caches relevant info.

  Pass `FBInk.Constants.fbfd_auto()` (-1) for automatic fd management.

  ## Example

      {:ok, fd} = FBInk.open()
      {:ok, _} = FBInk.init(fd, %FBInk.Config{is_quiet: true})
  """
  @spec init(fbfd(), config()) :: ok_int()
  def init(fbfd, config) do
    NIF.nif_init(fbfd, to_config_map(config))
  end

  @doc """
  Reinitialize FBInk after a framebuffer state change (e.g., rotation or
  bitdepth change).

  Returns flags indicating what changed (see `FBInk.Constants.ReinitFlags`).
  """
  @spec reinit(fbfd(), config()) :: ok_int()
  def reinit(fbfd, config) do
    NIF.nif_reinit(fbfd, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # State
  # ---------------------------------------------------------------------------

  @doc """
  Get a snapshot of the current FBInk internal state as a map.

  Returns a map with all fields from the C `FBInkState` struct.

  ## Example

      state = FBInk.get_state(%FBInk.Config{})
      state.screen_width  #=> 1264
      state.screen_height #=> 1680
  """
  @spec get_state(config()) :: map()
  def get_state(config) do
    NIF.nif_get_state(to_config_map(config))
  end

  @doc """
  Dump the current FBInk state to stdout (for debugging).
  """
  @spec state_dump(config()) :: :ok
  def state_dump(config) do
    NIF.nif_state_dump(to_config_map(config))
  end

  @doc """
  Get the rectangle of the last drawing operation.

  If `rotated` is true, coordinates are in canonical rotation space.
  """
  @spec get_last_rect(boolean()) :: map()
  def get_last_rect(rotated \\ false) do
    NIF.nif_get_last_rect(bool_to_int(rotated))
  end

  @doc """
  Get the marker of the last eInk update submission.
  """
  @spec get_last_marker() :: non_neg_integer()
  def get_last_marker, do: NIF.nif_get_last_marker()

  @doc """
  Check if the framebuffer is quirky (deprecated).
  """
  @spec fb_quirky?() :: boolean()
  def fb_quirky?, do: NIF.nif_is_fb_quirky()

  # ---------------------------------------------------------------------------
  # Configuration Updates
  # ---------------------------------------------------------------------------

  @doc """
  Update FBInk's internal verbosity flags from the config.
  """
  @spec update_verbosity(config()) :: :ok
  def update_verbosity(config) do
    NIF.nif_update_verbosity(to_config_map(config))
  end

  @doc """
  Update the internal pen colors from the config's fg_color/bg_color fields.
  """
  @spec update_pen_colors(config()) :: ok_int()
  def update_pen_colors(config) do
    NIF.nif_update_pen_colors(to_config_map(config))
  end

  @doc """
  Set the foreground pen to a specific gray value (0-255).

  If `quantize` is true, the value is quantized to the nearest palette entry.
  If `update` is true, the internal pen state is immediately updated.
  """
  @spec set_fg_pen_gray(non_neg_integer(), boolean(), boolean()) :: ok_int()
  def set_fg_pen_gray(y, quantize \\ false, update \\ true) do
    NIF.nif_set_fg_pen_gray(y, bool_to_int(quantize), bool_to_int(update))
  end

  @doc """
  Set the background pen to a specific gray value (0-255).
  """
  @spec set_bg_pen_gray(non_neg_integer(), boolean(), boolean()) :: ok_int()
  def set_bg_pen_gray(y, quantize \\ false, update \\ true) do
    NIF.nif_set_bg_pen_gray(y, bool_to_int(quantize), bool_to_int(update))
  end

  @doc """
  Set the foreground pen to a specific RGBA color.
  """
  @spec set_fg_pen_rgba(
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          boolean(),
          boolean()
        ) :: ok_int()
  def set_fg_pen_rgba(r, g, b, a, quantize \\ false, update \\ true) do
    NIF.nif_set_fg_pen_rgba(r, g, b, a, bool_to_int(quantize), bool_to_int(update))
  end

  @doc """
  Set the background pen to a specific RGBA color.
  """
  @spec set_bg_pen_rgba(
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          boolean(),
          boolean()
        ) :: ok_int()
  def set_bg_pen_rgba(r, g, b, a, quantize \\ false, update \\ true) do
    NIF.nif_set_bg_pen_rgba(r, g, b, a, bool_to_int(quantize), bool_to_int(update))
  end

  # ---------------------------------------------------------------------------
  # Text Printing (Fixed-Cell Bitmap Fonts)
  # ---------------------------------------------------------------------------

  @doc """
  Print a string to the framebuffer using a built-in bitmap font.

  The `config` controls positioning (row/col), font selection, colors,
  alignment, and refresh behavior.

  Returns `{:ok, rows_printed}` on success (number of rows the string occupied).

  ## Example

      {:ok, fd} = FBInk.open()
      FBInk.init(fd, %FBInk.Config{})
      FBInk.print(fd, "Hello World!", %FBInk.Config{
        row: 5,
        fontname: FBInk.Constants.Font.terminus(),
        fontmult: 2,
        is_centered: true
      })
  """
  @spec print(fbfd(), iodata(), config()) :: ok_int()
  def print(fbfd, string, config) do
    NIF.nif_print(fbfd, string, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Text Printing (OpenType/TrueType)
  # ---------------------------------------------------------------------------

  @doc """
  Load an OpenType/TrueType font file for a given style.

  Must be called before `print_ot/4`. You can load up to 4 fonts
  (regular, italic, bold, bold_italic).

  ## Example

      FBInk.add_ot_font("/usr/share/fonts/noto/NotoSans-Regular.ttf",
                         FBInk.Constants.FontStyle.regular())
  """
  @spec add_ot_font(String.t(), integer()) :: ok_int()
  def add_ot_font(filename, style) do
    NIF.nif_add_ot_font(filename, style)
  end

  @doc """
  Free all loaded OpenType fonts.
  """
  @spec free_ot_fonts() :: ok_int()
  def free_ot_fonts, do: NIF.nif_free_ot_fonts()

  @doc """
  Print a string using a loaded OpenType font.

  Returns `{:ok, rv, fit}` where `fit` is a map with rendering metrics:
  - `:computed_lines` - Lines the text would need
  - `:rendered_lines` - Lines actually rendered
  - `:bbox` - Bounding box `%{width: w, height: h}`
  - `:truncated` - Whether text was truncated

  ## Example

      FBInk.add_ot_font("/path/to/font.ttf", FBInk.Constants.FontStyle.regular())
      {:ok, _, fit} = FBInk.print_ot(fd, "Hello OT!", %FBInk.OTConfig{
        size_px: 24,
        is_centered: true,
        margins: %{top: 100, bottom: 0, left: 0, right: 0}
      }, %FBInk.Config{})
  """
  @spec print_ot(fbfd(), iodata(), ot_config(), config()) ::
          {:ok, integer(), map()} | {:error, integer()}
  def print_ot(fbfd, string, ot_config, config) do
    NIF.nif_print_ot(fbfd, string, to_ot_config_map(ot_config), to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Progress & Activity Bars
  # ---------------------------------------------------------------------------

  @doc """
  Draw a progress bar at the position specified by the config.

  `percentage` is 0-100.
  """
  @spec print_progress_bar(fbfd(), non_neg_integer(), config()) :: ok_int()
  def print_progress_bar(fbfd, percentage, config) do
    NIF.nif_print_progress_bar(fbfd, percentage, to_config_map(config))
  end

  @doc """
  Draw an activity bar (indefinite progress indicator).

  `progress` is an incrementing counter that controls the bar position.
  """
  @spec print_activity_bar(fbfd(), non_neg_integer(), config()) :: ok_int()
  def print_activity_bar(fbfd, progress, config) do
    NIF.nif_print_activity_bar(fbfd, progress, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Image Rendering
  # ---------------------------------------------------------------------------

  @doc """
  Print an image file (PNG, JPEG, BMP, TGA, GIF, PNM) to the framebuffer.

  `x_off` and `y_off` are pixel offsets for positioning. Use the config's
  `halign`/`valign` fields for alignment, and `scaled_width`/`scaled_height`
  for scaling.

  ## Example

      FBInk.print_image(fd, "/mnt/onboard/cover.png", 0, 0, %FBInk.Config{
        halign: FBInk.Constants.Align.center(),
        valign: FBInk.Constants.Align.center()
      })
  """
  @spec print_image(fbfd(), String.t(), integer(), integer(), config()) :: ok_int()
  def print_image(fbfd, filename, x_off, y_off, config) do
    NIF.nif_print_image(fbfd, filename, x_off, y_off, to_config_map(config))
  end

  @doc """
  Print raw pixel data to the framebuffer.

  `data` is a binary containing the raw pixel data. `w` and `h` are the
  dimensions. The pixel format must match the framebuffer's current format.
  """
  @spec print_raw_data(fbfd(), binary(), integer(), integer(), integer(), integer(), config()) ::
          ok_int()
  def print_raw_data(fbfd, data, w, h, x_off, y_off, config) do
    NIF.nif_print_raw_data(fbfd, data, w, h, x_off, y_off, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Screen Clear
  # ---------------------------------------------------------------------------

  @doc """
  Clear the screen (or a rectangular region).

  `rect` can be an `FBInk.Rect` struct, a map, or `nil` for full screen.
  `no_rota` bypasses rotation handling when true.

  ## Example

      # Clear full screen
      FBInk.cls(fd, %FBInk.Config{}, nil, false)

      # Clear a region
      FBInk.cls(fd, %FBInk.Config{}, %FBInk.Rect{left: 0, top: 0, width: 200, height: 200}, false)
  """
  @spec cls(fbfd(), config(), rect() | nil, boolean()) :: ok_int()
  def cls(fbfd, config, rect \\ nil, no_rota \\ false) do
    NIF.nif_cls(fbfd, to_config_map(config), to_rect_map(rect), bool_to_int(no_rota))
  end

  @doc """
  Clear a grid-aligned region (in character cell coordinates).
  """
  @spec grid_clear(fbfd(), non_neg_integer(), non_neg_integer(), config()) :: ok_int()
  def grid_clear(fbfd, cols, rows, config) do
    NIF.nif_grid_clear(fbfd, cols, rows, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Refresh
  # ---------------------------------------------------------------------------

  @doc """
  Trigger an eInk refresh for a specific region.

  Coordinates are in pixels. Pass all zeros for a full-screen refresh.
  The config controls waveform mode, dithering, etc.
  """
  @spec refresh(
          fbfd(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          config()
        ) :: ok_int()
  def refresh(fbfd, region_top, region_left, region_width, region_height, config) do
    NIF.nif_refresh(
      fbfd,
      region_top,
      region_left,
      region_width,
      region_height,
      to_config_map(config)
    )
  end

  @doc """
  Trigger an eInk refresh for a rectangle.
  """
  @spec refresh_rect(fbfd(), rect(), config()) :: ok_int()
  def refresh_rect(fbfd, rect, config) do
    NIF.nif_refresh_rect(fbfd, to_rect_map(rect), to_config_map(config))
  end

  @doc """
  Trigger an eInk refresh for a grid-aligned region.
  """
  @spec grid_refresh(fbfd(), non_neg_integer(), non_neg_integer(), config()) :: ok_int()
  def grid_refresh(fbfd, cols, rows, config) do
    NIF.nif_grid_refresh(fbfd, cols, rows, to_config_map(config))
  end

  @doc """
  Wait for an update submission to be accepted by the EPDC.
  """
  @spec wait_for_submission(fbfd(), non_neg_integer()) :: ok_int()
  def wait_for_submission(fbfd, marker) do
    NIF.nif_wait_for_submission(fbfd, marker)
  end

  @doc """
  Wait for an update to complete rendering on the eInk display.
  """
  @spec wait_for_complete(fbfd(), non_neg_integer()) :: ok_int()
  def wait_for_complete(fbfd, marker) do
    NIF.nif_wait_for_complete(fbfd, marker)
  end

  @doc """
  Wait for any pending update to complete (Kindle MTK only).
  """
  @spec wait_for_any_complete(fbfd()) :: ok_int()
  def wait_for_any_complete(fbfd) do
    NIF.nif_wait_for_any_complete(fbfd)
  end

  # ---------------------------------------------------------------------------
  # Dump & Restore
  # ---------------------------------------------------------------------------

  @doc """
  Dump the entire framebuffer contents to an opaque resource.

  Returns `{:ok, dump_ref}` where `dump_ref` is a NIF resource that
  holds the pixel data. Use `restore/3` to restore it, or
  `get_dump_data/1` to extract the raw pixels.
  """
  @spec dump(fbfd()) :: {:ok, dump_ref()} | {:error, integer()}
  def dump(fbfd), do: NIF.nif_dump(fbfd)

  @doc """
  Dump a rectangular region of the framebuffer.
  """
  @spec region_dump(
          fbfd(),
          integer(),
          integer(),
          non_neg_integer(),
          non_neg_integer(),
          config()
        ) :: {:ok, dump_ref()} | {:error, integer()}
  def region_dump(fbfd, x_off, y_off, w, h, config) do
    NIF.nif_region_dump(fbfd, x_off, y_off, w, h, to_config_map(config))
  end

  @doc """
  Dump a rect-defined region of the framebuffer.
  """
  @spec rect_dump(fbfd(), rect()) :: {:ok, dump_ref()} | {:error, integer()}
  def rect_dump(fbfd, rect) do
    NIF.nif_rect_dump(fbfd, to_rect_map(rect))
  end

  @doc """
  Restore a previously dumped framebuffer region.
  """
  @spec restore(fbfd(), config(), dump_ref()) :: ok_int()
  def restore(fbfd, config, dump_ref) do
    NIF.nif_restore(fbfd, to_config_map(config), dump_ref)
  end

  @doc """
  Free the pixel data held by a dump resource.

  This is called automatically when the resource is garbage collected,
  but you can call it explicitly to free memory sooner.
  """
  @spec free_dump_data(dump_ref()) :: :ok
  def free_dump_data(dump_ref), do: NIF.nif_free_dump_data(dump_ref)

  @doc """
  Extract the dump data as an Elixir map with a binary pixel payload.

  Returns `{:ok, dump_info}` where `dump_info` contains:
  - `:data` - Raw pixel data as a binary
  - `:stride` - Scanline stride in bytes
  - `:size` - Total data size in bytes
  - `:area` - Dumped region as `%{left:, top:, width:, height:}`
  - `:clip` - Clip region
  - `:rota` - Rotation at dump time
  - `:bpp` - Bits per pixel
  - `:is_full` - Whether this is a full-screen dump
  """
  @spec get_dump_data(dump_ref()) :: {:ok, map()} | {:error, atom()}
  def get_dump_data(dump_ref), do: NIF.nif_get_dump_data(dump_ref)

  # ---------------------------------------------------------------------------
  # Screen Inversion
  # ---------------------------------------------------------------------------

  @doc """
  Invert the entire screen contents.
  """
  @spec invert_screen(fbfd(), config()) :: ok_int()
  def invert_screen(fbfd, config) do
    NIF.nif_invert_screen(fbfd, to_config_map(config))
  end

  @doc """
  Invert the pixels in a rectangular region.
  """
  @spec invert_rect(fbfd(), rect(), boolean()) :: ok_int()
  def invert_rect(fbfd, rect, no_rota \\ false) do
    NIF.nif_invert_rect(fbfd, to_rect_map(rect), bool_to_int(no_rota))
  end

  # ---------------------------------------------------------------------------
  # Rotation Helpers
  # ---------------------------------------------------------------------------

  @doc """
  Convert a native (kernel) rotation value to FBInk's canonical rotation.
  """
  @spec rota_native_to_canonical(non_neg_integer()) :: non_neg_integer()
  def rota_native_to_canonical(rotate) do
    NIF.nif_rota_native_to_canonical(rotate)
  end

  @doc """
  Convert an FBInk canonical rotation value to native (kernel) rotation.
  """
  @spec rota_canonical_to_native(non_neg_integer()) :: non_neg_integer()
  def rota_canonical_to_native(rotate) do
    NIF.nif_rota_canonical_to_native(rotate)
  end

  # ---------------------------------------------------------------------------
  # Framebuffer Info
  # ---------------------------------------------------------------------------

  @doc """
  Set framebuffer parameters (rotation, bitdepth, grayscale).

  Use `FBInk.Constants.FBInfoMagic` values to keep current settings.
  """
  @spec set_fb_info(fbfd(), non_neg_integer(), non_neg_integer(), non_neg_integer(), config()) ::
          ok_int()
  def set_fb_info(fbfd, rota, bpp, grayscale, config) do
    NIF.nif_set_fb_info(fbfd, rota, bpp, grayscale, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # Drawing Primitives
  # ---------------------------------------------------------------------------

  @doc """
  Fill a rectangle with a grayscale color (0-255).
  """
  @spec fill_rect_gray(fbfd(), config(), rect(), boolean(), non_neg_integer()) :: ok_int()
  def fill_rect_gray(fbfd, config, rect, no_rota \\ false, y) do
    NIF.nif_fill_rect_gray(
      fbfd,
      to_config_map(config),
      to_rect_map(rect),
      bool_to_int(no_rota),
      y
    )
  end

  @doc """
  Fill a rectangle with an RGBA color.
  """
  @spec fill_rect_rgba(
          fbfd(),
          config(),
          rect(),
          boolean(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer()
        ) :: ok_int()
  def fill_rect_rgba(fbfd, config, rect, no_rota \\ false, r, g, b, a) do
    NIF.nif_fill_rect_rgba(
      fbfd,
      to_config_map(config),
      to_rect_map(rect),
      bool_to_int(no_rota),
      r,
      g,
      b,
      a
    )
  end

  @doc """
  Set a single pixel to a grayscale value.
  """
  @spec put_pixel_gray(fbfd(), non_neg_integer(), non_neg_integer(), non_neg_integer()) ::
          ok_int()
  def put_pixel_gray(fbfd, x, y, v) do
    NIF.nif_put_pixel_gray(fbfd, x, y, v)
  end

  @doc """
  Set a single pixel to an RGBA color.
  """
  @spec put_pixel_rgba(
          fbfd(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer()
        ) :: ok_int()
  def put_pixel_rgba(fbfd, x, y, r, g, b, a) do
    NIF.nif_put_pixel_rgba(fbfd, x, y, r, g, b, a)
  end

  @doc """
  Read a pixel's RGBA values at the given coordinates.

  Returns `{:ok, {r, g, b, a}}` on success.
  """
  @spec get_pixel(fbfd(), non_neg_integer(), non_neg_integer()) ::
          {:ok, {non_neg_integer(), non_neg_integer(), non_neg_integer(), non_neg_integer()}}
          | {:error, integer()}
  def get_pixel(fbfd, x, y) do
    NIF.nif_get_pixel(fbfd, x, y)
  end

  @doc """
  Pack a grayscale value into a native pixel value for the current fb format.
  """
  @spec pack_pixel_gray(non_neg_integer()) :: {:ok, non_neg_integer()} | {:error, integer()}
  def pack_pixel_gray(y), do: NIF.nif_pack_pixel_gray(y)

  @doc """
  Pack RGBA values into a native pixel value for the current fb format.
  """
  @spec pack_pixel_rgba(
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer(),
          non_neg_integer()
        ) :: {:ok, non_neg_integer()} | {:error, integer()}
  def pack_pixel_rgba(r, g, b, a), do: NIF.nif_pack_pixel_rgba(r, g, b, a)

  # ---------------------------------------------------------------------------
  # EPDC Wakeup
  # ---------------------------------------------------------------------------

  @doc """
  Wake up the EPDC (Kobo Mk.8+ only).

  Some Kobo devices put the EPDC to sleep aggressively. Call this before
  drawing if you suspect the EPDC is asleep.
  """
  @spec wakeup_epdc() :: ok_int()
  def wakeup_epdc, do: NIF.nif_wakeup_epdc()

  # ---------------------------------------------------------------------------
  # Sunxi-Specific (Kobo Elipsa/Sage)
  # ---------------------------------------------------------------------------

  @doc """
  Toggle NTX pen mode on sunxi Kobo devices.
  """
  @spec sunxi_toggle_ntx_pen_mode(fbfd(), boolean()) :: ok_int()
  def sunxi_toggle_ntx_pen_mode(fbfd, toggle) do
    NIF.nif_sunxi_toggle_ntx_pen_mode(fbfd, bool_to_int(toggle))
  end

  @doc """
  Enforce a specific rotation on sunxi Kobo devices.

  See `FBInk.Constants.SunxiForceRota` for mode values.
  """
  @spec sunxi_ntx_enforce_rota(fbfd(), integer(), config()) :: ok_int()
  def sunxi_ntx_enforce_rota(fbfd, mode, config) do
    NIF.nif_sunxi_ntx_enforce_rota(fbfd, mode, to_config_map(config))
  end

  # ---------------------------------------------------------------------------
  # MTK-Specific (Kindle PW5+)
  # ---------------------------------------------------------------------------

  @doc """
  Set swipe animation parameters (Kindle MTK only).

  See `FBInk.Constants.MTKSwipeDirection` for direction values.
  """
  @spec mtk_set_swipe_data(integer(), non_neg_integer()) :: ok_int()
  def mtk_set_swipe_data(direction, steps) do
    NIF.nif_mtk_set_swipe_data(direction, steps)
  end

  @doc """
  Configure halftone mode (Kindle MTK only).

  `exclude_regions` is a list of up to 2 rect maps to exclude.
  """
  @spec mtk_set_halftone(fbfd(), [rect()], integer()) :: ok_int()
  def mtk_set_halftone(fbfd, exclude_regions, halftone_size) do
    regions = Enum.map(exclude_regions || [], &to_rect_map/1)
    NIF.nif_mtk_set_halftone(fbfd, regions, halftone_size)
  end

  @doc """
  Toggle automatic REAGL mode (Kindle MTK only).
  """
  @spec mtk_toggle_auto_reagl(fbfd(), boolean()) :: ok_int()
  def mtk_toggle_auto_reagl(fbfd, toggle) do
    NIF.nif_mtk_toggle_auto_reagl(fbfd, bool_to_int(toggle))
  end

  @doc """
  Toggle pen mode (Kindle MTK only).
  """
  @spec mtk_toggle_pen_mode(fbfd(), boolean()) :: ok_int()
  def mtk_toggle_pen_mode(fbfd, toggle) do
    NIF.nif_mtk_toggle_pen_mode(fbfd, bool_to_int(toggle))
  end

  # ---------------------------------------------------------------------------
  # Input Device Scanning
  # ---------------------------------------------------------------------------

  @doc """
  Scan for input devices matching the given type filters.

  `match_types` and `exclude_types` are bitmasks from `FBInk.Constants.InputDeviceType`.
  `settings` is a bitmask from `FBInk.Constants.InputSettings`.

  Returns `{:ok, [device_map]}` where each device has `:type`, `:fd`,
  `:matched`, `:name`, and `:path` fields.

  ## Example

      alias FBInk.Constants.InputDeviceType

      {:ok, devices} = FBInk.input_scan(
        InputDeviceType.touchscreen(),
        0,
        FBInk.Constants.InputSettings.scan_only()
      )
  """
  @spec input_scan(non_neg_integer(), non_neg_integer(), non_neg_integer()) ::
          {:ok, [map()]}
  def input_scan(match_types, exclude_types, settings) do
    NIF.nif_input_scan(match_types, exclude_types, settings)
  end

  @doc """
  Check a specific input device file against type filters.

  Returns `{:ok, device_map}` if matched, `{:error, :not_found}` otherwise.
  """
  @spec input_check(String.t(), non_neg_integer(), non_neg_integer(), non_neg_integer()) ::
          {:ok, map()} | {:error, :not_found}
  def input_check(filepath, match_types, exclude_types, settings) do
    NIF.nif_input_check(filepath, match_types, exclude_types, settings)
  end

  # ---------------------------------------------------------------------------
  # Button Scan (Deprecated)
  # ---------------------------------------------------------------------------

  @doc """
  Scan for the Connect button on Kobo's USB screen (deprecated).
  """
  @spec button_scan(fbfd(), boolean(), boolean()) :: ok_int()
  def button_scan(fbfd, press_button, nosleep) do
    NIF.nif_button_scan(fbfd, bool_to_int(press_button), bool_to_int(nosleep))
  end

  @doc """
  Wait for USBMS processing to complete (deprecated).
  """
  @spec wait_for_usbms_processing(fbfd(), boolean()) :: ok_int()
  def wait_for_usbms_processing(fbfd, force_unplug) do
    NIF.nif_wait_for_usbms_processing(fbfd, bool_to_int(force_unplug))
  end
end
