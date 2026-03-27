{
  description = "Qt development environment with Bazel";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let 
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    base = pkgs.appimageTools.defaultFhsEnvArgs;
    fhs = pkgs.buildFHSEnv (base // {
      name = "QT";
      targetPkgs = pkgs: with pkgs; [
        bazel_7
        cmake
        gcc13
        clang
        binutils
        qt6.full
        libGL
        libglvnd
        mesa
        clang-tools_18
	      clang
      ];
      
      extraBuildCommands = ''
        mkdir -p usr/bin
        ln -s ${pkgs.gcc13}/bin/gcc usr/bin/gcc
        ln -s ${pkgs.gcc13}/bin/g++ usr/bin/g++
        ln -s ${pkgs.binutils}/bin/ld usr/bin/ld
        
        # Add OpenGL headers to expected paths
        mkdir -p usr/include
        ln -s ${pkgs.libGL.dev}/include/GL usr/include/GL
      '';
      
      profile = ''
        export CC=${pkgs.gcc13}/bin/gcc
        export CXX=${pkgs.gcc13}/bin/g++
        export PATH=${pkgs.gcc13}/bin:${pkgs.binutils}/bin:$PATH
        export QT_PATH=${pkgs.qt6.full}
        export QT_PLUGIN_PATH="$QT_PATH/${pkgs.qt6.qtbase.qtPluginPrefix}"
        export QML2_IMPORT_PATH="$QT_PATH/${pkgs.qt6.qtbase.qtQmlPrefix}"
        
        # Add OpenGL library paths
        export LD_LIBRARY_PATH=${pkgs.libGL}/lib:${pkgs.libglvnd}/lib:${pkgs.mesa}/lib:$LD_LIBRARY_PATH
        export C_INCLUDE_PATH=${pkgs.libGL.dev}/include:$C_INCLUDE_PATH
        export CPLUS_INCLUDE_PATH=${pkgs.libGL.dev}/include:$CPLUS_INCLUDE_PATH
      '';
    });
  in {
    devShells.${system}.default = fhs.env;
  };
}