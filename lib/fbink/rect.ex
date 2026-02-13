defmodule FBInk.Rect do
  @moduledoc """
  Rectangle definition for FBInk operations.

  Maps directly to FBInk's `FBInkRect` C struct.
  """

  @type t :: %__MODULE__{
          left: non_neg_integer(),
          top: non_neg_integer(),
          width: non_neg_integer(),
          height: non_neg_integer()
        }

  defstruct left: 0, top: 0, width: 0, height: 0

  @spec to_map(t()) :: map()
  def to_map(%__MODULE__{} = rect) do
    Map.from_struct(rect)
  end
end
