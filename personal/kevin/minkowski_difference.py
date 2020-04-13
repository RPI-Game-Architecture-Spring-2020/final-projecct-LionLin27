import sys
import os
import math
import pygame

class vec2():
	def __init__(self, x, y):
		self.x = x
		self.y = y
		
	def __add__(self, other):
		return vec2(self.x + other.x, self.y + other.y)
		
	def __sub__(self, other):
		return vec2(self.x - other.x, self.y - other.y)
		
	def __str__(self):
		return "<%d, %d>" % (self.x, self.y)
		
	def __mul__(self, other):
		return vec2(self.x * other, self.y * other)
		
	def to_int_tuple(self):
		return (int(self.x), int(self.y))
		
	def dist(self, other):
		return math.sqrt(math.pow(self.x - other.x, 2) + math.pow(self.y - other.y, 2))
		
class circle():
	def __init__(self, x, y, r, c):
		self.pos = vec2(x, y)
		self.radius = r
		self.color = c
		
	def is_clicked(self, clickPos):
		if self.pos.dist(vec2(clickPos[0], clickPos[1])) < self.radius:
			return True
		return False
		
	def move(self, dir):
		self.pos.x += dir[0]
		self.pos.y += dir[1]
		
	def draw(self, screen, selected):
		if selected:
			# Draw a yellow circle slightly larger to get an outline.
			pygame.draw.circle(screen, (255, 255, 0), self.pos.to_int_tuple(), self.radius + 2)
		pygame.draw.circle(screen, self.color, self.pos.to_int_tuple(), self.radius)

def run():
	success, fail = pygame.init()
	print("Pygame initialized with %d successes and %d failures." % (success, fail))
	
	screenSize = (640, 480)
	origin = vec2(screenSize[0] / 2, screenSize[1] / 2)

	screen = pygame.display.set_mode(screenSize)
	clock = pygame.time.Clock()
	
	circle1 = circle(50, 50, 40, (255, 0, 0))
	circle2 = circle(150, 150, 50, (0, 0, 255))
	
	minkowskiDiff = circle(0, 0, 0, (0, 255, 0))
	
	selected = None
	
	running = True
	while (running):
		clock.tick(60)
		
		screen.fill((0,0,0))
		
		for event in pygame.event.get():
			if (event.type == pygame.QUIT):
				running = False
			elif (event.type == pygame.KEYDOWN):
				if (event.key == pygame.K_ESCAPE):
					running = False
			elif (event.type == pygame.MOUSEBUTTONDOWN):
				clickPos = pygame.mouse.get_pos()
				for circ in [circle1, circle2]:
					if (circ.is_clicked(clickPos)):
						selected = circ
						break
			elif (event.type == pygame.MOUSEBUTTONUP):
				selected = None
				
		mouseMovement = pygame.mouse.get_rel()
		if not selected is None:
			selected.move(mouseMovement)
			
		# Update the Minkowski difference.
		c1pos = circle1.pos - origin
		c2pos = circle2.pos - origin
		minkowskiDiff.pos = (c1pos - c2pos) + origin
		minkowskiDiff.radius = circle1.radius + circle2.radius
					
		# Draw the axes.
		pygame.draw.line(screen, (255, 255, 255), (screenSize[0] / 2, 0), (screenSize[0] / 2, screenSize[1]), 2)
		pygame.draw.line(screen, (255, 255, 255), (0, screenSize[1] / 2), (screenSize[0], screenSize[1] / 2), 2)
				
		# Draw the circles.
		circle1.draw(screen, selected is circle1)
		circle2.draw(screen, selected is circle2)
		minkowskiDiff.draw(screen, False)
		
		pygame.display.flip()
		
	pygame.quit()
	
	
if __name__ == "__main__":
	run()