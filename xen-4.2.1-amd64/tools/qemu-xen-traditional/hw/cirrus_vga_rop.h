/*
 * QEMU Cirrus CLGD 54xx VGA Emulator.
 *
 * Copyright (c) 2004 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define get_base(p, s, b) do { \
    if ((p) >= (s)->vram_ptr && (p) < (s)->vram_ptr + (s)->vram_size) \
	(b) = (s)->vram_ptr; \
    else if ((p) >= &(s)->cirrus_bltbuf[0] && \
	     (p) < &(s)->cirrus_bltbuf[CIRRUS_BLTBUFSIZE]) \
	(b) = &(s)->cirrus_bltbuf[0]; \
    else \
	return; \
} while(0)

#define m(x) ((x) & s->cirrus_addr_mask)

static void
glue(cirrus_bitblt_rop_fwd_, ROP_NAME)(CirrusVGAState *s,
                             uint8_t *dst_,const uint8_t *src_,
                             int dstpitch,int srcpitch,
                             int bltwidth,int bltheight)
{
    int x,y;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch -= bltwidth;
    srcpitch -= bltwidth;

    if (dstpitch < 0 || srcpitch < 0) {
        /* is 0 valid? srcpitch == 0 could be useful */
        return;
    }

    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x++) {
            ROP_OP(*(dst_base + m(dst)), *(src_base + m(src)));
            dst++;
            src++;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

static void
glue(cirrus_bitblt_rop_bkwd_, ROP_NAME)(CirrusVGAState *s,
                                        uint8_t *dst_,const uint8_t *src_,
                                        int dstpitch,int srcpitch,
                                        int bltwidth,int bltheight)
{
    int x,y;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch += bltwidth;
    srcpitch += bltwidth;
    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x++) {
            ROP_OP(*(dst_base + m(dst)), *(src_base + m(src)));
            dst--;
            src--;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

static void
glue(glue(cirrus_bitblt_rop_fwd_transp_, ROP_NAME),_8)(CirrusVGAState *s,
						       uint8_t *dst_,const uint8_t *src_,
						       int dstpitch,int srcpitch,
						       int bltwidth,int bltheight)
{
    int x,y;
    uint8_t p;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch -= bltwidth;
    srcpitch -= bltwidth;
    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x++) {
	    p = *(dst_base + m(dst));
            ROP_OP(p, *(src_base + m(src)));
	    if (p != s->gr[0x34]) *(dst_base + m(dst)) = p;
            dst++;
            src++;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

static void
glue(glue(cirrus_bitblt_rop_bkwd_transp_, ROP_NAME),_8)(CirrusVGAState *s,
							uint8_t *dst_,const uint8_t *src_,
							int dstpitch,int srcpitch,
							int bltwidth,int bltheight)
{
    int x,y;
    uint8_t p;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch += bltwidth;
    srcpitch += bltwidth;
    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x++) {
	    p = *(dst_base + m(dst));
            ROP_OP(p, *(src_base + m(src)));
	    if (p != s->gr[0x34]) *(dst_base + m(dst)) = p;
            dst--;
            src--;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

static void
glue(glue(cirrus_bitblt_rop_fwd_transp_, ROP_NAME),_16)(CirrusVGAState *s,
							uint8_t *dst_,const uint8_t *src_,
							int dstpitch,int srcpitch,
							int bltwidth,int bltheight)
{
    int x,y;
    uint8_t p1, p2;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch -= bltwidth;
    srcpitch -= bltwidth;
    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x+=2) {
	    p1 = *(dst_base + m(dst));
	    p2 = *(dst_base + m(dst+1));
            ROP_OP(p1, *(src_base + m(src)));
            ROP_OP(p2, *(src_base + m(src+1)));
	    if ((p1 != s->gr[0x34]) || (p2 != s->gr[0x35])) {
		*(dst_base + m(dst)) = p1;
		*(dst_base + m(dst+1)) = p2;
	    }
            dst+=2;
            src+=2;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

static void
glue(glue(cirrus_bitblt_rop_bkwd_transp_, ROP_NAME),_16)(CirrusVGAState *s,
							 uint8_t *dst_,const uint8_t *src_,
							 int dstpitch,int srcpitch,
							 int bltwidth,int bltheight)
{
    int x,y;
    uint8_t p1, p2;
    uint32_t dst, src;
    uint8_t *dst_base, *src_base;
    get_base(dst_, s, dst_base);
    get_base(src_, s, src_base);
    dst = dst_ - dst_base;
    src = src_ - src_base;
    dstpitch += bltwidth;
    srcpitch += bltwidth;
    for (y = 0; y < bltheight; y++) {
        for (x = 0; x < bltwidth; x+=2) {
	    p1 = *(dst_base + m(dst-1));
	    p2 = *(dst_base + m(dst));
            ROP_OP(p1, *(src_base + m(src-1)));
            ROP_OP(p2, *(src_base + m(src)));
	    if ((p1 != s->gr[0x34]) || (p2 != s->gr[0x35])) {
		*(dst_base + m(dst-1)) = p1;
		*(dst_base + m(dst)) = p2;
	    }
            dst-=2;
            src-=2;
        }
        dst += dstpitch;
        src += srcpitch;
    }
}

#define DEPTH 8
#include "cirrus_vga_rop2.h"

#define DEPTH 16
#include "cirrus_vga_rop2.h"

#define DEPTH 24
#include "cirrus_vga_rop2.h"

#define DEPTH 32
#include "cirrus_vga_rop2.h"

#undef ROP_NAME
#undef ROP_OP

#undef get_base
#undef m