{
  description = "CWAL - Blazing-fast pywal-like color palette generator written in C";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        version =
          builtins.replaceStrings [ "\n" ] [ "" ]
          (builtins.readFile ./VERSION);
      in
      {
        packages = rec {
          cwal = pkgs.stdenv.mkDerivation {
            pname = "cwal";
            inherit version;
            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              makeBinaryWrapper
            ];

            buildInputs = with pkgs; [
              imagemagick
              libimagequant
              lua
            ];

            postFixup = ''
              wrapProgram $out/bin/cwal \
                --prefix XDG_DATA_DIRS : $out/share
            '';

            meta = with pkgs.lib; {
              description = "Blazing-fast pywal-like color palette generator written in C";
              homepage = "https://github.com/nitinbhat972/cwal";
              license = licenses.gpl3Only;
              platforms = platforms.unix;
              mainProgram = "cwal";
            };
          };

          default = cwal;
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
          ];
        };
      }
    );
}
