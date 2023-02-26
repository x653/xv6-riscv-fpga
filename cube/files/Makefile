
all:	front.stl rear.stl
front.stl:	cube.scad
	openscad -D item=1 -o front.stl cube.scad

rear.stl:	cube.scad
	openscad -D item=2 -o rear.stl cube.scad


clean:
	rm -f front.stl rear.stl

.PHONY: all clean
