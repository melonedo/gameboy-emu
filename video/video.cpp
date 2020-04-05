
#include <array>
#include <cstring>
#include <vector>
#include <algorithm>
#include "video.h"

namespace gameboy
{
  std::array<row_buf_t, screen_row_num> screen_buf;

  typedef std::array<std::array<color_t, 8>, 8> tile_t;
  std::array<tile_t, 384> tile_set;

  struct sprite_t {
    byte_t x, y, tile_num;
    bool hidden, y_flip, x_flip, palette;
  };
  std::array<sprite_t, 40> sprite_set;

  // ff40: LCDC
  bool bg_on;
  bool sprite_on;
  bool use_large_sprite;
  dbyte_t bg_map_addr;
  bool unsigned_tile_num; // Affects both bg and win
  bool win_on;
  dbyte_t win_map_addr;
  bool lcdc_on;

  typedef std::array<byte_t, 4> palette_t;
  // ff47: BGP
  palette_t bgp;
  // ff48 and ff49: OBP0 and OBP1
  palette_t obp[2];

  void preprocess_tile(dbyte_t tile_num, byte_t row_num, bool is_high_byte,
    byte_t val);

  void preprocess_palette(palette_t &plt , byte_t val);

  void dma_transfer(byte_t val);

  byte_t write_video_mem(dbyte_t addr, byte_t val)
  {
    if (addr >= 0x8000 && addr < 0xa000)
    {
      if (addr < 0x9800)
      {
        // The tile set is preprocessed
        addr -= 0x8000;
        // 0b a aaaabbbc, a for tile number, b for row number, c for is_high_byte
        preprocess_tile(addr / 16, (addr % 16) / 2, addr % 2, val);
      }
    }
    else if (addr >= 0xfe00 && addr < 0xfea0)
    {
      addr -= 0xfe00;
      size_t sprite_num = addr / 4;
      sprite_t &spr = sprite_set[sprite_num];
      switch (addr % 4)
      {
        case 0:
        spr.y = val;
        break;

        case 1:
        spr.x = val;
        break;

        case 2:
        spr.tile_num = val;
        break;

        case 3:
        spr.hidden = val & 0x80;
        spr.y_flip = val & 0x40;
        spr.x_flip = val & 0x20;
        spr.palette = val & 0x10;
      }
    }
    else if (addr >= 0xff40 && addr <= 0xff4b)
    {
      switch (addr)
      {
        case LY:
        // Read only
        return memory.at(0xff44);

        case DMA:
        // DMA trnasfer
        dma_transfer(val);
        // Unreadable
        return 0xff;

        case BGP:
        preprocess_palette(bgp, val);
        break;

        case OBP0:
        preprocess_palette(obp[0], val);
        break;

        case OBP1:
        preprocess_palette(obp[1], val);
        break;

        default:
        break;
      }
    }
    else
    {
      // This should not happen
    }
    return val;
  }

  void dma_transfer(byte_t val)
  {
    dbyte_t addr = 0x100 * val;
    byte_t *dst = &memory.at(0xfe00);
    byte_t *src = &memory.at(addr);
    memcpy(dst, src, 0xa0 * sizeof(byte_t));
  }

  void preprocess_tile(dbyte_t tile_num, byte_t row_num, bool is_high_byte,
    byte_t val)
  {
    tile_t &tile = tile_set[tile_num];
    std::array<byte_t, 8> &row = tile[row_num];

    // Write to write_bit, while remain the other bit as is
    byte_t write_bit = is_high_byte ? 2 : 1;
    byte_t mask = 1;
    for (int i = 0; i < 8; i++, mask <<= 1)
    {
      if (mask & val)
      {
        row[i] |= write_bit;
      }
      else
      {
        row[i] &= ~write_bit;
      }
    }
  }

  void preprocess_palette(palette_t &plt, byte_t val)
  {
    byte_t mask = 0b11;
    for (int i = 0; i < 4; i++, val >>= 2)
    {
      plt[i] = val & mask;
    }
  }

