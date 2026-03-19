// Device.h: Device-specific struct and routines

#pragma once

EXTERN_C_START

// {FF969022-3111-4441-8F88-875440172C2E} for the device interface
DEFINE_GUID(GUID_DEVICEINTERFACE_AmtPtpHidFilter,
    0xff969022, 0x3111, 0x4441, 0x8f, 0x88, 0x87, 0x54, 0x40, 0x17, 0x2c, 0x2e);

// Definition
#define HID_XFER_PACKET_SIZE 32

#define HID_VID_APPLE_USB 0x05ac
#define HID_VID_APPLE_BT  0x004c
#define HID_PID_MAGIC_TRACKPAD_2 0x0265
#define HID_PID_MAGIC_MOUSE_2    0x0269

// Magic Mouse 2 specific constants
#define MM2_BT_HEADER_SIZE    14   // Bytes before finger data in BT report
#define MM2_FINGER_SIZE       8    // Bytes per finger record
#define MM2_BUTTON_OFFSET     1    // Button byte offset in report
#define MM2_REPORT_ID         0x12 // Report ID for Magic Mouse 2

// Magic Mouse 2 coordinate ranges
#define MM2_X_MIN   -1100
#define MM2_X_MAX    1258
#define MM2_Y_MIN   -1589
#define MM2_Y_MAX    2047

// Magic Mouse 2 touch states
#define MM2_TOUCH_STATE_NONE  0x00
#define MM2_TOUCH_STATE_START 0x30
#define MM2_TOUCH_STATE_DRAG  0x40

// Magic Mouse 2 mouse delta offsets in BT report header
#define MM2_MOUSE_DX_OFFSET   2   // bytes 2-3: relative X (16-bit signed LE)
#define MM2_MOUSE_DY_OFFSET   4   // bytes 4-5: relative Y (16-bit signed LE)

/* device-specific parameters */
typedef struct _BCM5974_PARAM {
    int snratio;		/* signal-to-noise ratio */
    int min;			/* device minimum reading */
    int max;			/* device maximum reading */
} BCM5974_PARAM, *PBCM5974_PARAM;

// Device Context
typedef struct _DEVICE_CONTEXT
{
    PDEVICE_OBJECT  WdmDeviceObject;
    WDFDEVICE       Device;
    WDFQUEUE        HidReadQueue;

    // Device identification
    USHORT VendorID;
    USHORT ProductID;
    USHORT VersionNumber;
    size_t InputHeaderSize;
    size_t InputFingerSize;
    size_t InputFingerDelta;
    size_t InputButtonDelta;
    BCM5974_PARAM X;
    BCM5974_PARAM Y;

    // List of buffers
    WDFLOOKASIDE HidReadBufferLookaside;

    // System HID transport
    WDFIOTARGET HidIoTarget;
    BOOLEAN     IsHidIoDetourCompleted;
    WDFTIMER    HidTransportRecoveryTimer;
    WDFWORKITEM HidTransportRecoveryWorkItem;

    // Device State
    BOOLEAN DeviceConfigured;
    BOOLEAN IsMagicMouse2;

    // PTP report specific
    LARGE_INTEGER   LastReportTime;
    BOOLEAN         PtpInputOn;
    BOOLEAN         PtpReportTouch;
    BOOLEAN         PtpReportButton;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, PtpFilterGetContext)

// Request context
typedef struct _WORKER_REQUEST_CONTEXT {
    PDEVICE_CONTEXT DeviceContext;
    WDFMEMORY RequestMemory;
} WORKER_REQUEST_CONTEXT, * PWORKER_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKER_REQUEST_CONTEXT, WorkerRequestGetContext)

// Initialization routines
NTSTATUS
PtpFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

// Power Management Handlers
EVT_WDF_DEVICE_PREPARE_HARDWARE PtpFilterPrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY PtpFilterDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT PtpFilterDeviceD0Exit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT PtpFilterSelfManagedIoInit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART PtpFilterSelfManagedIoRestart;

// Device Management routines
NTSTATUS
PtpFilterConfigureMultiTouch(
    _In_ WDFDEVICE Device
);

VOID
PtpFilterRecoveryTimerCallback(
    WDFTIMER Timer
);

EXTERN_C_END
