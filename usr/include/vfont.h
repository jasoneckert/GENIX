/*
 * @(#)vfont.h	3.4	7/14/83
 * @(#)Copyright (C) 1983 by National Semiconductor Corp.
 */

/*
 * The structures header and dispatch define the format of a font file.
 *
 * See vfont(5) for more details.
 */

# define VF_MAGIC	286	/* vfont font header magic number	*/
# define MF_MAGIC	287	/* Genix font header magic number	*/

/*
 * Vfont font header
 */
struct header {
	short		magic;
	unsigned short	size;	/* Bitmap size	*/
	short		maxx;
	short		maxy;
	short		xtend;
};

/*
 * Genix font header
 *	Same size as vfont font header, but two fields are used
 *	for different purposes: to record first and last character
 *	index actually contained in the font.  (Thus information
 *	for the character 'A' lives at chardes index position
 *	'A'-first_ch.)
 */
typedef struct font_header {
	short		magic;
	unsigned short	size;		/* Bitmap size			*/
	short		first_ch;	/* Index of 1st char in font	*/
	short		last_ch;	/* Index of last char in font	*/
	short		xtend;
} font_header;

/*
 * Full version used by vfont and related programs
 */
struct dispatch {
	unsigned short	addr;	/* really an offset */
	short		nbytes;
	char		up;
	char		down;
	char		left;
	char		right;
	short		width;
};

/*
 * Stripped down version used for Genix font files.
 */
typedef struct chardes {
	unsigned short	offset;
	short		nbytes;
	char		up;
	char		down;
	char		left;
	char		right;
} chardes;
