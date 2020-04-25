static inline void fir8x4(int16 *r, uint8 *src, int srcstride, int16 *coef)
{
	r[0] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[1];
	r[1] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[2];
	r[1] += *src * coef[1];
	r[2] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[3];
	r[1] += *src * coef[2];
	r[2] += *src * coef[1];
	r[3] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[4];
	r[1] += *src * coef[3];
	r[2] += *src * coef[2];
	r[3] += *src * coef[1];
	src += srcstride;

	r[0] += *src * coef[5];
	r[1] += *src * coef[4];
	r[2] += *src * coef[3];
	r[3] += *src * coef[2];
	src += srcstride;

	r[1] += *src * coef[5];
	r[2] += *src * coef[4];
	r[3] += *src * coef[3];
	src += srcstride;

	r[2] += *src * coef[5];
	r[3] += *src * coef[4];
	src += srcstride;

	r[3] += *src * coef[5];
}


static inline void fir16x4(int32 *r, uint16 *src, int srcstride, int16 *coef)
{
	r[0] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[1];
	r[1] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[2];
	r[1] += *src * coef[1];
	r[2] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[3];
	r[1] += *src * coef[2];
	r[2] += *src * coef[1];
	r[3] = *src * coef[0];
	src += srcstride;

	r[0] += *src * coef[4];
	r[1] += *src * coef[3];
	r[2] += *src * coef[2];
	r[3] += *src * coef[1];
	src += srcstride;

	r[0] += *src * coef[5];
	r[1] += *src * coef[4];
	r[2] += *src * coef[3];
	r[3] += *src * coef[2];
	src += srcstride;

	r[1] += *src * coef[5];
	r[2] += *src * coef[4];
	r[3] += *src * coef[3];
	src += srcstride;

	r[2] += *src * coef[5];
	r[3] += *src * coef[4];
	src += srcstride;

	r[3] += *src * coef[5];
}
