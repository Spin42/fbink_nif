defmodule FBInk.Config do
  @moduledoc """
  Configuration struct for FBInk operations.

  Maps directly to FBInk's `FBInkConfig` C struct. All fields default to
  sane zero-values, matching C's zero-initialization behavior.

  ## Fields

    * `:row` - Y axis position (line number). Negative values count from bottom.
    * `:col` - X axis position (column number). Negative values count from right.
    * `:fontmult` - Font scaling multiplier (0 = auto-fit to screen height).
    * `:fontname` - Built-in font index (see `FBInk.Font`).
    * `:is_inverted` - Invert colors when drawing.
    * `:is_flashing` - Request full black flash refresh (UPDATE_MODE_FULL).
    * `:is_cleared` - Clear full screen before drawing.
    * `:is_centered` - Center text horizontally.
    * `:hoffset` - Horizontal pixel offset for fine positioning.
    * `:voffset` - Vertical pixel offset for fine positioning.
    * `:is_halfway` - Vertically center text on screen.
    * `:is_padded` - Pad text line with background color (left fill).
    * `:is_rpadded` - Right-pad text line with background color.
    * `:fg_color` - Foreground color palette index (see `FBInk.Color`).
    * `:bg_color` - Background color palette index (see `FBInk.Color`).
    * `:is_overlay` - Don't draw background; use inverse of existing pixel.
    * `:is_bgless` - Don't draw background pixels.
    * `:is_fgless` - Don't draw foreground pixels.
    * `:no_viewport` - Ignore Kobo viewport/offset corrections.
    * `:is_verbose` - Enable verbose diagnostic output on stdout.
    * `:is_quiet` - Suppress hardware info during init.
    * `:ignore_alpha` - Flatten alpha channel when rendering images.
    * `:halign` - Horizontal alignment for images/dumps (see `FBInk.Align`).
    * `:valign` - Vertical alignment for images/dumps (see `FBInk.Align`).
    * `:scaled_width` - Scale image to this width (0 = no scale, -1 = viewport width).
    * `:scaled_height` - Scale image to this height (0 = no scale, -1 = viewport height).
    * `:wfm_mode` - Waveform mode (see `FBInk.WaveformMode`).
    * `:dithering_mode` - Hardware dithering mode (see `FBInk.DitheringMode`).
    * `:sw_dithering` - Use software ordered dithering for images.
    * `:cfa_mode` - CFA post-processing mode for color panels.
    * `:is_nightmode` - Use hardware inversion via EPDC flag.
    * `:no_refresh` - Skip eInk refresh after drawing.
    * `:no_merge` - Set EINK_NO_MERGE flag (Kobo sunxi only).
    * `:is_animated` - Enable swipe animation (Kindle MTK only).
    * `:saturation_boost` - Image saturation boost percentage (Kaleido panels).
    * `:to_syslog` - Redirect messages to syslog instead of stdout.
  """

  @type t :: %__MODULE__{
          row: integer(),
          col: integer(),
          fontmult: non_neg_integer(),
          fontname: non_neg_integer(),
          is_inverted: boolean(),
          is_flashing: boolean(),
          is_cleared: boolean(),
          is_centered: boolean(),
          hoffset: integer(),
          voffset: integer(),
          is_halfway: boolean(),
          is_padded: boolean(),
          is_rpadded: boolean(),
          fg_color: non_neg_integer(),
          bg_color: non_neg_integer(),
          is_overlay: boolean(),
          is_bgless: boolean(),
          is_fgless: boolean(),
          no_viewport: boolean(),
          is_verbose: boolean(),
          is_quiet: boolean(),
          ignore_alpha: boolean(),
          halign: non_neg_integer(),
          valign: non_neg_integer(),
          scaled_width: integer(),
          scaled_height: integer(),
          wfm_mode: non_neg_integer(),
          dithering_mode: non_neg_integer(),
          sw_dithering: boolean(),
          cfa_mode: non_neg_integer(),
          is_nightmode: boolean(),
          no_refresh: boolean(),
          no_merge: boolean(),
          is_animated: boolean(),
          saturation_boost: non_neg_integer(),
          to_syslog: boolean()
        }

  defstruct row: 0,
            col: 0,
            fontmult: 0,
            fontname: 0,
            is_inverted: false,
            is_flashing: false,
            is_cleared: false,
            is_centered: false,
            hoffset: 0,
            voffset: 0,
            is_halfway: false,
            is_padded: false,
            is_rpadded: false,
            fg_color: 0,
            bg_color: 0,
            is_overlay: false,
            is_bgless: false,
            is_fgless: false,
            no_viewport: false,
            is_verbose: false,
            is_quiet: false,
            ignore_alpha: false,
            halign: 0,
            valign: 0,
            scaled_width: 0,
            scaled_height: 0,
            wfm_mode: 0,
            dithering_mode: 0,
            sw_dithering: false,
            cfa_mode: 0,
            is_nightmode: false,
            no_refresh: false,
            no_merge: false,
            is_animated: false,
            saturation_boost: 0,
            to_syslog: false

  @doc """
  Convert the struct to a plain map for passing to the NIF.
  """
  @spec to_map(t()) :: map()
  def to_map(%__MODULE__{} = config) do
    Map.from_struct(config)
  end
end
