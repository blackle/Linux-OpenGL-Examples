# Linux-OpenGL-Examples

Some code examples for opening windows for various purposes in very few bytes.

## gtk-opengl - 1585 bytes

Example code for opening a glsl shader fullscreen with gtk. Closes with the standard alt+f4 on Ubuntu. Shaders get passed an iTime uniform in seconds. App closes automatically after 10 seconds.

## gtk-webkit - 1074 bytes

Example code for opening an html page fullscreen with gtk. Closes with the standard alt+f4 on Ubuntu. WebGL is enabled however is not used in the example. Testing with webkit gtk has found that most(?) shadertoy shaders seem to work. It has yet to be seen if initializing a shader in javascript takes fewer bytes than with gtk's opengl code.
