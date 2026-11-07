# Lifecycle and threading

## GLFW runtime and windows

The first live window acquires a shared GLFW runtime lease. Every window state
retains that lease; GLFW is terminated only when the last window state is
destroyed. A failed window creation releases its lease through RAII, and does
not terminate GLFW while another window is alive.

`Window` owns a private shared state containing the native window, framebuffer
size, callback, and runtime lease. The framebuffer callback refers to that
state—not the public wrapper—and exceptions from user resize callbacks are
captured inside the C callback and rethrown from `pollEvents()`.

## Context and device ownership

The context is created with the window and is owned by that window state. A
device does not destroy the context; it retains the state so the context
cannot be destroyed while the device is alive. Before each GL operation the
device makes its context current.

When a device is destroyed, it makes the context current and deletes its
program, vertex array, and buffer. Only after the device releases its shared
window state can the state detach and destroy the context/window. Thus the
normal order is device resources, context/window, then final GLFW termination.

Both `Window` and device are non-copyable and must be created, used, and
destroyed on the thread that created the first GLFW window. This follows
GLFW's main-thread requirements. Each device also retains its own OpenGL
entry-point table for its context. Cross-thread operations throw
`std::logic_error`; destroying an owner on the wrong thread terminates rather
than calling GLFW or OpenGL unsafely.

## Application shutdown

Prefer declaring the `Window` before its device. C++ then destroys the device
first. Shared private state also makes the reverse wrapper-destruction order
safe: destroying `Window` clears its callback, while the device keeps the
underlying context alive until its GL resources have been released.

The triangle sample's `--frames 1` mode exercises rendering and normal
destruction without relying on a user to close the window.
