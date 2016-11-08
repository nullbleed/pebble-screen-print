# pebble-screen-print

A watchface designed by [/u/stegosaurusflex1](https://www.reddit.com/user/stegosaurusflex1) for pebble smartwatches.


### Features

- Show connection state
- Battery level
- Custom colors
- Vibrate on disconnect
- Weather (design improvements and icons needed)
- Shake for Steps (not yet implemented)


### Font

This watchface uses the Porto font from uppertype.


See [this link](http://uppertype.net/ula.htm) for the license.


[Here](http://uppertype.net/fonts/porto.htm) is a link to the font.


### Build

In order to build this watchface you need to follow these steps:

- Install the pebble-fctx package and its compiler.
- Download the Porto font from the provided link
- Convert the Font with [Fontforge](https://github.com/fontforge/fontforge) into a svg-font.
- Convert the svg-font with the fctx-compiler with the following command to a ffont:
    `fctx-compiler -r "[a-zA-Z0-9:] Porto.svg"`
- If you put the `Porto.ffont` into `resources/font/` you are able to build this watchface with
    `pebble build`


### License

code (c) 2016 Nullbleed

Licensed under the GNU Lesser General Public License Version 3 (LGPLv3).
See [LICENSE](./LICENSE) for more details.

-------------------

design (c) 2016 [/u/stegosaurusflex1](https://www.reddit.com/user/stegosaurusflex1)

-------------------

font (c) uppertype

See [this link](http://uppertype.net/ula.htm) for the license.

-------------------

GBitmap-Colour-Palette-Manipulator is licensed under the MIT License and used in this project.

Please follow [this link](https://github.com/rebootsramblings/GBitmap-Colour-Palette-Manipulator) to open the GBitmap-Colour-Palette-Manipulator.
