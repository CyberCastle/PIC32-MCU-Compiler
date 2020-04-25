#include <math.h>
#include <stdio.h>
#include <assert.h>


#define MAX16  0x00007FFF
#define MAX32  0x7FFFFFFF


void CalcCoeffs(int n)
{
  char filename[100];

  sprintf(filename, "fft16c%d.c", n);
  FILE *fp16 = fopen(filename, "wt");
  assert(fp16 != NULL);
#if DEBUG
  printf("Created fft16c%d.c\n", n);
#endif

  sprintf(filename, "fft32c%d.c", n);
  FILE *fp32 = fopen(filename, "wt");
  assert(fp32 != NULL);
#if DEBUG
  printf("Created fft32c%d.c\n", n);
#endif

  fprintf(fp16, "#include <dsplib_dsp.h>\n\n");
  fprintf(fp16, "const int16c fft16c%d[%d] =\n{\n", n, n/2);

  fprintf(fp32, "#include <dsplib_dsp.h>\n\n");
  fprintf(fp32, "const int32c fft32c%d[%d] =\n{\n", n, n/2);

  int i;
  for (i = 0; i < (n/2); i++)
  {
	double c = cos(-2.0 * M_PI * i / n);
	double s = sin(-2.0 * M_PI * i / n);

	int c32r = MAX32 * c;
	int c32i = MAX32 * s;

	int c16r = (c32r >> 16) & 0xFFFF;
	int c16i = (c32i >> 16) & 0xFFFF;

	fprintf(fp16, "\t{ 0x%04x, 0x%04x },\n", c16r, c16i);
	fprintf(fp32, "\t{ 0x%08x, 0x%08x },\n", c32r, c32i);
  }

  fprintf(fp16, "};\n\n");
  fprintf(fp32, "};\n\n");

  fclose(fp16);
  fclose(fp32);
}


int main()
{
  int n;

  for (n = 16; n <= 4096; n <<= 1)
  {
	CalcCoeffs(n);
  }

  return 0;
}
