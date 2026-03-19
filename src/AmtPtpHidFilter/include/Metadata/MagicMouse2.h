// MagicMouse2.h: Magic Mouse 2 specific HID PTP descriptor
// Adapted from MagicTrackpad2.h for Magic Mouse 2 touch surface
// Surface: 90.56mm x 51.52mm
// X range: -1100 to 1258 (logical 0-2358)
// Y range: -1589 to 2047 (logical 0-3636)
#pragma once

// Magic Mouse 2 has a smaller touch surface than the trackpad
// Physical max X: 906 (= 90.56mm in 0.01cm units)
// Physical max Y: 515 (= 51.52mm in 0.01cm units)
// Logical max X: 2358 (= 0x0936)
// Logical max Y: 3636 (= 0x0E34)

#define AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION \
	BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
		/* Begin a byte */ \
		LOGICAL_MAXIMUM, 0x01, /* Logical Maximum: 1 */ \
		USAGE, 0x47, /* Usage: Confidence */ \
		USAGE, 0x42, /* Usage: Tip switch */ \
		REPORT_COUNT, 0x02, /* Report Count: 2 */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		REPORT_COUNT, 0x06, /* Report Count: 6 */ \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		/* End of a byte */ \
		/* Begin of 4 bytes: Contact ID */ \
		REPORT_COUNT, 0x01, /* Report Count: 1 */ \
		REPORT_SIZE, 0x20, /* Report Size: 0x20 (4 bytes) */ \
		LOGICAL_MAXIMUM_3, 0xff, 0xff, 0xff, 0xff, /* Logical Maximum: 0xffffffff */ \
		USAGE, 0x51, /* Usage: Contact Identifier */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		/* End of 4 bytes */ \
		/* Begin of 4 bytes: X and Y */ \
		USAGE_PAGE, 0x01, /* Usage Page: Generic Desktop */ \
		LOGICAL_MAXIMUM_2, 0x36, 0x09, /* Logical Maximum: 2358 */ \
		REPORT_SIZE, 0x10, /* Report Size: 0x10 (2 bytes) */ \
		UNIT_EXPONENT, 0x0e, /* Unit exponent: -2 */ \
		UNIT, 0x11, /* Unit: SI Length (cm) */ \
		USAGE, 0x30, /* Usage: X */ \
		PHYSICAL_MAXIMUM_2, 0x8a, 0x03, /* Physical Maximum: 906 (90.56mm) */ \
		REPORT_COUNT, 0x01, /* Report count: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM_2, 0x03, 0x02, /* Physical Maximum: 515 (51.52mm) */ \
		LOGICAL_MAXIMUM_2, 0x34, 0x0e, /* Logical Maximum: 3636 */ \
		USAGE, 0x31, /* Usage: Y */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM, 0x00, /* Physical Maximum: 0 */ \
		UNIT_EXPONENT, 0x00, /* Unit exponent: 0 */ \
		UNIT, 0x00, /* Unit: None */ \
		/* End of 4 bytes */ \
	END_COLLECTION /* End Collection */

// Mouse TLC for optical sensor cursor movement (dX/dY from report header)
#define AAPL_MAGIC_MOUSE2_MOUSE_TLC \
	USAGE_PAGE, 0x01,               /* Usage Page: Generic Desktop */ \
	USAGE, 0x02,                    /* Usage: Mouse */ \
	BEGIN_COLLECTION, 0x01,         /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_STANDARDMOUSE, /* Report ID: 0x02 */ \
		USAGE, 0x01,                /* Usage: Pointer */ \
		BEGIN_COLLECTION, 0x00,     /* Begin Collection: Physical */ \
			USAGE_PAGE, 0x09,       /* Usage Page: Button */ \
			USAGE_MINIMUM, 0x01,    /* Usage Minimum: Button 1 */ \
			USAGE_MAXIMUM, 0x02,    /* Usage Maximum: Button 2 */ \
			LOGICAL_MINIMUM, 0x00,  /* Logical Minimum: 0 */ \
			LOGICAL_MAXIMUM, 0x01,  /* Logical Maximum: 1 */ \
			REPORT_COUNT, 0x02,     /* Report Count: 2 */ \
			REPORT_SIZE, 0x01,      /* Report Size: 1 */ \
			INPUT, 0x02,            /* Input: (Data, Var, Abs) */ \
			REPORT_COUNT, 0x06,     /* Report Count: 6 (padding) */ \
			REPORT_SIZE, 0x01,      /* Report Size: 1 */ \
			INPUT, 0x03,            /* Input: (Const, Var, Abs) */ \
			USAGE_PAGE, 0x01,       /* Usage Page: Generic Desktop */ \
			USAGE, 0x30,            /* Usage: X */ \
			USAGE, 0x31,            /* Usage: Y */ \
			LOGICAL_MINIMUM_2, 0x01, 0x80,  /* Logical Minimum: -32767 */ \
			LOGICAL_MAXIMUM_2, 0xFF, 0x7F,  /* Logical Maximum: 32767 */ \
			REPORT_SIZE, 0x10,      /* Report Size: 16 */ \
			REPORT_COUNT, 0x02,     /* Report Count: 2 */ \
			INPUT, 0x06,            /* Input: (Data, Var, Rel) */ \
		END_COLLECTION,             /* End Collection: Physical */ \
	END_COLLECTION                  /* End Collection: Application */

#define AAPL_MAGIC_MOUSE2_PTP_TLC \
	USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
	USAGE, 0x05, /* Usage: Touch Pad */ \
	BEGIN_COLLECTION, 0x01, /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_MULTITOUCH, /* Report ID: Multi-touch */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION, /* 1 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION, /* 2 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION, /* 3 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION, /* 4 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_MOUSE2_PTP_FINGER_COLLECTION, /* 5 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		UNIT_EXPONENT, 0x0c, /* Unit exponent: -4 */ \
		UNIT_2, 0x01, 0x10, /* Time: Second */ \
		PHYSICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		LOGICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		USAGE, 0x56, /* Usage: Scan Time */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE, 0x54, /* Usage: Contact Count */ \
		LOGICAL_MAXIMUM, 0x7f, \
		REPORT_SIZE, 0x08, \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE_PAGE, 0x09, /* Usage Page: Button */ \
		USAGE, 0x01, /* Button 1 */ \
		LOGICAL_MAXIMUM, 0x01, \
		REPORT_SIZE, 0x01, \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x07, \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		REPORT_ID, REPORTID_DEVICE_CAPS, \
		USAGE, 0x55, /* Usage: Maximum Contacts */ \
		USAGE, 0x59, /* Usage: Touchpad Button Type */ \
		LOGICAL_MINIMUM, 0x00, \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT, 0x02, \
		FEATURE, 0x02, \
		USAGE_PAGE_1, 0x00, 0xff, \
		REPORT_ID, REPORTID_PTPHQA, \
		USAGE, 0xc5, \
		LOGICAL_MINIMUM, 0x00, \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT_2, 0x00, 0x01, \
		FEATURE, 0x02, \
	END_COLLECTION /* End Collection */
