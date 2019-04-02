# Linux-OpenGL-Examples

Some code examples for opening windows for various purposes in very few bytes. The examples mostly do the same thing, the only difference being gtk-webkit uses a different shader source, and is also heavily unoptimized.

## gtk-opengl - 1292 bytes

Example code for opening a glsl shader fullscreen with gtk. Closes with the standard ALT+F4 on Ubuntu. Renders the shader once on load.

## gtk-webkit - 1482 bytes

Example code for opening a webgl enabled html page fullscreen with gtk and webkit. Closes with the standard ALT+F4 on Ubuntu. WebGL is used to display a shader full screen with some very unoptimized javascript. Like gtk-opengl, the shader is rendered once. Optimizing the js for size will probably take off another 100 bytes or two.

## xlib-opengl - 1474 bytes

Example code for opening a glsl shader fullscreen with vanilla xlib. Like gtk-opengl and gtk-webkit, the shader is rendered once. Unlike gtk-opengl, the window must be closed with ESC. This code is based on the code for Cenotaph For Soda, a 4k gfx demo for Revision 2018. OPINION: xlib feels the most fragile of the bunch here...
