# Anastasia Engine Architecture & Conversation Summary

_Updated: 2025-10-21_

This document distills our entire conversation history and the final agreed-upon architecture for the Anastasia project, covering build tooling, window/input abstraction, rendering pipeline, event systems, and future plans (logging, tools, etc.).

---

## 1. CMake Toolkit & Build Options
- **`aph_option` / `aph_setup_target`**
  - `aph_option` handles boolean/string cache options with optional `STRINGS` list; we discussed extending it with validation.
  - `aph_setup_target(TARGET_SUFFIX ...)`: builds `ana-${suffix}` STATIC/SHARED based on `ANA_SHARED`, applies compiler options, sets include paths, exports alias `Anastasia::suffix`.
- **Compiler Settings (`AnaCompilerOptions.cmake`)**
  - IPO/LTO detection via `check_ipo_supported`; discussed using `$<BOOL:${ipo_supported}>` and avoiding duplicate `-flto`.
  - Sanitizer toggles (TSAN/ASAN/MSAN), RTTI/exceptions control, mold linker settings。。
  - Warnings across GCC/Clang, addition of precompiled headers, build-type-specific flags.
- **Key improvements noted**
  - Ensure `INTERPROCEDURAL_OPTIMIZATION` uses proper genex, avoid overwriting `CMAKE_*_LINKER_FLAGS`, remove redundant `-fPIC`, separate GCC/Clang-specific warnings, make `Renderer`/WSI using `POSITION_INDEPENDENT_CODE`.

## 2. Event System Evolution
- **Initial `EventManager` review**
  - Identified need for per-type locking, snapshot processing, handler return semantics (consume/broadcast), subscription lifetime and capacity controls.
  - Current implementation: per-event queue with mutex, `registerEvent` template defined inline (solve link errors), `processAll()` snapshots type list.
- **Timer/Logger**
  - Timer suggestions: use `steady_clock`, thread-safe read, transparent string_view find, sticky RAII/optional return semantics.
  - Logging: plan to introduce `ana::log` (spdlog), central init/shutdown, `std::set_terminate`, integrate backward-cpp stack traces (TODO).

## 3. Input & Camera Controls
- Explained `Camera::setProjection`, `return *this`, dirty flags; built WASD controller via event bus + state.
- Primary loop: `glfwPollEvents` → EventManager→ update state→ `eye` translation; emphasised `em.processAll()` necessity.
- Proposed modular `CameraController` for future.

## 4. Reference: Aphrodite WSI
- Reviewed `GPU/Aphrodite/src/wsi` using SDL3, event push to global EventManager, DPI management, surface creation.
- Confirmed this pattern influences current GLFW-based design.

## 5. Buffer Resource Management
- Added `ana::vk::Buffer` RAII wrapper (`src/api/vulkan/buffer.h/.cpp`) managing VkBuffer + memory, with map/unmap/write/flush/invalidate helpers.
- `BufferManager` simplifies host-visible buffers and staged device-local uploads (creates staging buffer + copy via `Device::copyBuffer`).
- Model/vertex data now uses BufferManager; destructor simplified (RAII handles cleanup).
- Removes repetitive `vkCreateBuffer`/`vkAllocateMemory` boilerplate, paving the way for future resource managers (images, descriptors, etc.).

## 6. WSI Refactor (Final Architecture)
- **Goal**: decouple window/input (WSI) from Vulkan renderer; App owns WSI.
- **Interface (`src/wsi/wsi.h`)**
  - Methods: `poll()`, `getRequiredInstanceExtensions()`, `createSurface(VkInstance)`, `framebufferExtent()`, `nativeHandle()`.
  - Sink setters for keyboard/mouse/resize events.
- **GLFW Implementation (`src/wsi/wsi.cpp`)**
  - Creates/destroys window, caches framebuffer extent/cursor, handles callbacks, maps to engine `Key/KeyState` via `keymap.h`.
  - `CreateGLFWWSI(width, height, title)` returns `std::unique_ptr<IWSI>`.
- **Build system**
  - `src/wsi/CMakeLists.txt` registers `ana-wsi` target, links `glfw`, `ana-common`, `ana-event`.
- **Device/Renderer**
  - `vk::Device(IWSI&)` uses WSI for instance extensions, surface creation; cleans up surface on destruction.
  - `Renderer(IWSI&, Device&)` uses `framebufferExtent()` for swapchain, `notifyWindowResized()` for rebuild, no direct GLFW usage.
- **App Layer (`APP::run`)**
  - Creation order: `wsi -> device -> renderer -> renderSystem -> loadGameObjects`。
  - Event sinks: keyboard -> EventManager (maintain WASD state); resize -> `renderer->notifyWindowResized()`.
  - Loop: `wsi->poll()` → `em.processAll()` → update camera `eye` using `frameTime` → `renderer->beginFrame()` … `renderSystem->renderGameObjects()`.
- **ANAwindow**
  - Reduced to thin wrapper owning `unique_ptr<IWSI>`; methods inline forward to WSI; `.cpp` removed.

## 7. Remaining Warnings / TODOs
- Warnings: unused `Event::m_type`, shadowed `properties` in `Device`, `swapChain` naming, etc.
- Add ImGui capture checks in WSI callbacks.
- Extend `IWSI` with additional window controls (title, fullscreen, cursor, etc.) for future.
- Implement `ana::log`, integrate try/catch + terminate handler.
- Potential Headless/SDL/Win32 WSI backends; multi-window management.

## 8. Logging / Exception Capture Plan (pending)
- `Log` module (spdlog) with core & client logger, console + file sinks.
- `main()` wrap `APP` in try/catch, plus `std::set_terminate` to log unhandled exceptions.
- Optionally integrate backward-cpp `SignalHandling` for crash stack traces.

## 9. Latest changes (Commit HEAD)
- `wsi` module added; Device/Renderer signatures updated to use WSI.
- App stores WSI/Device/Renderer as `unique_ptr`, loads game objects post-initialization.
- CMake updates to build `ana-wsi` and link where required.
- `doc/conversation_log.md` and this document created to track decisions & TODOs.

---

## Appendix: Implementation Files
- WSI interface: `src/wsi/wsi.h`
- GLFW WSI: `src/wsi/wsi.cpp`
- Device: `src/api/vulkan/device.h/.cpp`
- Renderer: `src/api/vulkan/renderer.h/.cpp`
- App entry: `src/app.h/.cpp`
- Conversation logs: `doc/conversation_log.md`, `doc/conversation_summary.md`

---
**Next recommended actions**
1. Add logging module and exception handlers.
2. Resolve compile warnings (rename shadowed vars, use `[[maybe_unused]]` or remove unused fields).
3. Expand WSI interface (cursor, fullscreen etc.) and consider SDL/headless implementations.
4. Modularize systems (CameraController, InputSystem) for future ECS/game loop.

> This documentation captures all work to date and the agreed-upon architecture for moving forward.
