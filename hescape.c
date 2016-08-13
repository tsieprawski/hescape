#include <stdio.h>
#include <string.h>
#include "hescape.h"

#ifdef __SSE4_2__
# ifdef _MSC_VER
#  include <nmmintrin.h>
# else
#  include <x86intrin.h>
# endif
#endif

#if __GNUC__ >= 3
# define likely(x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#else
# define likely(x) (x)
# define unlikely(x) (x)
#endif

static const uint8_t *ESCAPED_STRING[] = {
  "",
  "&quot;",
  "&amp;",
  "&#39;",
  "&lt;",
  "&gt;",
};

// This is strlen(ESCAPED_STRING[x]) optimized specially.
// Mapping: 1 => 6, 2 => 5, 3 => 5, 4 => 4, 5 => 4
#define ESC_LEN(x) ((13 - x) / 2)

/*
 * Given ASCII-compatible character, return index of ESCAPED_STRING.
 *
 * " (34) => 1 (&quot;)
 * & (38) => 2 (&amp;)
 * ' (39) => 3 (&#39;)
 * < (60) => 4 (&lt;)
 * > (62) => 5 (&gt;)
 */
static const char HTML_ESCAPE_TABLE[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 5, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

uint8_t*
ensure_allocated(uint8_t *buf, size_t size, size_t *asize)
{
  if (size < *asize)
    return buf;

  size_t new_size;
  if (*asize == 0) {
    new_size = size;
  } else {
    new_size = *asize;
  }

  // Increase buffer size by 1.5x if realloced multiple times.
  while (new_size < size)
    new_size = (new_size << 1) - (new_size >> 1);

  // Round allocation up to multiple of 8.
  new_size = (new_size + 7) & ~7;

  *asize = new_size;
  return realloc(buf, new_size);
}

size_t
hesc_escape_html(uint8_t **dest, const uint8_t *buf, size_t size)
{
  size_t asize = 0, esc_i, esize = 0, rbuf_end = 0;
  const uint8_t *esc;
  uint8_t *rbuf = NULL;

  for (size_t i = 0; i < size; i++) {
    if ((esc_i = HTML_ESCAPE_TABLE[buf[i]])) {
      esc = ESCAPED_STRING[esc_i];
      rbuf = ensure_allocated(rbuf, sizeof(uint8_t) * (size + esize + ESC_LEN(esc_i) + 1), &asize);

      memmove(rbuf + rbuf_end, buf + (rbuf_end - esize), i - (rbuf_end - esize));
      memmove(rbuf + i + esize, esc, ESC_LEN(esc_i));
      rbuf_end = i + esize + ESC_LEN(esc_i);
      esize += ESC_LEN(esc_i) - 1;
    }
  }

  if (rbuf_end == 0) {
    *dest = (uint8_t *)buf;
    return size;
  } else {
    // Copy pending characters including NULL character.
    memmove(rbuf + rbuf_end, buf + (rbuf_end - esize), (size + 1) - (rbuf_end - esize));

    *dest = rbuf;
    return size + esize;
  }
}
