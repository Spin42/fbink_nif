defmodule FBInk.Constants do
  @moduledoc """
  Constants and enumerations matching the FBInk C header definitions.

  These modules provide named constants for all FBInk enums, so you can
  write readable code instead of raw integer values.
  """

  # ===========================================================================
  # Special fd values
  # ===========================================================================

  @doc "Automatic framebuffer fd handling (open/close per call)."
  def fbfd_auto, do: -1

  @doc "Retrieve last sent update marker."
  def last_marker, do: 0

  # ===========================================================================
  # Feature flags (bitmask)
  # ===========================================================================

  defmodule Feature do
    @moduledoc "Feature flags returned by `FBInk.features/0`."
    import Bitwise

    def minimal, do: 0
    def draw, do: 1 <<< 0
    def bitmap, do: 1 <<< 1
    def fonts, do: 1 <<< 2
    def unifont, do: 1 <<< 3
    def opentype, do: 1 <<< 4
    def image, do: 1 <<< 5
    def button_scan, do: 1 <<< 6
    def input, do: 1 <<< 7

    def full do
      draw() ||| bitmap() ||| fonts() ||| opentype() ||| image() ||| input()
    end
  end

  # ===========================================================================
  # Target platforms
  # ===========================================================================

  defmodule Target do
    @moduledoc "Build target platform identifiers."
    def linux, do: 0
    def kobo, do: 1
    def kindle, do: 2
    def kindle_legacy, do: 3
    def cervantes, do: 4
    def remarkable, do: 5
    def pocketbook, do: 6
  end

  # ===========================================================================
  # Fonts
  # ===========================================================================

  defmodule Font do
    @moduledoc "Built-in bitmap font indices for `FBInk.Config.fontname`."
    def ibm, do: 0
    def unscii, do: 1
    def unscii_alt, do: 2
    def unscii_thin, do: 3
    def unscii_fantasy, do: 4
    def unscii_mcr, do: 5
    def unscii_tall, do: 6
    def block, do: 7
    def leggie, do: 8
    def veggie, do: 9
    def kates, do: 10
    def fkp, do: 11
    def ctrld, do: 12
    def orp, do: 13
    def orpb, do: 14
    def orpi, do: 15
    def scientifica, do: 16
    def scientificab, do: 17
    def scientificai, do: 18
    def terminus, do: 19
    def terminusb, do: 20
    def fatty, do: 21
    def spleen, do: 22
    def tewi, do: 23
    def tewib, do: 24
    def topaz, do: 25
    def microknight, do: 26
    def vga, do: 27
    def unifont, do: 28
    def unifontdw, do: 29
    def cozette, do: 30
  end

  # ===========================================================================
  # Font styles (for OpenType)
  # ===========================================================================

  defmodule FontStyle do
    @moduledoc "Font style for OpenType rendering."
    def regular, do: 0
    def italic, do: 1
    def bold, do: 2
    def bold_italic, do: 3
  end

  # ===========================================================================
  # Alignment
  # ===========================================================================

  defmodule Align do
    @moduledoc "Alignment indices for images and dumps."
    def none, do: 0
    def center, do: 1
    def edge, do: 2
  end

  # ===========================================================================
  # Padding
  # ===========================================================================

  defmodule Padding do
    @moduledoc "Padding modes for OpenType rendering."
    def no_padding, do: 0
    def horizontal, do: 1
    def vertical, do: 2
    def full, do: 3
  end

  # ===========================================================================
  # Foreground colors
  # ===========================================================================

  defmodule FGColor do
    @moduledoc "Foreground color palette indices (0x00 = black, 0xFF = white)."
    def black, do: 0
    def gray1, do: 1
    def gray2, do: 2
    def gray3, do: 3
    def gray4, do: 4
    def gray5, do: 5
    def gray6, do: 6
    def gray7, do: 7
    def gray8, do: 8
    def gray9, do: 9
    def gray_a, do: 10
    def gray_b, do: 11
    def gray_c, do: 12
    def gray_d, do: 13
    def gray_e, do: 14
    def white, do: 15
  end

  # ===========================================================================
  # Background colors (reversed order from FG)
  # ===========================================================================

  defmodule BGColor do
    @moduledoc "Background color palette indices (0 = white, 15 = black)."
    def white, do: 0
    def gray_e, do: 1
    def gray_d, do: 2
    def gray_c, do: 3
    def gray_b, do: 4
    def gray_a, do: 5
    def gray9, do: 6
    def gray8, do: 7
    def gray7, do: 8
    def gray6, do: 9
    def gray5, do: 10
    def gray4, do: 11
    def gray3, do: 12
    def gray2, do: 13
    def gray1, do: 14
    def black, do: 15
  end

  # ===========================================================================
  # Waveform modes
  # ===========================================================================

  defmodule WaveformMode do
    @moduledoc """
    Waveform mode indices for eInk refresh.

    The optimal mode depends on your content type:
    - `:auto` - Let the EPDC decide (default).
    - `:du` - Direct Update (fast, 1-bit black/white only).
    - `:gc16` - Grayscale Clearing 16 (high quality, full flash).
    - `:a2` - Animation mode (fastest, 1-bit).
    - `:gl16` - Grayscale Low-latency 16.
    """
    def auto, do: 0
    def du, do: 1
    def gc16, do: 2
    def gc4, do: 3
    def a2, do: 4
    def gl16, do: 5
    def reagl, do: 6
    def reagld, do: 7
    def gc16_fast, do: 8
    def gl16_fast, do: 9
    def du4, do: 10
    def gl4, do: 11
    def gl16_inv, do: 12
    def gck16, do: 13
    def glkw16, do: 14
    def init, do: 15
    def unknown, do: 16
    def init2, do: 17
    def a2in, do: 18
    def a2out, do: 19
    def gc16hq, do: 20
    def gs16, do: 21
    def gu16, do: 22
    def glk16, do: 23
    def clear, do: 24
    def gc4l, do: 25
    def gcc16, do: 26
    def glrc16, do: 27
    def gc16_partial, do: 28
    def gck16_partial, do: 29
    def dunm, do: 30
    def p2sw, do: 31
    def gcck16, do: 32
    def glrck16, do: 33
  end

  # ===========================================================================
  # Hardware dithering modes
  # ===========================================================================

  defmodule DitheringMode do
    @moduledoc "Hardware dithering mode indices."
    def passthrough, do: 0
    def floyd_steinberg, do: 1
    def atkinson, do: 2
    def ordered, do: 3
    def quant_only, do: 4
    def legacy, do: 255
  end

  # ===========================================================================
  # CFA modes
  # ===========================================================================

  defmodule CFAMode do
    @moduledoc "Color Filter Array post-processing modes (for color panels)."
    def default, do: 0
    def aie_s4, do: 1
    def aie_s7, do: 2
    def aie_s9, do: 3
    def g0, do: 4
    def g1, do: 5
    def g2, do: 6
    def ntx, do: 7
    def ntx_sf, do: 8
    def skip, do: 9
  end

  # ===========================================================================
  # NTX rotation quirks
  # ===========================================================================

  defmodule NTXRota do
    @moduledoc "NTX rotation quirk indices."
    def straight, do: 0
    def all_inverted, do: 1
    def odd_inverted, do: 2
    def sane, do: 3
    def sunxi, do: 4
    def cw_touch, do: 5
    def ccw_touch, do: 6
  end

  # ===========================================================================
  # Sunxi force rotation
  # ===========================================================================

  defmodule SunxiForceRota do
    @moduledoc "Sunxi forced rotation modes (Kobo Elipsa/Sage)."
    def notsup, do: -128
    def current_rota, do: -5
    def current_layout, do: -4
    def portrait, do: -3
    def landscape, do: -2
    def gyro, do: -1
    def ur, do: 0
    def cw, do: 1
    def ud, do: 2
    def ccw, do: 3
    def workbuf, do: 4
  end

  # ===========================================================================
  # MTK swipe directions
  # ===========================================================================

  defmodule MTKSwipeDirection do
    @moduledoc "MTK swipe animation directions (Kindle PW5+)."
    def down, do: 0
    def up, do: 1
    def left, do: 2
    def right, do: 3
  end

  # ===========================================================================
  # MTK halftone modes
  # ===========================================================================

  defmodule MTKHalftone do
    @moduledoc "MTK halftone modes (Kindle PW5+)."
    def disabled, do: 0
    def default_checker_size, do: 1
  end

  # ===========================================================================
  # Pixel formats
  # ===========================================================================

  defmodule PixelFormat do
    @moduledoc "Framebuffer pixel format identifiers."
    def unknown, do: 0
    def y4, do: 4
    def y8, do: 8
    def bgr565, do: 16
    def rgb565, do: 17
    def bgr24, do: 24
    def rgb24, do: 25
    def bgra, do: 32
    def rgba, do: 33
    def bgr32, do: 34
    def rgb32, do: 35
  end

  # ===========================================================================
  # Input device types (bitmask)
  # ===========================================================================

  defmodule InputDeviceType do
    @moduledoc "Input device type bitmask flags for `FBInk.input_scan/3`."
    import Bitwise
    def unknown, do: 0
    def pointingstick, do: 1 <<< 0
    def mouse, do: 1 <<< 1
    def touchpad, do: 1 <<< 2
    def touchscreen, do: 1 <<< 3
    def joystick, do: 1 <<< 4
    def tablet, do: 1 <<< 5
    def key, do: 1 <<< 6
    def keyboard, do: 1 <<< 7
    def accelerometer, do: 1 <<< 8
    def power_button, do: 1 <<< 16
    def sleep_cover, do: 1 <<< 17
    def pagination_buttons, do: 1 <<< 18
    def home_button, do: 1 <<< 19
    def light_button, do: 1 <<< 20
    def menu_button, do: 1 <<< 21
    def dpad, do: 1 <<< 22
    def rotation_event, do: 1 <<< 23
    def scaled_tablet, do: 1 <<< 24
    def volume_buttons, do: 1 <<< 25
    def kindle_frame_tap, do: 1 <<< 26
  end

  # ===========================================================================
  # Input scan settings (bitmask)
  # ===========================================================================

  defmodule InputSettings do
    @moduledoc "Input scan settings bitmask flags."
    import Bitwise
    def scan_only, do: 1 <<< 0
    def open_blocking, do: 1 <<< 1
    def match_all, do: 1 <<< 2
    def exclude_all, do: 1 <<< 3
    def no_recap, do: 1 <<< 4
  end

  # ===========================================================================
  # Reinit return flags
  # ===========================================================================

  defmodule ReinitFlags do
    @moduledoc "Bitmask flags that may be OR'd into `reinit/2` return values."
    import Bitwise
    def ok_bpp_change, do: 1 <<< 9
    def ok_rota_change, do: 1 <<< 10
    def ok_layout_change, do: 1 <<< 11
    def ok_grayscale_change, do: 1 <<< 12
  end

  # ===========================================================================
  # set_fb_info magic values
  # ===========================================================================

  defmodule FBInfoMagic do
    @moduledoc "Magic values for `FBInk.set_fb_info/5`."
    import Bitwise
    def keep_current_rotate, do: 1 <<< 7
    def keep_current_bitdepth, do: 1 <<< 7
    def keep_current_grayscale, do: 1 <<< 7
    def toggle_grayscale, do: 1 <<< 6
  end
end
