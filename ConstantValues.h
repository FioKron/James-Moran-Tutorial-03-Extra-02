#pragma once

#include <string>

/** For the number of vertices present in g_vertices */
const int G_VERTICES_COUNT = 3;

/** For the Additional Exercises of Tutorial 02 */

// Constant values:

const int G_CLEAR_COLOUR_ARRAY_SIZE = 4;

/** For Exercise 01 of Tutorial 3 */

// Constant values:

const float G_DEFAULT_CLEAR_COLOUR[G_CLEAR_COLOUR_ARRAY_SIZE] = { 1.0f, 1.0f, 1.0f, 1.0f };

/** For Extra 01 of Tutorial 03 */

// Constant values:

/** For the respective Virtual-Key codes */

// Numbers (not from the numpad):

const int VK_0 = 0x30;
const int VK_1 = 0x31;
const int VK_2 = 0x32;
const int VK_3 = 0x33;
const int VK_4 = 0x34;
const int VK_5 = 0x35;
const int VK_6 = 0x36;
const int VK_7 = 0x37;
const int VK_8 = 0x38;
const int VK_9 = 0x39;

// Leters:

const int VK_AKEY = 0x41;
const int VK_BKEY = 0x42;
const int VK_CKEY = 0x43;
const int VK_DKEY = 0x44;
const int VK_EKEY = 0x45;
const int VK_FKEY = 0x46;
const int VK_GKEY = 0x47;
const int VK_HKEY = 0x48;
const int VK_IKEY = 0x49;
const int VK_JKEY = 0x4A;
const int VK_KKEY = 0x4B;
const int VK_LKEY = 0x4C;
const int VK_MKEY = 0x4D;
const int VK_NKEY = 0x4E;
const int VK_OKEY = 0x4F;
const int VK_PKEY = 0x50;
const int VK_QKEY = 0x51;
const int VK_RKEY = 0x52;
const int VK_SKEY = 0x53;
const int VK_TKEY = 0x54;
const int VK_UKEY = 0x55;
const int VK_VKEY = 0x56;
const int VK_WKEY = 0x57;
const int VK_XKEY = 0x58;
const int VK_YKEY = 0x59;
const int VK_ZKEY = 0x5A;

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