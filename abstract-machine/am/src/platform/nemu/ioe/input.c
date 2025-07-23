#include <am.h>
#include <nemu.h>

/*
| 31..16位 | 15位   | 14..0位 |
|  保留    | 状态位 | 按键码  |
状态码1表示按下，0表示释放。
*/
#define KEYDOWN_MASK 0x8000
void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t k = inl(KBD_ADDR);// & KEYDOWN_MASK;
  kbd->keydown = k & KEYDOWN_MASK ? true: false; 
  kbd->keycode = k & ~KEYDOWN_MASK;
}
