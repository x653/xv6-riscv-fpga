//Parameter
cube_a = 100;       //Kantenlänge außen
cube_d = 2;         //Dicke der Wand
boden_d = 4;        //Boden Dicke


rip_h = 5;          //Rippe Abstand
rip_d = 1;          //Rippen Dicke
rip_l = 2;          //Rippen Länge


schienen_d = 2.4;               //Dicke der Schienen
schienen_a = 60;                //Abstand der Schienen
schienen_loch = 2.4;            //Lochdurchmesser       

rand=0.7;                       //Bevel

$fn=32;                         //Ecken für Kreise

//Hilfsparameter
rip_a = cube_a;
rip_b = rip_d+rip_l;          
rip_i = cube_a - 2 * rip_b;
cool_h = 6*rip_h + 3 * rip_d;   //gesamte Kühlrippen Höhe
schienen_l = cube_a - 2*boden_d;    
boden_h = (cube_a-cool_h)/2;    
cube_i = cube_a-2*rip_b-2*cube_d;

//Was soll gezeigt werden?
item="all.stl";
print(item);

//Select the item to print
module print(item="all.stl"){
   if (item=="front.stl") front();
   if (item=="rear.stl") rear();
   if (item=="rip.stl") rip();
   if (item=="rip_center.stl") rip_center();
   if (item=="trayICE40.stl") trayICE40();
   if (item=="tray32u4.stl") tray32u4();
   if (item=="traySDCard.stl") traySDCard();
   if (item=="trayThinker.stl") trayThinker();
   if (item=="logo.stl") logo_plate();
   if (item=="all.stl"){
        cool();
        translate([0,0,-100]) front();
        translate([0,0,50])   rear();
        translate([150,0,0])  trayICE40();
        translate([-150,0,0]) tray32u4();
        translate([0,-150,0]) trayThinker();
        translate([0,150,0])  traySDCard();
   }
} 


//Logo on small plate
module logo_plate(){
    difference(){
        cubebevel(15.8,15.8,1.8,0.3);
        translate([0,0,0.9])mirror([0,0,1])logo(6);
    }
}
    
//The cube logo with an X
module logo(laenge){
    dicke=laenge*0.133;
    translate([0.05*laenge,0.2*laenge,0])rotate(28)
    union(){
        //The X
        rotate(52) prism2(2*laenge/3,3*dicke/4,3*dicke/4);
        rotate(123) prism2(2*laenge/3,3*dicke/4,3*dicke/4);
        
        //cube front
        translate([0,laenge/2,0]) prism2(laenge,dicke,dicke);
        translate([0,-laenge/2,0]) prism2(laenge,dicke,dicke);
        translate([laenge/2,0,0])rotate(90) prism2(laenge,dicke,dicke);
        translate([-laenge/2,0,0])rotate(90) prism2(laenge,dicke,dicke);
        //cube rear
        translate([-0.8*laenge,-0.3*laenge,0])rotate(90) prism2(laenge,dicke,dicke);
        translate([-0.3*laenge,-0.8*laenge,0]) prism2(laenge,dicke,dicke);
        //cube front to rear
        translate([-0.65*laenge,0.35*laenge,0])rotate(45) prism(0.42*laenge,1.4*dicke,dicke);
        translate([-0.65*laenge,-0.65*laenge,0])rotate(45) prism(0.42*laenge,1.4*dicke,dicke);
        translate([0.35*laenge, -0.65*laenge,0])rotate(-135) prism(0.42*laenge,1.4*dicke,dicke);
    }
}

//Tray for the SD Card Reader
module traySDCard(){
    cubebevel(schienen_a-2*schienen_d,schienen_l,2,rand);
    translate([0,-(schienen_l-2)/2,2]) cube([40,2,2],center=true);
    
