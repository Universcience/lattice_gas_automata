/*  Quick and dirty implementation of an HPP cellular automaton (SDL2-based).
 *  Copyright (C) 2017-2018 - Jérôme Kirman
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

#define REFLECT 0x10
#define NORTH 0x08
#define EAST  0x04
#define SOUTH 0x02
#define WEST  0x01

#define HX 320
#define HY 240
#define HR 120

#define HOLE(x,y) ( sqrt((HX-x)*(HX-x) + (HY-y)*(HY-y)) < HR )

typedef char cell;

unsigned echo_fps (unsigned interval, void* param)
{
	int* pframes = param;
	printf ("%i FPS\n", *pframes);
	*pframes = 0;
	return interval;
}

int main ()
{
	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_Event e;
	int frames = 0;
	SDL_AddTimer (1000, echo_fps, &frames);

	SDL_Window* mw = SDL_CreateWindow("Modèle HPP", 0, 0, WIDTH, HEIGHT, 0);
	SDL_Renderer* r = SDL_CreateRenderer(mw, -1, 0);
	SDL_Texture* t = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

	SDL_Surface* buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
	uint32_t* pixels = buffer->pixels;

	int border = 0xff00ff00;
	int particle = 0x003f0000;

	// > Paint borders
	for (int x = 0 ; x < WIDTH ; x++)
		pixels[x] = pixels[(HEIGHT-1)*WIDTH + x] = border;
	for (int y = 0 ; y < HEIGHT; y++)
		pixels[y*WIDTH] = pixels[(y+1)*WIDTH - 1] = border;

	// Data init
	srand(time(NULL));

	cell** odata = calloc(WIDTH, sizeof(*odata));
	cell** ndata = calloc(WIDTH, sizeof(*ndata));
	cell** tmp = NULL;
	for (int x = 0 ; x < WIDTH ; x++)
	{
		odata[x] = calloc(HEIGHT, sizeof(**odata));
		ndata[x] = calloc(HEIGHT, sizeof(**ndata));
		for (int y = 0 ; y < HEIGHT ; y++)
		{
			if (!HOLE(x,y))
				odata[x][y] = 1 << rand()%4;
			if (x == 1 || y == 1 || x == WIDTH-2 || y == HEIGHT-2)
				odata[x][y] = REFLECT;
		}
	}

	bool end = false;
	// Main loop
	while (!end)
	{
		// Handle events
		while (SDL_PollEvent(&e))
			if (e.type == SDL_QUIT)
				end = true;

		// Move, bump and draw particles
		for (int x = 1 ; x < WIDTH-1 ; x++)
			for (int y = 1 ; y < HEIGHT-1 ; y++)
			{
				// Move
				ndata[x][y] =
					(odata[x][y] & REFLECT) |
					(odata[x][y+1] & NORTH) |
					(odata[x-1][y] & EAST ) |
					(odata[x][y-1] & SOUTH) |
					(odata[x+1][y] & WEST );

				// Reflector cells + collisions.
				if (ndata[x][y] & REFLECT)
				{
					cell r = ndata[x][y] & 0x0F;
					r = r >> 2 | (r & 0x03) << 2;
					ndata[x][y] = (ndata[x][y] & ~0x0F) | r;
				}
				else if (ndata[x][y] == (NORTH | SOUTH))
					ndata[x][y] = EAST | WEST;
				else if (ndata[x][y] == (WEST | EAST))
					ndata[x][y] = NORTH | SOUTH;

				// Draw, with neat Kernighan bitcount
				unsigned pc = 0, v = ndata[x][y];
				for ( ; v ; pc++)
					v &= v-1;
				pixels[y*buffer->w + x] = particle * pc;
			}

		// Refresh screen
		SDL_RenderClear(r);
		//TODO: Optimize away next line somehow ?
		SDL_UpdateTexture(t, NULL, pixels, buffer->pitch);
		SDL_RenderCopy(r, t, NULL, NULL);
		SDL_RenderPresent(r);

		// Swap data buffers (time step)
		tmp = odata;
		odata = ndata;
		ndata = tmp;

		frames++;
	}

	for (int x = 0 ; x < WIDTH ; x++)
	{
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
