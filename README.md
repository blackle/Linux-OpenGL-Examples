# Linux-OpenGL-Examples

Here are examples of rendering a shader full screen on linux. All of these examples do it in less than 2 kilobytes thanks to vondehi and copious amounts of ELF stripping.

There are three examples, each using a different library to open a window and get an opengl context.

## xlib-opengl - 1503 bytes

Vanilla xlib is the largest of the bunch and the most brittle. It is highly not recommended to use xlib in a demo unless you cannot assume these other libraries will be on the system.

## gtk-opengl - 1396 bytes

GTK is a step up from xlib, being both smaller and more robust. If the compo you are entering doesn't allow using SDL2, GTK is an ok alternative. Before switching to GCC 8.3.0, this was 100 bytes smaller than it is now. I am not sure why it is larger, but this likely means it can be sizecoded/stripped further.

## sdl2-opengl - 1010 bytes

Using SDL2 will give you very small binaries. Use SDL2 whenever you can, as it also has a few other useful subsystems at minimal cost (for example, audio.)

