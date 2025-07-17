# 🐟 Tunafish

A simple and minimal 3D game engine in C.

## Motivation

This is a learning project, not meant to be a production game engine.

## Why C? Why not C++/Rust?

C is supreme, C is great. I wanted to learn a proper programming language, not a convoluted superset with footguns in
its syntax. Rust is
great, but too verbose and too much compiler magic. C is simple.

## Supported Platforms

1. MacOS (What I use)
2. Linux (What I want to use, but I have an actual life)
3. Windows (Absolute shit, probably builds on it, don't know, don't care)

## Build Instructions

You can't see the cmake files? Use cmake. Figure it out. Use LLMs, don't be a little child and learn how to read and
build a
codebase like a big boy.

## Roadmap

### ✅ COMPLETED (Solid Foundation)

- [x] **Core Systems** - Types, exports, logging, math library, time system
- [x] **Platform Layer** - GLFW window management with OpenGL context
- [x] **Engine Lifecycle** - Proper initialization/shutdown with resource management
- [x] **Frame Timing** - Delta time, FPS monitoring, frame rate control

### 🎯 CURRENT FOCUS

Working on the renderer abstraction because everything else is for weaklings who can't prioritize.

### 📋 TODO (Priority Order)

#### **HIGH PRIORITY** (Do This Shit First)

- [ ] **Memory Management** - Arena allocator for engine/renderer (malloc everywhere is for amateurs)
- [ ] **Input System** - Keyboard/mouse event processing (can't make games without input, genius)
- [ ] **Error Handling** - Consistent error codes and propagation (because crashes are embarrassing)
- [ ] **Renderer Abstraction** - OpenGL backend with clean API (the whole point of this engine)
- [ ] **Basic Rendering** - Clear screen, draw triangles, basic shaders

#### **MODERATE PRIORITY** (After You Get the Basics Right)

- [ ] **Logging Optimization** - Lazy evaluation for debug logs (performance matters)
- [ ] **Asset Loading** - Textures, models, shaders from files
- [ ] **Configuration System** - Runtime settings without recompiling like a caveman
- [ ] **Scene Management** - Basic 3D scene graph for organizing objects
- [ ] **Camera System** - FPS camera, orbit camera, projection matrices

#### **LOW PRIORITY** (For When You're Bored)

- [ ] **Math Optimizations** - SIMD, inline hot paths (micro-optimizations for perfectionists)
- [ ] **Advanced Rendering** - PBR, shadows, post-processing
- [ ] **Physics Integration** - Basic collision detection
- [ ] **Audio System** - 3D positional audio
- [ ] **Thread Safety** - When single-threading becomes your bottleneck
- [ ] **Multiple Backends** - Vulkan, Metal, DirectX (because OpenGL isn't cool enough)

#### **FUTURE CONSIDERATIONS** (Pipe Dreams)

- [ ] **Editor/Tools** - Visual scene editor, asset pipeline
- [ ] **Scripting** - Lua or custom scripting language
- [ ] **Networking** - Multiplayer support
- [ ] **Mobile Support** - iOS/Android (if you hate yourself)

## Next steps

Use this as a base, improve upon it. Don't create issues and pull requests, I don't care about you or your opinions.
Just fork and build. I am an entrepreneur, not a software engineer. I don't have time to waste on your ideal project
state of something I have written.