#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s != '\0') {
    len++;
    s++;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  size_t i;
  
  for (i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;

  for(i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for(; i < n; i++) {
    dst[i] = '\0';
  }

  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t i;
  size_t dst_len = strlen(dst);
  

  for (i = 0; src[i] != '\0'; i++) {
    dst[dst_len + i] = src[i];
  }
  dst[dst_len + i] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  int cmp;

  while(s1[i] != '0' || s2[i] != '0') {
    cmp = s1[i] - s2[i];
    if (cmp != 0) break;
    i++;
  }

  return cmp;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
  int cmp;

  while(i < n && (s1[i] != '0' || s2[i] != '0')) {
    cmp = s1[i] - s2[i];
    if (cmp != 0) break;
    i++;
  }

  return cmp;
}

void *memset(void *s, int c, size_t n) {
  size_t i;

  // s[i] 等价于 *(s + i)，但 s 是 void * 类型，所以(char *)s[i]为非法指针针运算。 
  for (i = 0; i < n; i++) {
    *(char *)(s+i) = (char)c;
  }

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  size_t i;

  if ((src < dst) && (src + n > dst)) {
    for (i = 0; i < n; i++) {
      *(char *)(dst+n-1-i) = *(char *)(src+n-1-i);
    }
  }
  else {
    for (i = 0; i < n; i++) {
      *(char *)(dst+i) = *(char *)src+i;
    }
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;

  for (i = 0; i < n; i++) {
    *(char *)(out+i) = *(char *)(in+i);
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  int cmp;

  for (i = 0; i < n; i++) {
    cmp = *(char *)(s1+i) - *(char *)(s2+i);
    if(cmp != 0) break;
  }

  return cmp;
}

#endif
