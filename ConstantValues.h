#pragma once

#include <string>

/** For the number of vertices present in g_vertices (6 for two triangles) */
const UINT G_VERTICES_COUNT = 8;

/** For the Additional Exercises of Tutorial 02 */

// Constant values:

const UINT G_CLEAR_COLOUR_ARRAY_SIZE = 4;

/** For Exercise 01 of Tutorial 3 */

// Constant values:

const float G_DEFAULT_CLEAR_COLOUR[G_CLEAR_COLOUR_ARRAY_SIZE] = { 1.0f, 1.0f, 1.0f, 1.0f };

/** For Extra 01 of Tutorial 03 */

// Constant values:

/** For the respective Virtual-Key codes */

// Numbers (not from the numpad):

const UINT VK_0 = 0x30;
const UINT VK_1 = 0x31;
const UINT VK_2 = 0x32;
const UINT VK_3 = 0x33;
const UINT VK_4 = 0x34;
const UINT VK_5 = 0x35;
const UINT VK_6 = 0x36;
const UINT VK_7 = 0x37;
const UINT VK_8 = 0x38;
const UINT VK_9 = 0x39;

// Leters:

const UINT VK_AKEY = 0x41;
const UINT VK_BKEY = 0x42;
const UINT VK_CKEY = 0x43;
const UINT VK_DKEY = 0x44;
const UINT VK_EKEY = 0x45;
const UINT VK_FKEY = 0x46;
const UINT VK_GKEY = 0x47;
const UINT VK_HKEY = 0x48;
const UINT VK_IKEY = 0x49;
const UINT VK_JKEY = 0x4A;
const UINT VK_KKEY = 0x4B;
const UINT VK_LKEY = 0x4C;
const UINT VK_MKEY = 0x4D;
const UINT VK_NKEY = 0x4E;
const UINT VK_OKEY = 0x4F;
const UINT VK_PKEY = 0x50;
const UINT VK_QKEY = 0x51;
const UINT VK_RKEY = 0x52;
const UINT VK_SKEY = 0x53;
const UINT VK_TKEY = 0x54;
const UINT VK_UKEY = 0x55;
const UINT VK_VKEY = 0x56;
const UINT VK_WKEY = 0x57;
const UINT VK_XKEY = 0x58;
const UINT VK_YKEY = 0x59;
const UINT VK_ZKEY = 0x5A;

/** For modification of the Position and Colour of vertices */
const float POSITION_MODIFICATION_SCALAR = 0.0010f;
const float COLOUR_MODIFICATION_SCALAR = 0.010f;

/**
The bounds that position and colour values
of vertices can tend to (either way)
*/
const float POSITION_LOWER_BOUND = -1.0f;
const float POSITION_UPPER_BOUND = 1.0f;
const float COLOUR_LOWER_BOUND = 0.0f;
/**
As on screen positions are valid between -1.0 and 1.0.
Whilst colour values are valid between 0.0 and 1.0.
*/
const float COLOUR_UPPER_BOUND = POSITION_UPPER_BOUND;

// Rename for each tutorial
//char		g_TutorialName[100] = "James Moran Tutorial 02 Exercise 01\0";

// (Attempting to use std::string instead):
const std::string TUTORIAL_NAME = "James Moran Tutorial 03 Extra 01\0";

// Give your app window your own name
//char Name[100] = "James Moran\0";

// (Changed to std::string once again, also made const.)
const std::string NAME = "James Moran\0";