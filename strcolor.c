#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define clear() printf("\033[2J\033[0;0H");

typedef struct
{
	unsigned char r, g, b;
} RGB;

// Thank you: https://geekymonkey.com/Programming/CSharp/RGB2HSL_HSL2RGB.htm
RGB hsl2rgb(float h, float s, float l)
{
	float r, g, b;
	r = g = b = l;
	float v = (l <= .5f) ? (l * (1.f + s)) : (1.f + s - l * s);
	if (v > 0.f) {
		float m, sv, fract, vsf, mid1, mid2;
		int sx;
		m = l + l - v;
		sv = (v - m) / v;
		h *= 6.f;
		sx = (int) h;
		fract = h - sx;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch(sx)
		{
			case 0:
                              r = v;
                              g = mid1;
                              b = m;
                              break;
                        case 1:
                              r = mid2;
                              g = v;
                              b = m;
                              break;
                        case 2:
                              r = m;
                              g = v;
                              b = mid1;
                              break;
                        case 3:
                              r = m;
                              g = mid2;
                              b = v;
                              break;
                        case 4:
                              r = mid1;
                              g = m;
                              b = v;
                              break;
                        case 5:
                              r = v;
                              g = m;
                              b = mid2;
                              break;
		}
	}
	RGB out = {
		round(r * 255.f),
		round(g * 255.f),
		round(b * 255.f)
	};
	return out;
}

struct sigaction *sa;

void handler(int s)
{
	clear();
	system("tput cvvis");
	free(sa);
	exit(0);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s <string>\n", argv[0]);
		return 0;
	}

	// Create sighandler for post-interrupt actions
	sa = calloc(1, sizeof(struct sigaction));
	if (!sa) {
		printf("Not enough memory\n");
		return 1;
	}
	sa->sa_handler = handler;
	sigaction(SIGINT, sa, NULL);
	system("tput civis");

	clock_t cl, dt;
	cl = clock();
	int fps = CLOCKS_PER_SEC / 30; // 30 FPS

	float hue = 0.f;
	float d = 0.02f; // Hue delta

	while(1) {
		dt = clock() - cl;
		cl = clock();
		if (dt < fps) usleep(fps - dt); // Limit FPS

		// Print string
		clear();
		for (int i = 0; i < strlen(argv[1]); ++i) {
			RGB v = hsl2rgb(fmod(hue + (i * d), 1.f), .5f, .5f);
			printf("\x1b[38;2;%d;%d;%dm%c", v.r, v.g, v.b, argv[1][i]);
		}
		fflush(stdout);

		hue = fmod(hue + d, 1.f); // Advance hue
	}
}