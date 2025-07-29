#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  
  char *start = (char *)fmt;
  while (*fmt) {
    if (*fmt == '%') {
      fmt++;    
      switch (*fmt) {
        case 'd':{
          int num = va_arg(args, int);
          char buf[16] = {0};
          int i;
          int fuhao = 1;
          if (num < 0) {
            putch('-');
            fuhao = -1;
          }
          if (num == 0) putch ('0');
          for (i = 0; num != 0; i++) {
            buf[i] = (fuhao *(num % 10)) +48;
            num /= 10;
          }
          for(i--; i >= 0; i--) {
            putch(buf[i]);
          }
          break;
        }
        case 'c': {
          char c = va_arg(args, int);
          putch(c);
          break;
        }
        case 's': {
          char *s = va_arg(args, char*);
          while(*s != '\0') {
            putch(*s);
            s++;
          }
          break;
        }
        case 'x':
        case 'X': {
          unsigned int num = va_arg(args, unsigned);
          char buf[16] = {0};
          int i;
          if (num == 0) putch ('0');
          for (i = 0; num != 0; i++) {
            int yu = (num % 16);
            if (yu < 10) buf[i] = yu + 48;
            else { 
              buf[i] = yu + 87;
            }
            num /= 16;
          }
          for (i--; i>=0; i--) {
            putch(buf[i]);
          }
          break;
        }
        default:
          putch (*fmt);

      }
      fmt++;

    }
    else {
      putch(*fmt);
      fmt++;
    }
  }
  return (long int)fmt - (long int)start;
}
int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) { 
  va_list args;
  va_start(args, fmt);
  
  char *start = out;
  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      
      switch (*fmt) {
        case 'd':
          int num = va_arg(args, int);
          char buf[16] = {0};
          int i;

          if (num < 0) *out++ = '-';
          
          for (i = 0; num != 0; i++) {
            buf[i] = (num % 10) +48;
            num /= 10;
          }

          for(i--; i >= 0; i--) {
            *out++ = buf[i];
          }
          break;
        case 's':
          char *s = va_arg(args, char*);
          while(*s != '\0') {
            *out++ = *s++;
          }
          break;
        default:
          *out++ = *fmt;
          break;
      }

      fmt++;
    }
    else {
      *out++ = *fmt++;
    }
  }
  
  *out = '\0';
  return (int)out - (int)start;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
