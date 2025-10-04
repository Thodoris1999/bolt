# bolt
A 3D graphics library

## Philosophy
In a nutshell bolt's goal is a fast, lightweight, flexible and portable graphics library.

Graphics APIs require lots of boilerplate code for a typical 3D application, while other open source graphics libraries usually target specific applications (like games or simulation), adding many abstractions which add overhead (handling of shadows, lighting). Also, advanced users have to fight the API and know the internals in order to have fine control over their pipeline behavior. This is more and more becoming the case recently as graphics pipelines are getting highly complex and everyone wants to handle things differently. It seems that everyone has to make their custom 3D engine instead of using Ogre or something (so do I :/)

bolt does a few things to address this:
1) Exposes a cross API interface that is as low level as possible, exposing things like shaders, shader attachments (uniforms, vertex arrays) and vertex attributes.
2) Offers different graphics API backends selectable at runtime (only OpenGL currently available)
3) Users are completely in charge of shaders, they can use any shading language or abstraction on top of that, as long as it is compatible with the graphics API backend.
4) For convenience, offers perhaps the only 3D graphics engine data structure that has survived all these decades, a scene graph. As the name suggests this is just a graph and nothing else, only used to update the 3D object positions in an efficient way.

As a result, you can see in the objectVis example that the code to get a minimal scene working is simpler than ogre and more lightweight. Once you have a solid collection of low level APIs, it is easy to build performant abstractions on top of it. This approach is very much imgui-like. In fact, with the OpenGL backend, integration with an imgui overlay is as simple as adding imgui calls after the scene `draw` call.

## Building
Like any cmake project
```
git clone https://github.com/Thodoris1999/bolt
git submodule update --init
cmake -B build
cmake --build build
```

## Examples
Note: `assimp` is an optional dependency, but required for the examples to build.

Example input files for the examples can be found in `examples/assets`

### objectVis
```
./build/examples/objectVis <assetFile>
```
Renders a collection of objects specified by `assetFile` to an SDL window.

### dumpFrame
```
./build/examples/dumpFrame <assetFile>
```
Renders a collection of objects specified by `assetFile` to a frame buffer and dumps it to `frame.bin`. You can visualize this image with `examples/disp_frame.py` (requires matplotlib and numpy)

## Project state
Currently in alpha. The Vulkan backend is coming next, and is expected to introduce things like asynchronousness and triple buffering to the API. Once that is taken care of, I want the project to settle on an API that will never break, at which point it enters beta.
