OBJS = front.stl rear.stl rip.stl rip_center.stl trayICE40.stl tray32u4.stl traySDCard.stl trayThinker.stl

all:	$(OBJS)

%.stl:	cube.scad
	openscad -D 'item="$@"' -o $@ cube.scad

clean:
	rm -f $(OBJS)

.PHONY: all clean
