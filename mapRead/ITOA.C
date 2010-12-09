/*----------------------------------------------------------------------*\
 | DG/UX supplies an itoa function that has no radix argument; I assume |
 | this means that it only does decimal conversion.                     | 
 |                                                                      |
 | This version should accept any radix.                                |
\*----------------------------------------------------------------------*/

char *itoa (int v, char *s, int r) {
	int i,neg = 0;
	char *p = s;
	char *q = s;

	if (r < 0 || r > 35) {
		*s = 0;
		return (s);
		}
	if (r == 0) r = 10;
	if (v == 0) {
		*p++ = '0';
		*p = 0;
		return (s);
		}
	if (v < 0) {
		neg = 1;
		v = -v;
		}
	while (v > 0) {
		i = v % r;
		if (i > 9) i += 7;
		*p++ = '0' + i;
		v /= r;
		}
	if (neg) *p++ = '-';
	*p-- = 0;
	q = s;
	while (p > q) {
		i = *q;
		*q++ = *p;
		*p-- = i;
		}
	return (s);
	}
