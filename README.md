# Map generation for 2Dot

## This a tool for quickly generating maps for 2Dot.  It works by taking in a image on which the walkable path has been drawn, it will then create a .map file that automatically fills in the walls for you.

### Some assumptions
1.  The endzone is a square
2.  The first 5 pixels of your image represent the key to the image, they should all be different colors.  They correspond to the following tile types in 2dot: red (starting) tiles, blue tiles, red checkerboard tiles, green tiles, and endzone tiles.  Any other color will be interpreted as a wall.
3.  The upper left and lower right corners have the color key of the starting zone (the first pixel color of the image).  Leave the second pixel as a different color, the program will change it for you.

