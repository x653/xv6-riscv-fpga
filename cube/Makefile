OBJS = front.stl rear.stl rip.stl rip_center.stl trayICE40.stl tray32u4.stl traySDCard.stl trayThinker.stl

all:	$(OBJS)

front.stl:	cube.scad
	openscad -D item=1 -o front.stl cube.scad
rear.stl:	cube.scad
	openscad -D item=2 -o rear.stl cube.scad
rip.stl:	cube.scad
	openscad -D item=3 -o rip.stl cube.scad
rip_center.stl:	cube.scad
	openscad -D item=4 -o rip_center.stl cube.scad
trayICE40.stl:	cube.scad
	openscad -D item=5 -o trayICE40.stl cube.scad
tray32u4.stl:	cube.scad
	openscad -D item=6 -o tray32u4.stl cube.scad
traySDCard.stl:	cube.scad
	openscad -D item=7 -o traySDCard.stl cube.scad
trayThinker.stl:	cube.scad
	openscad -D item=8 -o trayThinker.stl cube.scad

clean:
	rm -f $(OBJS)

.PHONY: all clean
