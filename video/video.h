// Render video into a buffer

#include <array>
#include <cstdint>
#include "../util/byte-type.h"
#include "../memory/memory.h"

namespace gameboy
{
  // Type for the four colors
  typedef uint8_t color_t;

  const static int screen_column_num = 160;
  const static int screen_row_num = 144;

  // 8-pixel margin on both sides
  typedef std::array<color_t, screen_column_num + 8 * 2> row_buf_t;

  extern std::array<row_buf_t, screen_row_num> screen_buf;

  // Render the given row, if row_num < 144.
  // Does not affect external state, such as LY or STAT.
  void render_row(int row_num);

  // Handle writing to video memory, return the new value of the registers.
  // Video memory includes:
  // 8000-a000: Video ram;
  // fe00-fea0: Sprite memory;
  // ff40 (LCDC) ff41 (STAT) ff42 (SCY) ff43 (SCX) ff44 (LY)  ff45 (LYC)
  // ff46 (DMA) ff47 (BGP) ff48 (OBP0) ff49 (OBP1) ff4a (WY) ff4b (WX).
  byte_t write_video_mem(dbyte_t addr, byte_t val);

  enum {LCDC = 0xff40, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP0, OBP1, WY, WX};

  extern bool lcd_on;
};
