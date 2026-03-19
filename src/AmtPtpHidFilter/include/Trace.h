// Trace.h: Tracing stubs (WPP disabled for manual build)

#pragma once

// Trace level constants (from evntrace.h)
#define TRACE_LEVEL_NONE        0
#define TRACE_LEVEL_CRITICAL    1
#define TRACE_LEVEL_FATAL       1
#define TRACE_LEVEL_ERROR       2
#define TRACE_LEVEL_WARNING     3
#define TRACE_LEVEL_INFORMATION 4
#define TRACE_LEVEL_VERBOSE     5

// Trace flags
#define TRACE_DRIVER  0x01
#define TRACE_DEVICE  0x02
#define TRACE_QUEUE   0x04
#define TRACE_HID     0x08
#define TRACE_INPUT   0x10

// TraceEvents stub - does nothing
#define TraceEvents(level, flags, msg, ...)

// Trace stub
#define Trace(level, msg, ...)

// WPP init/cleanup stubs
#define WPP_INIT_TRACING(DriverObject, RegistryPath)
#define WPP_CLEANUP(DriverObject)