  // Copy 8 bytes
  void copy_one_row(const tile_t &tile, uint8_t row_num,
    const palette_t &plt, color_t *dst);

  const tile_t &get_bg_tile(byte_t code);

  void render_row(int row_num)
  {
    std::array<color_t, screen_column_num + 16> &buf = screen_buf.at(row_num);

    // First prepare the sprites
    std::vector<byte_t> spr_top, spr_bottom;
    for (int i = 0; i < 40; i++)
    {
      const sprite_t &spr = sprite_set[i];
      int diff = row_num - (spr.y - 16);
      if (diff >= 0 && diff < 8)
      {
        if (spr.hidden)
        {
          spr_bottom.push_back(i);
        }
        else
        {
          spr_top.push_back(i);
        }
      }
    }
    // Sort so the priority decreases
    auto cmp = [](int lhs, int rhs)
    { return sprite_set[lhs].x < sprite_set[rhs].x; };
    std::stable_sort(spr_bottom.begin(), spr_bottom.end(), cmp);
    std::stable_sort(spr_top.begin(), spr_top.end(), cmp);

    // Sprites in the bottom
    if (sprite_on)
    {
      for (auto i = spr_bottom.crbegin(); i != spr_bottom.crend(); i++)
      {
        const sprite_t &spr = sprite_set[*i];
        copy_one_row(tile_set[spr.tile_num], row_num - (spr.y - 16),
          obp[spr.palette], buf.begin() + 8 + (spr.x - 8));
      }
    }

    // Background
    if (bg_on)
    {
      // Position of screen relative to background
      byte_t left = memory.at(SCX);
      byte_t up = memory.at(SCY);
      dbyte_t relative_row = (up + row_num) % 256;
      dbyte_t map_base = bg_map_addr + 32 * relative_row;
      int map_index_begin = left / 8;
      auto copy_dst = buf.begin() + 8 - left % 8;
      const int tile_num = screen_column_num / 8 + 1;

      for (int i = 0; i < tile_num; i++)
      {
        copy_one_row(
          get_bg_tile(memory.at(map_base + (i + map_index_begin) % 32)),
          relative_row % 8, bgp, copy_dst + 8 * i);
      }
    }

    // Window
    if (bg_on && win_on)
    {
      // Absolute position (relative to the screen)
      byte_t up = memory.at(WY);
      if (up <= row_num)
      {
        dbyte_t relative_row = row_num - up;
        byte_t left = memory.at(WX) - 7;
        dbyte_t map_base = win_map_addr + 32 * row_num;
        int tile_num = (160 - left) / 8 + 1;
        auto copy_dst = buf.begin() + 8 + left;
        for (int i = 0; i < tile_num; i++)
        {
          copy_one_row(get_bg_tile(memory.at(map_base + i)),
            relative_row % 8, bgp, copy_dst + 8 * i);
        }
      }
    }

    // Sprites on top
    if (sprite_on)
    {
      for (auto i = spr_top.crbegin(); i != spr_top.crend(); i++)
      {
        const sprite_t &spr = sprite_set[*i];
        copy_one_row(tile_set[spr.tile_num], row_num - (spr.y - 16),
          obp[spr.palette], buf.begin() + 8 + (spr.x - 8));
      }
    }
  }

  const tile_t &get_bg_tile(byte_t code)
  {
    if (unsigned_tile_num)
    {
      return tile_set[code];
    }
    else
    {
      return tile_set[add_signed(dbyte_t(256), code)];
    }
  }

  void copy_one_row(const tile_t &tile, uint8_t row_num,
    const palette_t &plt, color_t *dst)
  {
    const std::array<color_t, 8> &row = tile[row_num];
    for (int i = 0; i < 8; i++)
    {
      dst[i] = plt[row[i]];
    }
  }
};
