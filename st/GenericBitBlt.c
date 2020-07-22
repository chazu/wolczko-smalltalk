/*
  	Smalltalk-80 Virtual Machine:
	BitBlt (also known as RasterOp)

	Copyright (c) 1984,1985,1986,1987,1988,1989 Mario Wolczko
	All rights reserved.

This module implements the copyBits primitive (G/R p.651). The
algorithm is a direct transliteration (with a few local
optimisations) of that presented on pp.355-61 of G/R.

It is assumed that the fields of the instance of BitBlt have
been fetched from object memory, and are presented directly
to the BitBlt() function.
However, should there be no source bitmap (srcBits==NULL) we have to
ensure that the dest bitmap is correctly half-toned.

$Log: GenericBitBlt.c,v $
 * Revision 5.0  1994/08/16  00:58:30  mario
 * Sync with others
 *
 * Revision 4.1  89/06/21  19:49:07  mario
 * *** empty log message ***
 * 
*/

static char *rcsid = "$Header: /home/mario/st/RCS/GenericBitBlt.c,v 5.0 1994/08/16 00:58:30 mario Exp mario $";

#include "ST.h"



/* the following abbreviations are used:
	s,src   source
	d,dest  destination
	ht      halftone
	h,H     height
	w,W     width
*/


					/*ST!BitBlt!primitiveCopyBits!*/


BitBlt(destBits, srcBits, htBits, combRule, dx, dy, w, h,
	clipX, clipY, clipW, clipH, sx, sy, sw, sh, dw, dh)
