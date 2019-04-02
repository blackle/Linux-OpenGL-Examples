# Linux-OpenGL-Examples

Some code examples for opening windows for various purposes in very few bytes. Unfortunately each example does its own thing, so comparing the sizes is an apple to oranges thing.

## gtk-opengl - 1585 bytes

Example code for opening a glsl shader fullscreen with gtk. Closes with the standard ALT+F4 on Ubuntu. Shaders get passed an iTime uniform in seconds. App closes automatically after 10 seconds.

## gtk-webkit - 1074 bytes

Example code for opening an html page fullscreen with gtk. Closes with the standard ALT+F4 on Ubuntu. WebGL is enabled however is not used in the example. Testing with webkit gtk has found that most(?) shadertoy shaders seem to work. It has yet to be seen if initializing a shader in javascript takes fewer bytes than with gtk's opengl code.

## xlib-opengl - 1498 bytes

Example code for opening a glsl shader fullscreen with vanilla xlib. Unlike gtk-opengl, this does not pass in an iTime or render the shader more than once, and also must be closed with ESC. This code is based on the code for Cenotaph For Soda, a 4k gfx demo for Revision 2018. OPINION: xlib feels the most fragile of the bunch here...
