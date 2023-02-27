cube_a = 100;
cube_i = 90;
rip_h = 5;
rip_d = 1;
rip_b = 3;
rip_l = 2;
rip_a = cube_a;
rip_i = cube_a - 2 * rip_b;

cool_h = 6*rip_h + 3 * rip_d;

boden_d = 4;
boden_h = (cube_a-cool_h)/2;

schienen_l = cube_a - 2*boden_d;
schienen_d = 2.4;
schienen_a = 60;
schienen_loch = 2;

rand=1;

$fn=32;
item="all.stl";
print(item);

module print(item="all.stl"){
   if (item=="front.stl") front();
   if (item=="rear.stl") rear();
   if (item=="rip.stl") rip();
   if (item=="rip_center.stl") rip_center();
   if (item=="trayICE40.stl") trayICE40();
   if (item=="tray32u4.stl") tray32u4();
   if (item=="traySDCard.stl") traySDCard();
   if (item=="trayThinker.stl") trayThinker();
   if (item=="all.stl"){
        translate([0,0,-100]) front();
        cool();
        translate([0,0,50])   rear();
        translate([150,0,0])  trayICE40();
        translate([-150,0,0]) tray32u4();
        translate([0,-150,0]) trayThinker();
        translate([0,150,0])  traySDCard();
   }
} 
    
module traySDCard(){
    cube([schienen_a-2*schienen_d,schienen_l,2],center=true);
    translate([0,-(schienen_l-2)/2,2])
    cube([40,2,2],center=true);
    
    translate([0,22.5,0])
    difference(){
        
        union(){
//            translate([0,2,3.5])
 //               cube([42,5,5],center=true);
            translate([18,2,1])
                cylinder(5,3,3);
            translate([-18,2,1])
                cylinder(5,3,3);

            translate([0,0,1+9/2])
                cube([38,35,9],center=true);
        }
        
    translate([0,2,1+9/2])
    cube([30,31,9],center=true);
    
    translate([0,1,9])
    cube([38,33,2],center=true);
    translate([0,1,1+6])
    cube([34,33,2],center=true);
    translate([0,2,1+6])
    cube([38,5,2],center=true);
    translate([18,2,1])
        cylinder(5,1.2,1.2);
    translate([-18,2,1])
        cylinder(5,1.2,1.2);

    }

}
module trayThinker(){
    cube([schienen_a-2*schienen_d,schienen_l,2],center=true);
    translate([0,-(schienen_l-2)/2,2])
    cube([40,2,2],center=true);
    translate([0,-10,1])
    rotate(90)
    scale(0.3)
import("thinker.stl", convexity = 5);
}

module tray32u4(){
    cube([cube_i,schienen_l,2],center=true);
    translate([0,-(schienen_l-2)/2,2])
    cube([60,2,2],center=true);
    translate([-11,-42,1])
        nupsi(5,6,2.4);
    translate([37,-42,1])
        nupsi(5,6,2.4);
    translate([4,8.5,1])
        nupsi(5,6,2.4);
    translate([32,8.5,1])
        nupsi(5,6,2.4);
}


module trayICE40(){
        cube([cube_i,schienen_l,2],center=true);
    translate([0,-(schienen_l-2)/2,2])
    cube([60,2,2],center=true);

    translate([-30.5,-23,1])
        nupsi(3,6,2.4);
    translate([13.5,-23,1])
        nupsi(3,6,2.4);
    translate([-30.5,16,1])
        nupsi(3,6,2.4);
    translate([13.5,16,1])
        nupsi(3,6,2.4);
}

module nupsi(h,a,i){
    difference(){
        cylinder(h,a/2,a/2);
        cylinder(h,i/2,i/2);
    }
}

module rear(){
    //Bodenplatte
    translate([0,0,cube_a/2-boden_d/2])
    difference(){
        bodenplatte();
        holes();
    }
    translate([0,0,(cool_h+boden_h-boden_d)/2])
        rahmen(cube_a,cube_i,boden_h-boden_d,rand);
    translate([0,0,(cool_h-rip_d)/2])
        rahmen(cube_a-2*rip_l,cube_i,rip_d,0);
    

}

module holes(){
    hole();
    mirror([0,1,0]) hole();
    mirror([0,1,0]) mirror([1,0,0]) hole();
    mirror([1,0,0]) hole();
    
}

module hole(){
    translate([schienen_a/2,(cube_i-2*schienen_d)/2,0])
    cylinder(boden_d,schienen_loch/2,schienen_loch/2,center=true); 
    translate([schienen_a/2,(cube_i-2*schienen_d)/2,boden_d/4])
    cylinder(boden_d/2,schienen_loch,schienen_loch,center=true);
}

