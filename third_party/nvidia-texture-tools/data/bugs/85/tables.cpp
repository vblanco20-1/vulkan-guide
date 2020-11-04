
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8;


static int Mul8Bit(int a, int b)
{
	int t = a * b + 128;
	return (t + (t >> 8)) >> 8;
}

// this exactly matches the (fm*2 + to)/3
static inline int Lerp13_16bit(int fm, int to)
{
	int t = fm * (2 * 0xAAAB) + to * 0xAAAB;
	return t >> 17;
}

static inline int Lerp13(int fm, int to)
{
	return (fm * 2 + to) / 3;
}


static void PrepareOptTable(uint8 * Table, const uint8 * expand, int size)
{
	for (int i = 0; i < 256; i++)
	{
		float bestErr = 256;

		for (int min = 0; min < size; min++)
		{
			for (int max = 0; max < size; max++)
			{
				int mine = expand[min];
				int maxe = expand[max];
				//if (maxe - mine < 32)
				{
					//printf("%d <-> %d\n", maxe + Mul8Bit(mine-maxe, 0x55), Lerp13(maxe, mine));
					//int err = abs(Lerp13_16bit(mine, maxe) - i);
					//int err = abs(maxe + Mul8Bit(mine-maxe, 0x55) - i);
					float err = abs(Lerp13(maxe, mine) - i);
					//err += 0.03f * abs(maxe - mine);
					err += 0.03f * abs(max - min);

					if (err < bestErr)
					{
						Table[i*2+0] = max;
						Table[i*2+1] = min;
						bestErr = err;
					}
				}
			}
		}
		printf("%d: %f %d\n", i, bestErr, abs(Table[i*2+0] - Table[i*2+1]));
	}
}


int main()
{
	uint8 OMatch5[256*2];
	uint8 OMatch6[256*2];
	
	uint8 Expand5[32];
	uint8 Expand6[64];

	for (int i=0; i<32; i++)
		Expand5[i] = (i<<3)|(i>>2);

	for (int i=0; i<64; i++)
		Expand6[i] = (i<<2)|(i>>4);

	PrepareOptTable(OMatch5, Expand5, 32);
	PrepareOptTable(OMatch6, Expand6, 64);

	printf("const static uint8 OMatch5[256][2] = {\n");
	for (int i = 0; i < 256; i++)
	{
		printf("\t{0x%.2X, 0x%.2X},\n", OMatch5[2*i+0], OMatch5[2*i+1]);
	}
	printf("}\n");

	printf("const static uint8 OMatch6[256][2] = {\n");
	for (int i = 0; i < 256; i++)
	{
		printf("\t{0x%.2X, 0x%.2X},\n", OMatch6[2*i+0], OMatch6[2*i+1]);
	}
	printf("}\n");

	return 0;
}