    translate([0,22.5,0])difference(){
        union(){
            translate([18,2,1]) cylinder(5,3,3);
            translate([-18,2,1]) cylinder(5,3,3);
            translate([0,0,1+9/2]) cube([38,35,9],center=true);
        }
        translate([0,2,1+9/2]) cube([30,31,9],center=true);
        translate([0,1,9]) cube([38,33,2],center=true);
        translate([0,1,1+6]) cube([34,33,2],center=true);
        translate([0,2,1+6]) cube([38,5,2],center=true);
        translate([18,2,1]) cylinder(5,1.2,1.2);
        translate([-18,2,1]) cylinder(5,1.2,1.2);
    }
}

//Tray with the Thinker
module trayThinker(){
    cubebevel(schienen_a-2*schienen_d,schienen_l,2,rand);
    translate([0,-(schienen_l-2)/2,2]) cube([40,2,2],center=true);
    translate([0,-10,1])rotate(90)scale(0.3)import("thinker.stl", convexity = 5);
}

//Tray for Olimexino 32u4
module tray32u4(){
    cubebevel(cube_i,schienen_l,2,rand);
    translate([0,-(schienen_l-2)/2,2])cube([60,2,2],center=true);
    translate([-11,-42,1]) nupsi(5,6,2.4);
    translate([37,-42,1]) nupsi(5,6,2.4);
    translate([4,8.5,1]) nupsi(5,6,2.4);
    translate([32,8.5,1]) nupsi(5,6,2.4);
}

//Tray for the iCE40HX8K-EVB
module trayICE40(){
    cubebevel(cube_i,schienen_l,2,rand);
    translate([0,-(schienen_l-2)/2,2]) cube([60,2,2],center=true);
    translate([-30.5,-23,1]) nupsi(3,6,2.4);
    translate([13.5,-23,1]) nupsi(3,6,2.4);
    translate([-30.5,16,1]) nupsi(3,6,2.4);
    translate([13.5,16,1]) nupsi(3,6,2.4);
}

//Litte cylinder for screws
module nupsi(h,a,i){
    difference(){
        cylinder(h,a/2,a/2);
        cylinder(h,i/2,i/2);
    }
}

//rear part of cube
module rear(){
    translate([0,0,cube_a/2-boden_d/2])difference(){
        bodenplatte();
        holes();
    }
    translate([0,0,(cool_h+boden_h-boden_d)/2])
        rahmen(cube_a,cube_i,boden_h-boden_d,rand);
    translate([0,0,(cool_h-rip_d)/2])
        rahmen(cube_a-2*rip_l,cube_i,rip_d,0);
}

//Four holes for rear part 
module holes(){
    hole();
    mirror([0,1,0]) hole();
    mirror([0,1,0]) mirror([1,0,0]) hole();
    mirror([1,0,0]) hole();
}

//Single hole for the screw
module hole(){
    translate([schienen_a/2,(cube_i-2*schienen_d)/2,0])
        cylinder(boden_d,schienen_loch/2,schienen_loch/2,center=true); 
    translate([schienen_a/2,(cube_i-2*schienen_d)/2,boden_d/4])
        cylinder(boden_d/2,schienen_loch,schienen_loch,center=true);
}

//Die Bodenplatte für rear und front
module bodenplatte(){
    difference(){
        cubebevel(cube_a,cube_a,boden_d,rand);
        translate([0,22.5,(boden_d-1)/2])
            cube([52,47,1],center=true);
        beveltop(cube_a,cube_a,boden_d,rand);
    }
    for (a=[1.5:3:43.5])
        translate([0,a,boden_d/2-1])prism(50, 3, 1);
}

