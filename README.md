# Simple Raycaster

![](https://raw.githubusercontent.com/atacoi/Simple-Raycaster/main/walk.gif)

A simple raycaster based off Lode's raycating tutorial built in C with the help of SDL2.
</hr>
Check out the tutorial <a href="https://lodev.org/cgtutor/raycasting.html">here</a>!

<h2>Build Info</h2>

SDL2 is linked to the executable using Frameworks, as is the standard on Macintosh computers.

The source files can be found in ``/src`` directory and headers in ``include``.

A ``.vscode`` folder is provided for compiling and running directly through the Run and Debug tab.

Note that this has only been tested on a M1 chip so it may not work on newer chips. 

<h2>Map Editing</h2>

The map's data can be found in ``file.txt``.

The first line specifies the map's width and height (assumed to be a perfect square).

The subsequent lines specify the map's structure. 

Each block in the map is specified by a 32-bit ``integer`` value, as in the Tile Info section.

The map must be layed out row by row with newlines (``\n``) separating each row.

<h2>Tile Info</h2>

Tile assets are located in ``tile_sheet.png``.

All tiles are ``64 x 64`` pixels.

Each tile is separated by exactly ``1`` pixel (including borders).

The following denotes the meaning of each ``integer`` value found in ``file.txt``:

<ul>
  <li> 0 - empty tile </li>
  <li> 1 - top left tile (gray brick)  </li>
  <li> 2 - top middle tile (flipped gray brick)  </li>
  <li> 3 - top right tile (dark blue brick)  </li>
  <li> 4 - bottom left tile (cell)  </li>
  <li> 5 - bottom middle tile (cell with skeleton)  </li>
  <li> 6 - bottom right tile (dark blue brick)  </li>
</ul>

Note that the border of the map cannot contain any empty tiles (``0``).

All tiles provided in the tile sheet are subject to copyright and belong to ID Software.
