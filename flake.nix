{
  description = "llvm";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            clang
            llvm
            SDL2
            clang-tools
            gnumake
          ];

          shellHook = ''
            export CPATH="${pkgs.SDL2.dev}/include:$CPATH"
            export CPATH="${pkgs.llvm.dev}/include:$CPATH"
          '';
        };
      });
}

