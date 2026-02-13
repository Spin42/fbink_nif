defmodule FBInk.NIF do
  @moduledoc false
  @on_load :load_nif

  def load_nif do
    path = :filename.join(:code.priv_dir(:fbink_nif), ~c"fbink_nif")
    :erlang.load_nif(path, 0)
  end

  # Info
  def nif_version, do: :erlang.nif_error(:not_loaded)
  def nif_target, do: :erlang.nif_error(:not_loaded)
  def nif_features, do: :erlang.nif_error(:not_loaded)

  # Lifecycle
  def nif_open, do: :erlang.nif_error(:not_loaded)
  def nif_close(_fbfd), do: :erlang.nif_error(:not_loaded)
  def nif_init(_fbfd, _config), do: :erlang.nif_error(:not_loaded)
  def nif_reinit(_fbfd, _config), do: :erlang.nif_error(:not_loaded)

  # State
  def nif_get_state(_config), do: :erlang.nif_error(:not_loaded)
  def nif_state_dump(_config), do: :erlang.nif_error(:not_loaded)
  def nif_get_last_rect(_rotated), do: :erlang.nif_error(:not_loaded)
  def nif_get_last_marker, do: :erlang.nif_error(:not_loaded)
  def nif_is_fb_quirky, do: :erlang.nif_error(:not_loaded)

  # Config updates
  def nif_update_verbosity(_config), do: :erlang.nif_error(:not_loaded)
  def nif_update_pen_colors(_config), do: :erlang.nif_error(:not_loaded)
  def nif_set_fg_pen_gray(_y, _quantize, _update), do: :erlang.nif_error(:not_loaded)
  def nif_set_bg_pen_gray(_y, _quantize, _update), do: :erlang.nif_error(:not_loaded)

  def nif_set_fg_pen_rgba(_r, _g, _b, _a, _quantize, _update),
    do: :erlang.nif_error(:not_loaded)

  def nif_set_bg_pen_rgba(_r, _g, _b, _a, _quantize, _update),
    do: :erlang.nif_error(:not_loaded)

  # Text printing
  def nif_print(_fbfd, _string, _config), do: :erlang.nif_error(:not_loaded)
  def nif_add_ot_font(_filename, _style), do: :erlang.nif_error(:not_loaded)
  def nif_free_ot_fonts, do: :erlang.nif_error(:not_loaded)
  def nif_print_ot(_fbfd, _string, _ot_config, _config), do: :erlang.nif_error(:not_loaded)

  # Progress/Activity bars
  def nif_print_progress_bar(_fbfd, _percentage, _config), do: :erlang.nif_error(:not_loaded)
  def nif_print_activity_bar(_fbfd, _progress, _config), do: :erlang.nif_error(:not_loaded)

  # Image rendering
  def nif_print_image(_fbfd, _filename, _x_off, _y_off, _config),
    do: :erlang.nif_error(:not_loaded)

  def nif_print_raw_data(_fbfd, _data, _w, _h, _x_off, _y_off, _config),
    do: :erlang.nif_error(:not_loaded)

  # Screen clear
  def nif_cls(_fbfd, _config, _rect, _no_rota), do: :erlang.nif_error(:not_loaded)
  def nif_grid_clear(_fbfd, _cols, _rows, _config), do: :erlang.nif_error(:not_loaded)

  # Refresh
  def nif_refresh(_fbfd, _top, _left, _width, _height, _config),
    do: :erlang.nif_error(:not_loaded)

  def nif_refresh_rect(_fbfd, _rect, _config), do: :erlang.nif_error(:not_loaded)
  def nif_grid_refresh(_fbfd, _cols, _rows, _config), do: :erlang.nif_error(:not_loaded)
  def nif_wait_for_submission(_fbfd, _marker), do: :erlang.nif_error(:not_loaded)
  def nif_wait_for_complete(_fbfd, _marker), do: :erlang.nif_error(:not_loaded)
  def nif_wait_for_any_complete(_fbfd), do: :erlang.nif_error(:not_loaded)

  # Dump/Restore
  def nif_dump(_fbfd), do: :erlang.nif_error(:not_loaded)

  def nif_region_dump(_fbfd, _x_off, _y_off, _w, _h, _config),
    do: :erlang.nif_error(:not_loaded)

  def nif_rect_dump(_fbfd, _rect), do: :erlang.nif_error(:not_loaded)
  def nif_restore(_fbfd, _config, _dump), do: :erlang.nif_error(:not_loaded)
  def nif_free_dump_data(_dump), do: :erlang.nif_error(:not_loaded)
  def nif_get_dump_data(_dump), do: :erlang.nif_error(:not_loaded)

  # Screen inversion
  def nif_invert_screen(_fbfd, _config), do: :erlang.nif_error(:not_loaded)
  def nif_invert_rect(_fbfd, _rect, _no_rota), do: :erlang.nif_error(:not_loaded)

  # Rotation helpers
  def nif_rota_native_to_canonical(_rotate), do: :erlang.nif_error(:not_loaded)
  def nif_rota_canonical_to_native(_rotate), do: :erlang.nif_error(:not_loaded)

  # Framebuffer info
  def nif_set_fb_info(_fbfd, _rota, _bpp, _grayscale, _config),
    do: :erlang.nif_error(:not_loaded)

  # Drawing primitives
  def nif_fill_rect_gray(_fbfd, _config, _rect, _no_rota, _y),
    do: :erlang.nif_error(:not_loaded)

  def nif_fill_rect_rgba(_fbfd, _config, _rect, _no_rota, _r, _g, _b, _a),
    do: :erlang.nif_error(:not_loaded)

  def nif_put_pixel_gray(_fbfd, _x, _y, _v), do: :erlang.nif_error(:not_loaded)

  def nif_put_pixel_rgba(_fbfd, _x, _y, _r, _g, _b, _a),
    do: :erlang.nif_error(:not_loaded)

  def nif_get_pixel(_fbfd, _x, _y), do: :erlang.nif_error(:not_loaded)
  def nif_pack_pixel_gray(_y), do: :erlang.nif_error(:not_loaded)
  def nif_pack_pixel_rgba(_r, _g, _b, _a), do: :erlang.nif_error(:not_loaded)

  # EPDC wakeup
  def nif_wakeup_epdc, do: :erlang.nif_error(:not_loaded)

  # Sunxi-specific
  def nif_sunxi_toggle_ntx_pen_mode(_fbfd, _toggle), do: :erlang.nif_error(:not_loaded)

  def nif_sunxi_ntx_enforce_rota(_fbfd, _mode, _config),
    do: :erlang.nif_error(:not_loaded)

  # MTK-specific
  def nif_mtk_set_swipe_data(_direction, _steps), do: :erlang.nif_error(:not_loaded)
  def nif_mtk_set_halftone(_fbfd, _exclude_regions, _size), do: :erlang.nif_error(:not_loaded)
  def nif_mtk_toggle_auto_reagl(_fbfd, _toggle), do: :erlang.nif_error(:not_loaded)
  def nif_mtk_toggle_pen_mode(_fbfd, _toggle), do: :erlang.nif_error(:not_loaded)

  # Input scanning
  def nif_input_scan(_match_types, _exclude_types, _settings),
    do: :erlang.nif_error(:not_loaded)

  def nif_input_check(_filepath, _match_types, _exclude_types, _settings),
    do: :erlang.nif_error(:not_loaded)

  # Button scan (deprecated)
  def nif_button_scan(_fbfd, _press_button, _nosleep), do: :erlang.nif_error(:not_loaded)
  def nif_wait_for_usbms_processing(_fbfd, _force_unplug), do: :erlang.nif_error(:not_loaded)
end
