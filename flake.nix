{
  description = "xicon flake";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.treefmt-nix.url = "github:numtide/treefmt-nix";

  outputs = {
    flake-utils,
    nixpkgs,
    treefmt-nix,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [];
        config = {};
      };
    in {
      packages.default = pkgs.callPackage ./default.nix {};
      formatter = treefmt-nix.lib.mkWrapper pkgs {
        projectRootFile = "flake.nix";
        programs.alejandra.enable = true;
        programs.clang-format.enable = true;
      };
      devShells.default = with pkgs;
        mkShell {
          buildInputs = [
            clang-tools # lsp-tooling

            xorg.libX11
            libpng
            pkg-config
          ];
        };
    });
}