module bodenplatte(){
    difference(){
        cubebevel(cube_a,boden_d,rand);
        translate([0,22.5,(boden_d-1)/2])
            cube([52,47,1],center=true);
        translate([cube_a/2,-cube_a/2,boden_d/2])
            rotate([270,0,90])
                bevel(cube_a,rand);
        translate([cube_a/2,cube_a/2,boden_d/2])
            rotate([270,0,180])
                bevel(cube_a,rand);
        translate([-cube_a/2,cube_a/2,boden_d/2])
            rotate([270,0,270])
                bevel(cube_a,rand);
        translate([-cube_a/2,-cube_a/2,boden_d/2])
            rotate([270,0,0])
                bevel(cube_a,rand); 
    }
    for (a=[1.5:3:43.5])
    translate([0,a,boden_d/2-1])prism(50, 3, 1);
    
}

module front(){
    //
    translate([0,0,(-cube_a+boden_h+boden_d)/2])
        rahmen(cube_a,cube_i,boden_h-boden_d,rand);
    translate([0,0,(-cool_h+rip_d)/2])
        rahmen(cube_a-2*rip_l,cube_i,rip_d,0);
    rahmen(cube_a-2*rip_b-0.2,cube_i,cube_a-2*boden_d,rand);
    //Bodenplatte
    translate([0,0,(-cube_a+boden_d)/2])
            mirror([0,0,1])
                difference(){
                    union(){
                    bodenplatte();
                    translate([0,35.5,-4])
                    cube([40,10,4],center=true);
                    }
                    translate([0,34.5,-2])
                    cube([26,3,boden_d+4],center=true);
                    translate([0,35.5,-3])
                    cube([36,6,6],center=true);
                }
    
    //rahmen halter
    four();
}



module four(){
    streben();
    mirror([0,1,0]) streben();
    mirror([0,1,0]) mirror([1,0,0]) streben();
    mirror([1,0,0]) streben();
}

module streben(){
    translate([(cube_i-3*schienen_d)/2,(cube_i-schienen_d)/2,0])
        cube([schienen_d,schienen_d,schienen_l],center=true);
    translate([schienen_a/2,(cube_i-2*schienen_d)/2,0])
    
        difference(){
            cube([2*schienen_d,2*schienen_d,schienen_l],center=true);
            translate([0,0,(schienen_l-6)/2])
            cylinder(6,schienen_loch/2,schienen_loch/2,center=true);    
        }
        
    translate([schienen_a/2-1.5*schienen_d,(cube_i-3*schienen_d)/2,0])
        cube([schienen_d,schienen_d,schienen_l],center=true);
}

module cool(){
    delta=(rip_d+rip_h)/2;
    //for(a=[1,3,5]){
    for(a=[0,1,2]){
    translate([0,0,delta+a*rip_h])mirror([0,0,1])rip();
    translate([0,0,-delta-a*rip_h])rip();
    }
    rip_center();
}

module rip_center(){
    rahmen(rip_a,rip_i,rip_d,rand);
}

module rip(){
    rahmen(rip_a-2*rip_l,rip_i,rip_h);
     translate([0,0,(-rip_h+rip_d)/2])
        rahmen(rip_a,rip_i,rip_d,rand);
}

module rahmen(aussen,innen,hoehe,rand){
    difference(){
        cubebevel(aussen,hoehe,rand);
        cube([innen,innen,hoehe],center=true);  
    }
}

module cubebevel(aussen,hoehe,rand){
        difference(){
        cube([aussen,aussen,hoehe],center=true);
        translate([aussen/2,-aussen/2,-hoehe/2])
            rotate([0,270,0])
                bevel(hoehe,rand);
        translate([aussen/2,aussen/2,-hoehe/2])
            rotate([0,270,90])
                bevel(hoehe,rand);
        translate([-aussen/2,aussen/2,-hoehe/2])
            rotate([0,270,180])
                bevel(hoehe,rand);
        translate([-aussen/2,-aussen/2,-hoehe/2])
            rotate([0,270,270])
                bevel(hoehe,rand);   
    }
}


 module prism(l, w, h){
      polyhedron(//pt 0        1        2        3        4        5
              points=[[-l/2,-w/2,0], [l/2,-w/2,0], [l/2,0,h], [-l/2,0,h], [-l/2,w/2,0], [l/2,w/2,0]],
              faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
              );      
      }
  
 module bevel(l, w){
      polyhedron(//pt 0        1        2        3        4        5
              points=[[0,0,0], [l,0,0], [l,0,w], [0,0,w], [0,w,0], [l,w,0]],
              faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
              );      
      }
  
