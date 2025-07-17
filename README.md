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

### ✅ COMPLETED

- [x] **Core Systems** - Types, exports, logging, math library, time system
- [x] **Platform Layer** - GLFW window management with OpenGL context
- [x] **Engine Lifecycle** - Proper initialization/shutdown with resource management
- [x] **Frame Timing** - Delta time, FPS monitoring, frame rate control

### 🎯 CURRENT FOCUS

#### **🎨 Renderer & Graphics Pipeline**

#### **Rendering Strategy**

Tunafish uses a **backend-abstracted renderer** with OpenGL as the primary implementation. The design prioritizes
simplicity and learning over bleeding-edge features.

##### **Core Philosophy:**

- **Clean abstraction** - Game code never touches OpenGL directly
- **Backend agnostic** - Vulkan support planned for future
- **Education focused** - Clear, readable code over micro-optimizations
- **Incremental complexity** - Start simple, add features as needed

##### **Rendering Pipeline:**

```
Game Code → Renderer API → OpenGL Backend → GPU
     ↓           ↓              ↓           ↓
  Meshes    Draw Commands    GL Calls    Pixels
Materials   State Changes    Buffers    Triangles
Transforms    Batching       Shaders     Colors
```

##### **Current Renderer Features:**

- [ ] **Basic geometry** - Triangles, cubes, custom meshes
- [ ] **3D camera system** - Perspective projection with view matrices
- [ ] **Shader abstraction** - Vertex/fragment shader compilation and uniforms
- [ ] **Material system** - Color properties and basic texturing support
- [ ] **State management** - Depth testing, clear operations, viewport control

##### **Planned Features:**

- [ ] **Texture loading** - PNG/JPG support with filtering options
- [ ] **Batch rendering** - Multiple objects with same material
- [ ] **Basic lighting** - Directional lights and ambient lighting
- [ ] **Debug rendering** - Wireframe, normals, bounding boxes
- [ ] **Vulkan backend** - Modern graphics API support

The renderer is designed to get pixels on screen quickly while maintaining a clean API that can evolve with more
advanced features.

---

### 📋 TODO (Priority Order)

#### **HIGH PRIORITY** (Do This Shit First)

- [x] **Memory Management** - Arena allocator for engine/renderer (malloc everywhere is for amateurs)
- [x] **Input System** - Keyboard/mouse event processing (can't make games without input, genius)
- [x] **Error Handling** - Consistent error codes and propagation (because crashes are embarrassing)
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