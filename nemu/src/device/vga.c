/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>

// 屏幕尺寸定义
#define SCREEN_W (MUXDEF(CONFIG_VGA_SIZE_800x600, 800, 400))
#define SCREEN_H (MUXDEF(CONFIG_VGA_SIZE_800x600, 600, 300))

/*---------------------------- 屏幕尺寸获取函数 ----------------------------*/
/*
 * 获取当前屏幕宽度：
 * - 在Abstract Machine(AM)平台下，从GPU硬件配置寄存器中读取实际宽度
 * - 否则返回编译时定义的SCREEN_W值, 400
 */
static uint32_t screen_width() {
  return MUXDEF(CONFIG_TARGET_AM, io_read(AM_GPU_CONFIG).width, SCREEN_W);
}
/*
 * 获取当前屏幕高度：
 * - 在AM平台下，从GPU硬件配置寄存器中读取实际高度
 * - 否则返回编译时定义的SCREEN_H值
 */
static uint32_t screen_height() {
  return MUXDEF(CONFIG_TARGET_AM, io_read(AM_GPU_CONFIG).height, SCREEN_H);
}
/*
 * 计算显存所需总大小：
 * 计算公式: 屏幕宽度 × 屏幕高度 × 每个像素的大小(uint32_t, 4字节)
 */
static uint32_t screen_size() {
  return screen_width() * screen_height() * sizeof(uint32_t);
}

static void *vmem = NULL;   // 虚拟显存区域指针，存储实际像素数据
static uint32_t *vgactl_port_base = NULL;  // VGA控制寄存器基地址指针

#ifdef CONFIG_VGA_SHOW_SCREEN
#ifndef CONFIG_TARGET_AM
#include <SDL2/SDL.h>

static SDL_Renderer *renderer = NULL;  // SDL渲染器对象
static SDL_Texture *texture = NULL;  // SDL纹理对象，用于像素渲染

/*
 * 初始化屏幕显示窗口
 * 使用SDL创建窗口、渲染器和纹理对象
 */
static void init_screen() {
  SDL_Window *window = NULL;
  char title[128];
  sprintf(title, "%s-NEMU", str(__GUEST_ISA__));
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(
      SCREEN_W * (MUXDEF(CONFIG_VGA_SIZE_400x300, 2, 1)),
      SCREEN_H * (MUXDEF(CONFIG_VGA_SIZE_400x300, 2, 1)),
      0, &window, &renderer);
  SDL_SetWindowTitle(window, title);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
  SDL_RenderPresent(renderer);
}
/*
 * 更新屏幕显示内容
 * 将显存(vmem)中的像素数据渲染到窗口
 */
static inline void update_screen() {
  SDL_UpdateTexture(texture, NULL, vmem, SCREEN_W * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}
#else
/*
 * AM平台下的空屏幕初始化函数
 */
static void init_screen() {}
/*
 * AM平台下的屏幕更新函数
 * 通过AM接口直接将显存数据提交给GPU硬件
 */
static inline void update_screen() {
 // 调用AM的GPU帧缓冲绘制接口
  // 参数: 设备类型, x位置, y位置, 数据源, 宽度, 高度, 是否立即同步
  io_write(AM_GPU_FBDRAW, 0, 0, vmem, screen_width(), screen_height(), true);
}
#endif
#endif
 
void vga_update_screen() {
  // TODO: call `update_screen()` when the sync register is non-zero,
  // then zero out the sync register
  if (vgactl_port_base[1] == 1) {
    update_screen();
    vgactl_port_base[1] = 0;
  }
}
/*---------------------------- VGA初始化函数 ----------------------------*/

/*
 * 初始化VGA显示系统
 * 设置控制寄存器、分配显存、建立内存映射
 */
void init_vga() {
  // 分配VGA控制寄存器空间(8字节)
  vgactl_port_base = (uint32_t *)new_space(8);
  vgactl_port_base[0] = (screen_width() << 16) | screen_height();
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("vgactl", CONFIG_VGA_CTL_PORT, vgactl_port_base, 8, NULL);
#else
  add_mmio_map("vgactl", CONFIG_VGA_CTL_MMIO, vgactl_port_base, 8, NULL);
#endif

  vmem = new_space(screen_size());
  add_mmio_map("vmem", CONFIG_FB_ADDR, vmem, screen_size(), NULL);
  IFDEF(CONFIG_VGA_SHOW_SCREEN, init_screen());  // 已定义CONFIG_VGA_SHOW_SCREEN
  IFDEF(CONFIG_VGA_SHOW_SCREEN, memset(vmem, 0, screen_size()));
}
