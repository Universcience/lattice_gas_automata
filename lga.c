/*  Quick and dirty implementation of a lattice gas cellular automaton.
 *  Copyright (C) 2017-2019 - Jérôme Kirman
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>

#define WIDTH  800
#define HEIGHT 600

#ifdef HPP
	#define PARTICLES 0x0F
	#define REFLECT 0x10
#elif FHP
	#define PARTICLES 0x7F
	#define REFLECT 0x80
#endif

// HPP particles
#define WEST  0x01
#define SOUTH 0x02
#define EAST  0x04
#define NORTH 0x08

// FHP particles
#define UL 0x01
#define ML 0x02
#define DL 0x04
#define DR 0x08
#define MR 0x10
#define UR 0x20
#define ST 0x40

/* The ugly truth of hexes:

 01--02--03--04 XX          01--02--03--04
 | \ | \ | \ | \            / \ / \ / \ / \
 05--06--07--08-09        05--06--07--08--09
 | / | / | / | /    <-->    \ / \ / \ / \ /
 10--11--12--13 XX          10--11--12--13
 | \ | \ | \ | \            / \ / \ / \ / \
 14--15--16--17-18        14--15--16--17--18

 Neighborhood of (x,y):

 d |  y even  |  y odd
---+----------+----------
UR | x+1  y-1 |  x   y-1
MR | x+1   y  | x+1   y
DR | x+1  y+1 |  x   y+1
DL |  x   y+1 | x-1  y+1
ML | x-1   y  | x-1   y
UL |  x   y-1 | x-1  y-1
*/

//TODO: Add FHP-III rules.
//TODO: Gauge pressure.

#define HX 320
#define HY 240
#define HR 120

#define HOLE(x,y) ( sqrt((HX-x)*(HX-x) + (HY-y)*(HY-y)) < HR )
#define WALL(x,y) ( x == 1 || y == 1 || x == WIDTH-2 || y == HEIGHT-2 )

typedef unsigned short cell;

unsigned echo_fps (unsigned interval, void* param);
unsigned bitc (cell n);
void hpp_update (cell** odata, cell** ndata, int x, int y);
void fhp_update (cell** odata, cell** ndata, int x, int y);

#ifdef HPP
	void (*lga_update) (cell** odata, cell** ndata, int x, int y) = hpp_update;
#elif FHP
	void (*lga_update) (cell** odata, cell** ndata, int x, int y) = fhp_update;
#endif

unsigned echo_fps (unsigned interval, void* param)
{
	int* fps = param;
	printf ("%i FPS\n", *fps);
	*fps = 0;
	return interval;
}

// Neat Kernighan bitcount.
inline unsigned bitc (cell n)
{
	unsigned c = 0;
	for ( ; n ; c++)
		n &= (cell) (n-1);
	return c;
}

void hpp_update (cell** odata, cell** ndata, int x, int y)
{
	// Move
	ndata[x][y] = (cell) (
		(odata[x][y] & REFLECT) |
		(odata[x][y+1] & NORTH) |
		(odata[x-1][y] & EAST ) |
		(odata[x][y-1] & SOUTH) |
		(odata[x+1][y] & WEST ));

	// Reflector cells + collisions.
	if (ndata[x][y] & REFLECT) {
		cell rf = ndata[x][y] & 0x0F;
		rf = (cell) (rf >> 2 | (rf & 0x03) << 2);
		ndata[x][y] = (cell) ((ndata[x][y] & ~0x0F) | rf);
	}
	else if (ndata[x][y] == (NORTH | SOUTH))
		ndata[x][y] = EAST | WEST;
	else if (ndata[x][y] == (WEST | EAST))
		ndata[x][y] = NORTH | SOUTH;
}

