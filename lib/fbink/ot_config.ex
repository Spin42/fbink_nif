defmodule FBInk.OTConfig do
  @moduledoc """
  OpenType font rendering configuration.

  Maps directly to FBInk's `FBInkOTConfig` C struct.

  ## Fields

    * `:margins` - Map with `:top`, `:bottom`, `:left`, `:right` pixel margins.
    * `:style` - Default font style (see `FBInk.FontStyle`).
    * `:size_pt` - Font size in points (default 12.0).
    * `:size_px` - Font size in pixels (overrides `:size_pt` when > 0).
    * `:is_centered` - Center text horizontally.
    * `:padding` - Padding mode (see `FBInk.Padding`).
    * `:is_formatted` - Enable bold/italic markdown-like markup.
    * `:compute_only` - Only compute line breaks, don't render.
    * `:no_truncation` - Return error if text won't fit.
  """

  @type t :: %__MODULE__{
          margins: %{top: integer(), bottom: integer(), left: integer(), right: integer()},
          style: integer(),
          size_pt: float(),
          size_px: non_neg_integer(),
          is_centered: boolean(),
          padding: non_neg_integer(),
          is_formatted: boolean(),
          compute_only: boolean(),
          no_truncation: boolean()
        }

  defstruct margins: %{top: 0, bottom: 0, left: 0, right: 0},
            style: 0,
            size_pt: 0.0,
            size_px: 0,
            is_centered: false,
            padding: 0,
            is_formatted: false,
            compute_only: false,
            no_truncation: false

  @spec to_map(t()) :: map()
  def to_map(%__MODULE__{} = config) do
    Map.from_struct(config)
  end
end
