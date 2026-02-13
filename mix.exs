defmodule FBInk.MixProject do
  use Mix.Project

  @version "0.1.0"
  @source_url "https://github.com/mlainez/fbink_nif"

  def project do
    [
      app: :fbink_nif,
      version: @version,
      elixir: "~> 1.14",
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_targets: ["all"],
      make_clean: ["clean"],
      make_env: make_env(),
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      description: "Elixir NIF bindings for FBInk (FrameBuffer eInker)",
      package: package()
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.8", runtime: false},
      {:ex_doc, "~> 0.31", only: :dev, runtime: false}
    ]
  end

  defp package do
    [
      files: ["lib", "c_src", "Makefile", "mix.exs", "README.md", "LICENSE"],
      licenses: ["MIT"],
      links: %{"GitHub" => @source_url}
    ]
  end

  defp make_env do
    env = %{"MIX_ENV" => to_string(Mix.env())}

    # Nerves sets ERTS_INCLUDE_DIR for cross-compile; forward it to Make
    env =
      case System.get_env("ERTS_INCLUDE_DIR") do
        nil -> env
        dir -> Map.put(env, "ERTS_INCLUDE_DIR", dir)
      end

    # Only pass FBINK_DIR for host dev builds (not cross-compile).
    # For Nerves, fbink.h and libfbink.so are in the Buildroot sysroot.
    env =
      if System.get_env("CROSSCOMPILE") do
        env
      else
        fbink_dir = System.get_env("FBINK_DIR", Path.expand("../FBInk", __DIR__))
        Map.put(env, "FBINK_DIR", fbink_dir)
      end

    env
  end
end
