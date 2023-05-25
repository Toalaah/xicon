{
  stdenv,
  libX11,
  libpng,
  pkg-config,
}:
stdenv.mkDerivation {
  pname = "xicon";
  version = builtins.replaceStrings ["\n"] [""] (builtins.readFile ./version);

  src = ./.;

  installFlags = ["PREFIX=$(out)"];

  nativeBuildInputs = [pkg-config];

  buildInputs = [
    libX11
    libpng
  ];
}
