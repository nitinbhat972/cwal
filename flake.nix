{
  description = "CWAL - Blazing-fast pywal-like color palette generator written in C";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      packages.default = pkgs.stdenv.mkDerivation {
        pname = "cwal";
        version = "0.5.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
        ];

        buildInputs = with pkgs; [
          imagemagick
          libimagequant
          lua
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
        ];

        # Use standard install phase from CMake
        # No need for manual cp if CMakeLists.txt has install() rules
        
        meta = with pkgs.lib; {
          description = "Blazing-fast pywal-like color palette generator written in C";
          homepage = "https://github.com/nitinbhat972/cwal";
          license = licenses.gpl3Only;
          platforms = platforms.linux ++ platforms.darwin;
          mainProgram = "cwal";
        };
      };

      devShells.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
        ];
        buildInputs = with pkgs; [
          imagemagick
          libimagequant
          lua
          gdb
          valgrind
        ];
      };
    });
}