unsigned sw, sh, dw, dh;
WORD destBits[], srcBits[], htBits[];
{
					/*ST!BitBlt!initializeBitBlt!*/

	static WORD RightMasks[] = {
		0, 1, 3, 7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
		0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,
		0xFFFF, 0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF,
		0x1FFFFF, 0x3FFFFF, 0x7FFFFF, 0xFFFFFF,
		0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF,
		0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
	};
	int hDir, vDir, srcDelta, destDelta;
	unsigned skew, compSkew, nWords;
	BOOL preload;
	WORD mask1, mask2, skewMask, *srcIndex;
	register WORD *destIndex;

					/*ST!BitBlt!clipRange!*/
	{
		int diffX, diffY;

		if ((diffX = clipX - dx) > 0) {
			sx += diffX;
			w -= diffX;
			dx = clipX;
		}
		MinBec(w, diffX + clipW);
		MinBec(w, dw - dx);

		if ((diffY = clipY - dy) > 0) {
			sy += diffY;
			h -= diffY;
			dy = clipY;
		}
		MinBec(h, diffY + clipH);
		MinBec(h, dh - dy);

		if (srcBits != NULL) {
			if (sx < 0) {
				dx -= sx;
				w += sx;
				sx = 0;
			}
			MinBec(w, sw - sx);

			if (sy < 0) {
				dy -= sy;
				h += sy;
				sy = 0;
			}
			MinBec(h, sh - sy);
		}
	}

	/* test for null range */
	if (w <= 0 || h <= 0)
		return;

					/*ST!BitBlt!computeMasks!*/
	{
		int     startBits = BPW - (dx & (BPW - 1)),
			endBits = BPW - 1 - ((dx + w - 1) & (BPW - 1));

		mask1 = RightMasks[startBits];
		mask2 = ~RightMasks[endBits];
		skew = (sx - dx) & (BPW - 1);
		compSkew = BPW - skew;
		skewMask =  skew==0 ? 0 : RightMasks[compSkew];

		if (w < startBits) {
			mask1 &= mask2;
			mask2 = 0;
			nWords = 1;
		} else
			nWords = (w - startBits + BPW + BPW - 1) >> L2BPW;
	}

					/*ST!BitBlt!checkOverlap!*/
	hDir = vDir = 1;
	if (srcBits == destBits && dy >= sy) {
		if (dy > sy) {
			vDir = -1;
			sy += h - 1;
			dy += h - 1;
		} else if (dx > sx) {
			hDir = -1;
			sx += w - 1;
			dx += w - 1;
			skewMask = ~skewMask;
			{  WORD t;  t = mask1;  mask1 = mask2;  mask2 = t; }
		}
	}

					/*ST!BitBlt!calculateOffsets!*/
	{
		int     destRaster = (dw + BPW - 1) >> L2BPW,
			srcRaster  = (sw + BPW - 1) >> L2BPW;
				/* ignore the fact that there might not be a src */
		preload = srcBits != NULL && skew != 0 && skew <= (sx & (BPW - 1));
		if (hDir < 0)
			preload = !preload;
		srcIndex = srcBits + sy * srcRaster + (sx >> L2BPW);
			/* ignore fact that might not be a src */
		destIndex = destBits + dy * destRaster + (dx >> L2BPW);
			/* NON-PORTABLE: assumes (-1)>>1 == -1 */
		srcDelta = (vDir < 0  ?  -srcRaster  :  srcRaster)
			- (hDir < 0 ?-nWords - (preload ? 1 : 0)
				    : nWords + (preload ? 1 : 0));
		destDelta = (vDir < 0  ?  -destRaster  :  destRaster)
			- (hDir < 0  ?  -nWords  :  nWords);
	}

	{	/* optimised copyLoop */

		if (srcBits == NULL  &&  htBits != NULL  &&  combRule == 3) {
			/* by far the most common case, so specially optimised */
			register WORD htWord;
			unsigned i;
			register unsigned word;

			for (i = 1;  i++ <= h;  destIndex += destDelta) {
				htWord = htBits[dy++ & (BPW - 1)];	/* assert: vDir,hDir==1 */

				/* assert: preload==FALSE */
					
				/* assert nWords >= 1*/
				/* this is the left edge: */
				*destIndex ^= (htWord ^ *destIndex) & mask1;
				++destIndex;

				/* this is the middle */
				for (word = 2;  word++ < nWords;  ) {
					*destIndex++ = htWord;
				}

				if (nWords > 1)	{	/* this is the right edge: */
					*destIndex ^= (htWord ^ *destIndex) & mask2;
					++destIndex;
				}

			}
			return;
		}
	}
					/*ST!BitBlt!copyLoop!*/
	{
		WORD prevWord, htWord, NskewMask = ~skewMask;
		unsigned i, word, nWords1 = nWords - 1;
		register WORD mergeMask, skewWord;

		for (i = 1;  i++ <= h;  srcIndex += srcDelta, destIndex += destDelta) {
			if (htBits != NULL) {
				htWord = htBits[dy & (BPW - 1)];
				dy += vDir;
			} else
				htWord = ONES;
			skewWord = htWord;
			
			if (preload) {
				prevWord = *srcIndex;
				srcIndex += hDir;
			} else
				prevWord = 0;
				
			mergeMask = mask1;
			
			for (word = 1;  word <= nWords;  word++, srcIndex += hDir, destIndex += hDir) {
				if (srcBits != NULL) {
					WORD thisWord = *srcIndex;
					skewWord = (prevWord & skewMask) | (thisWord & NskewMask);
					prevWord = thisWord;
					skewWord = (skewWord << skew) | (skewWord >> compSkew);
				}

#define srcWord         (skewWord & htWord)
#define destWord        (*destIndex)
/* old version:
	define assignDest(x)   (destWord = (mergeMask & (x)) | (~mergeMask & destWord))
*/
/* new version, c/o Rob Pike: */
#define assignDest(x)   (destWord ^= (((x) ^ destWord) & mergeMask))

					/*ST!BitBlt!merge:with:!*/

				switch (combRule) {
				case 0: assignDest(0);
					break;
				case 1: assignDest(srcWord & destWord);
					break;
				case 2: assignDest(srcWord & ~destWord);
					break;
				case 3: assignDest(srcWord);
					break;
				case 4: assignDest(~srcWord & destWord);
					break;
				case 5: assignDest(destWord);
					break;
				case 6: assignDest(srcWord ^ destWord);
					break;
				case 7: assignDest(srcWord | destWord);
					break;
				case 8: assignDest(~srcWord & ~destWord);
					break;
				case 9: assignDest(~srcWord ^ destWord);
					break;
				case 10:assignDest(~destWord);
					break;
				case 11:assignDest(srcWord | ~destWord);
					break;
				case 12:assignDest(~srcWord);
					break;
				case 13:assignDest(~srcWord | destWord);
					break;
				case 14:assignDest(~srcWord | ~destWord);
					break;
				case 15:assignDest(ONES);
					break;
				}
				
				mergeMask = word==nWords1 ? mask2 : ONES;
			}
		}
	}
}

void BitBlt_init()
{
	extern char *malloc();
	extern BITMAP theScreen;
	extern unsigned dispW, dispH;
	
	/* allocate screen */
	theScreen = (BITMAP)malloc(dispW*dispH/8);
}

void update_if_screen(pix, cx, cy, cw, ch, x, y, w, h)
BITMAP pix;
int cx, cy, cw, ch, x, y, w, h;
{
	/* fill this in as appropriate .. */
}