//Front part of Cube
module front(){
    translate([0,0,(-cube_a+boden_h+boden_d)/2])
        rahmen(cube_a,cube_i,boden_h-boden_d,rand);
    translate([0,0,(-cool_h+rip_d)/2])
        rahmen(cube_a-2*rip_l,cube_i,rip_d,0);
    difference(){
        rahmen(cube_a-2*rip_b,cube_i,cube_a-2*boden_d,rand);
        beveltop(cube_a-2*rip_b,cube_a-2*rip_b,cube_a-2*boden_d,rand);
    }
    //Bodenplatte
    translate([0,0,(-cube_a+boden_d)/2])mirror([0,0,1])
        difference(){
            union(){
                bodenplatte();
                translate([0,35.5,-4])cube([40,10,4],center=true);
            }
            //Schlitz für SD
            translate([0,34.5,-2]) cube([26,3,boden_d+4],center=true);
            translate([0,35.5,-3]) cube([36,6,6],center=true);
            //Logo
            translate([0,-37,2])mirror([1,0,0])mirror([0,0,1])logo(6);
        }
    //rahmen halter
    four();
}


//Rahmen halter vierfach
module four(){
    streben();
    mirror([0,1,0]) streben();
    mirror([0,1,0]) mirror([1,0,0]) streben();
    mirror([1,0,0]) streben();
}

//Einzelne Strebe
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

//Kühlrippen
module cool(){
    delta=(rip_d+rip_h)/2;
    //for(a=[1,3,5]){
    for(a=[1,3,5]){
    translate([0,0,delta+a*rip_h])mirror([0,0,1])rip();
    translate([0,0,-delta-a*rip_h])rip();
    }
    rip_center();
}

//Mittlere Kühlrippe
module rip_center(){
    rahmen(rip_a,rip_i,rip_d,rand);
}

//Kühlrippe
module rip(){
    rahmen(rip_a-2*rip_l,rip_i,rip_h);
     translate([0,0,(-rip_h+rip_d)/2])
        rahmen(rip_a,rip_i,rip_d,rand);
}

//Cubischer Torus
module rahmen(aussen,innen,hoehe,rand){
    difference(){
        cubebevel(aussen,aussen,hoehe,rand);
        cube([innen,innen,hoehe],center=true);  
    }
}

//Cube with Bevel
module cubebevel(x,y,hoehe,rand){
        difference(){
        cube([x,y,hoehe],center=true);
        translate([x/2,-y/2,-hoehe/2])
            rotate([0,270,0])
                bevel(hoehe,rand);
        translate([x/2,y/2,-hoehe/2])
            rotate([0,270,90])
                bevel(hoehe,rand);
        translate([-y/2,y/2,-hoehe/2])
            rotate([0,270,180])
                bevel(hoehe,rand);
        translate([-x/2,-y/2,-hoehe/2])
            rotate([0,270,270])
                bevel(hoehe,rand);   
    }
}

//Prism2
module prism2(l, w, h){
      polyhedron(//pt 0        1        2        3        4        5
              points=[[-(l+w)/2,-w/2,0], [(l+w)/2,-w/2,0], [l/2,0,h], [-l/2,0,h], [-(l+w)/2,w/2,0], [(l+w)/2,w/2,0]],
              faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
      );      
}
 
//Prism
 module prism(l=1, w=1, h=1){
      polyhedron(//pt 0        1        2        3        4        5
              points=[[-l/2,-w/2,0], [l/2,-w/2,0], [l/2,0,h], [-l/2,0,h], [-l/2,w/2,0], [l/2,w/2,0]],
              faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
              );      
      }
  
 //Top Bevel for cube (must be used with difference())
 module beveltop(x,y,z,rand){
    translate([x/2,-y/2,z/2])
            rotate([270,0,90])
                bevel(y,rand);
        translate([x/2,y/2,z/2])
            rotate([270,0,180])
                bevel(x,rand);
        translate([-x/2,y/2,z/2])
            rotate([270,0,270])
                bevel(y,rand);
        translate([-x/2,-y/2,z/2])
            rotate([270,0,0])
                bevel(x,rand);
}
     
//single bevel for one side
 module bevel(l=10, w=1){
      polyhedron(//pt 0        1        2        3        4        5
              points=[[0,0,0], [l,0,0], [l,0,w], [0,0,w], [0,w,0], [l,w,0]],
              faces=[[0,1,2,3],[5,4,3,2],[0,4,5,1],[0,3,4],[5,2,1]]
              );      
      }
  