void fhp_update (cell** odata, cell** ndata, int x, int y)
{
	// Move
	if (y%2 == 0)
		ndata[x][y] = (cell) (
			(odata[x][y] & REFLECT) |
			(odata[ x ][y+1] & UR) |
			(odata[x-1][ y ] & MR) |
			(odata[ x ][y-1] & DR) |
			(odata[x+1][y-1] & DL) |
			(odata[x+1][ y ] & ML) |
			(odata[x+1][y+1] & UL));
	else
		ndata[x][y] = (cell) (
			(odata[x][y] & REFLECT) |
			(odata[x-1][y+1] & UR) |
			(odata[x-1][ y ] & MR) |
			(odata[x-1][y-1] & DR) |
			(odata[ x ][y-1] & DL) |
			(odata[x+1][ y ] & ML) |
			(odata[ x ][y+1] & UL));

	// Reflector cells + collisions.
	if (ndata[x][y] & REFLECT) {
		cell rf = ndata[x][y] & 0x3F;
		rf = (cell) (rf >> 3 | (rf & 0x7) << 3);
		ndata[x][y] = (cell) ((ndata[x][y] & ~0x3F) | rf);
	}
	else if (ndata[x][y] == (UR | DR | ML))
		ndata[x][y] = UL | DL | MR;
	else if (ndata[x][y] == (UL | DL | MR))
		ndata[x][y] = UR | DR | ML;
	else if (ndata[x][y] == (UR | DL))
		ndata[x][y] = rand()%2 ? MR | ML : UL | DR;
	else if (ndata[x][y] == (MR | ML))
		ndata[x][y] = rand()%2 ? UR | DL : UL | DR;
	else if (ndata[x][y] == (UL | DR))
		ndata[x][y] = rand()%2 ? MR | ML : UR | DL;
}

int main (void)
{
	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_Event e;
	unsigned frames = 0;
	SDL_AddTimer (1000, echo_fps, &frames);

	SDL_Window* mw = SDL_CreateWindow("Lattice Gas Automaton", 0, 0, WIDTH, HEIGHT, 0);
	SDL_Renderer* r = SDL_CreateRenderer(mw, -1, 0);
	SDL_Texture* t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

	SDL_Surface* buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
	uint32_t* pixels = buffer->pixels;

	unsigned border = 0x00ff00ff,
	       particle = 0x00002000;

	// Data init
	srand((unsigned) time(NULL));

	cell** odata = calloc(WIDTH, sizeof(*odata));
	cell** ndata = calloc(WIDTH, sizeof(*ndata));
	cell** tmp = NULL;
	for (int x = 0 ; x < WIDTH ; x++) {
		odata[x] = calloc(HEIGHT, sizeof(**odata));
		ndata[x] = calloc(HEIGHT, sizeof(**ndata));
		for (int y = 0 ; y < HEIGHT ; y++) {
			if (!HOLE(x,y))
				odata[x][y] = (cell) (1 << rand() % (int) bitc(PARTICLES));
			if (WALL(x,y))
				odata[x][y] = REFLECT;
		}
	}

	// Main loop
	bool end = false;
	while (!end) {
		// Handle events
		while (SDL_PollEvent(&e))
			if (e.type == SDL_QUIT)
				end = true;

		// Move, bump and draw particles
		for (int x = 1 ; x < WIDTH-1 ; x++)
			for (int y = 1 ; y < HEIGHT-1 ; y++) {
				lga_update(odata, ndata, x, y);

				pixels[y*buffer->w + x] = particle * bitc(ndata[x][y] & PARTICLES)
				                        | ((ndata[x][y] & REFLECT) ? border : 0);
			}

		// Refresh screen
		SDL_RenderClear(r);
		SDL_UpdateTexture(t, NULL, pixels, buffer->pitch);
		SDL_RenderCopy(r, t, NULL, NULL);
		SDL_RenderPresent(r);

		// Swap data buffers (time step)
		tmp = odata;
		odata = ndata;
		ndata = tmp;

		frames++;
	}

	for (int x = 0 ; x < WIDTH ; x++) {
		free (odata[x]);
		free (ndata[x]);
	}
	free (odata);
	free (ndata);
	
	SDL_FreeSurface(buffer);
	SDL_DestroyTexture(t);
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(mw);
	SDL_Quit();

	return EXIT_SUCCESS;
}
