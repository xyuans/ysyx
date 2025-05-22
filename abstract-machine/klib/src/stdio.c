#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
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
          if (num < 0) *out++ = '-';
          while (num != 0) {
            *out++ = (num % 10) + 48;
            num /= 10;
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
  return (long int)out - (long int)start;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
