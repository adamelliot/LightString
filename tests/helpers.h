#pragma once

#define EXPECT_RGBA_EQ(COL, R, G, B, A) { EXPECT_EQ(COL.r, R); EXPECT_EQ(COL.g, G); EXPECT_EQ(COL.b, B); EXPECT_EQ(COL.a, A); };
#define EXPECT_RGB_EQ(COL, R, G, B) { EXPECT_EQ(COL.r, R); EXPECT_EQ(COL.g, G); EXPECT_EQ(COL.b, B); };

