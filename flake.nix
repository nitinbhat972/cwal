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
        version = "0.1.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
        ];

        buildInputs = with pkgs; [
          imagemagick
          libimagequant
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
        ];

        installPhase = ''
          runHook preInstall

          mkdir -p $out/bin
          cp cwal $out/bin/

          if [ -d ../templates ]; then
            mkdir -p $out/share/cwal
            cp -r ../templates $out/share/cwal/
          fi

          runHook postInstall
        '';

        meta = with pkgs.lib; {
          description = "CWAL - Blazing-fast pywal-like color palette generator written in C";
          license = licenses.gpl3;
          platforms = platforms.linux;
        };
      };

      devShells.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          cmake
          pkg-config
          imagemagick
          libimagequant
          gdb
          valgrind
        ];
      };
    });
}
