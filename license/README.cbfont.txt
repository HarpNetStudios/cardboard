Original Author:
    Tim (ThomasT) Thomas

Creation Date:
    2012/09/03

Version:
    1.0

Quick Start:
    Build this enhanced version of cbfont using the included build
    instructions, and then execute cbfont --help to display usage.

Overview:
    This is an enhanced version of cbfont.  The most significant new features
    are:
    - Glyph mapping (supports mapping of glyphs from multiple fonts).
    - Digit-only output, to help create the hud capture digit glyphs
      (packages/hud/digit_{blue,red,grey}.png and packages/hud/digit_font.cfg).
    - Color PNG support (with and without alpha), including support for custom
      character foreground and background colors.  (This is needed to
      generate the digit glyphs.)
    - Support for negative inborder and outborder values.  (Negative inborder
      values provide a simple way to "bold" a font.)
    - An option parser with built-in help.
    - Optional fatal error on missing glyphs.

    Minor features:
    - Ability to explicitly set the font name in the config file.
    - Ability to force a font to be of fixed width.
    - Noexec mode (useful for automating font processing, to weed out fonts
      with missing glyphs for example).
    - Version display (including copyright) with --version.
    - Alternate font packing algorithm.  The original algorithm can be used by
      specifying the --sauerpack option.
    - Additional error checking and reporting, especially for Free Type errors.

Build Instructions:
    Build from the CMake file on *nix systems, and the VS project in `src/vcpp`
    for Windows

Examples:
    To create the default font for the SVN release of Sauerbraten, execute
    within the packages/fonts directory:

        cbfont --spacewidth=32 --spaceheight=64 --texdir=packages/fonts \
               default.ttf default

    To map the 'a' character to '@' using the default font (and also set the
    space width and space height to default values), execute:

        cbfont --map='a @' --texdir=packages/fonts default.ttf default

    To create the blue digit file packages/hud/digit_blue.png using the default
    font, execute:

        cbfont --digits --texwidth=128 --texheight=128 \
               --outborder=1 --pad=20 --charwidth=34 --charheight=34 \
               --colortex --bgcolor=3949FF --fgcolor=3949FF \
                default.ttf digit_blue

    For the digit_red.png and digit_grey.png font files you would use FF0909
    and 808080 for the color, respectively.  Note that the digit font
    files have a special config file that must be hand-constructed from one of
    the generated config files (which one is immaterial).  See the existing
    packages/hud/digit_font.cfg file as an example.

    The next example demonstrates how to specify multiple mapping fonts:

        cbfont --digits --texwidth=128 --texheight=128 \
               --outborder=1 --pad=12 --charwidth=34 --charheight=34 \
               --colortex --bgcolor=3949FF --fgcolor=red \
               --map '0 0:U0085' \
               --map '1 0:U0021' \
               --map '2 0:U0091' \
               --map '3 0:U007E' \
               --map '4 0:U0040' \
               --map '5 0:U0025' \
               --map '6 1:U004D' \
               --map '7 1:U004E' \
               --map '8 1:U00B5' \
               --map '9 2:U267B' \
               --altfont=Wingdings.ttf \
               --altfont=AppleSymbols.ttf \
               Webdings.ttf digit_blue

    This command maps the numbers 0 - 9 to various symbols from three different
    fonts.

Testing Done:
    Compiled and tested on Windows XP, Ubuntu 10.10, and
    Mac OS X Snow Leopard 10.6.8.

    Compiled and tested on Windows 10 x64.

Copyright and License:
    README.cbfont.txt:
        Copyright (C) 2024 Tim Thomas, Zsolt Zitting
    cbfont.c:
        Copyright (C) 2024 Quinton Reeves, Lee Salzman, Martin Erik Werner,
        Tim Thomas, Zsolt Zitting

    As with the Sauerbraten and Cardboard source code, README.cbfont.txt and
    cbfont.c are covered by the ZLIB license:

       (http://www.opensource.org/licenses/zlib-license.php)

    Please see the file src/readme_source.txt in the source distribution for
    full details.

    There is NO WARRANTY, to the extent permitted by law.
