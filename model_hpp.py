#!/usr/bin/env python

# Quick and dirty implementation of an HPP cellular automaton (PyGame-based).
# Copyright (C) 2017 - Jérôme Kirman
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from array import array
import random
import math

import pygame
import time

WIDTH=500
HEIGHT=500

EXPECTED=500000

AVERAGE = EXPECTED * 1.0 / (WIDTH*HEIGHT)
RAND_PREC = 1000
def newcell():
	count = int(AVERAGE)
	if (AVERAGE-count)*RAND_PREC > random.randint(0,RAND_PREC):
		count += 1
	count = min(count,4)

	if count == 0:
		return 0
	elif count == 1:
		return random.choice([1,2,4,8])
	elif count == 2:
		return random.choice([3,5,6,9,10,12])
	elif count == 3:
		return random.choice([7,11,13,14])
	elif count == 4:
		return 15

# Initialize space with particles
space = []
for x in range(WIDTH):
	space.append(array("B"))
	for y in range(HEIGHT):
		space[x].append(newcell())

# Cleanup sides to prevent glitches (?)
for x in range(WIDTH):
	space[x][0] = 0
	space[x][HEIGHT-1] = 0
for y in range(HEIGHT):
	space[0][y] = 0
	space[WIDTH-1][y] = 0

# Diggy hole !
#for x in range(40,80):
#	for y in range(40,80):
#		space[x][y] = 0
cx,cy = 200,300
r = 100
for x in range(WIDTH):
	for y in range(HEIGHT):
		if math.sqrt((cx-x)**2 + (cy-y)**2) < r:
			space[x][y] = 0

# north: c & 8
# west:  c & 4
# south: c & 2
# east:  c & 1
def bump():
	global space
	# Particle bump
	for x in range(WIDTH):
		for y in range(HEIGHT):
			c = space[x][y]
			if c == 5:
				space[x][y] = 10
			elif c == 10:
				space[x][y] = 5
	# Wall bump
	for x in range(WIDTH):
		if space[x][0] == 2:
			space[x][0] = 8
		if space[x][HEIGHT-1] == 8:
			space[x][HEIGHT-1] = 2
	for y in range(HEIGHT):
		if space[0][y] == 4:
			space[0][y] = 1
		if space[WIDTH-1][y] == 1:
			space[WIDTH-1][y] = 4

nspace = []
for x in range(WIDTH):
	nspace.append(array("B"))
	for y in range(HEIGHT):
		nspace[x].append(0)

def move():
	global space,nspace
	for x in range(1,WIDTH-1):
		for y in range(1,HEIGHT-1):
			nspace[x][y] = (0
			             + (2 if space[x][y+1] & 2 else 0)
			             + (1 if space[x-1][y] & 1 else 0)
			             + (8 if space[x][y-1] & 8 else 0)
			             + (4 if space[x+1][y] & 4 else 0))
	for x in range(WIDTH):
		nspace[x][0] = 2 if space[x][1] & 2 else 0
		nspace[x][HEIGHT-1] = 8 if space[x][HEIGHT-2] & 8 else 0
	for y in range(HEIGHT):
		nspace[0][y] = 4 if space[1][y] & 4 else 0
		nspace[WIDTH-1][y] = 1 if space[WIDTH-2][y] & 1 else 0

	space,nspace = nspace,space

pygame.init()

window = pygame.display.set_mode((WIDTH,HEIGHT))
clock = pygame.time.Clock()

black = pygame.Color(0,0,0)

c0 = pygame.Color(0,0,0)
c1 = pygame.Color(0,64,0)
c2 = pygame.Color(0,128,0)
c3 = pygame.Color(0,192,0)
c4 = pygame.Color(0,255,0)

def color(c):
	if c == 0:
		return c0
	if c == 15:
		return c4
	if c in [1,2,4,8]:
		return c1
	if c in [7,11,13,14]:
		return c3
	return c2

otime = time.time()
frame = 0

while(True):

	bump()
	move()

	window.fill(black)
	for x in range(WIDTH):
		for y in range(HEIGHT):
			window.set_at((x,y), color(space[x][y]))
	
	pygame.display.update()
	clock.tick(60)

	frame += 1
	ntime = time.time()
	if ntime - otime > 1:
		print frame, " FPS"
		frame = 0
		otime = ntime
