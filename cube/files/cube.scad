rip_i=94;
rip_a=100;
rip_h=5;
rip_l=2;
rip_d=1;
translate([0,0,-100])

front();
cool();
translate([0,0,50])
rear();

module rear(){
    //Bodenplatte
    translate([0,0,50-2])
    cube([100,100,4],center=true);
    //
    translate([0,0,50-33.5/2])
        rahmen(100,90,33.5);
    translate([0,0,+32/2])
        rahmen(96,90,1);

    
    
}


module front(){
    //Bodenplatte
    translate([0,0,-50+2])
    cube([100,100,4],center=true);
    
    //
    translate([0,0,-50+33.5/2])
        rahmen(100,90,33.5);
    translate([0,0,-32/2])
        rahmen(96,90,1);
    rahmen(94,90,96);
    
    //rahmen halter
    translate([45-1.25,45-1.25-2.5,0])
        cube([2.5,2.5,96],center=true);
    translate([45-1.25,-45+1.25+2.5,0])
        cube([2.5,2.5,96],center=true);
    translate([-45+1.25,45-1.25-2.5,0])
        cube([2.5,2.5,96],center=true);
    translate([-45+1.25,-45+1.25+2.5,0])
        cube([2.5,2.5,96],center=true);
    
    //STREBEN
    translate([-45+2.5,-30+2.5,0])
        cube([5,5,96],center=true);
    translate([-45+3.75,-25+1.25,0])
        cube([2.5,2.5,96],center=true);
    translate([-45+2.5,+30-2.5,0])
        cube([5,5,96],center=true);
    translate([-45+3.75,+25-1.25,0])
        cube([2.5,2.5,96],center=true);
    translate([+45-2.5,-30+2.5,0])
        cube([5,5,96],center=true);
    translate([+45-3.75,-25+1.25,0])
        cube([2.5,2.5,96],center=true);
    translate([+45-2.5,+30-2.5,0])
        cube([5,5,96],center=true);
    translate([+45-3.75,+25-1.25,0])
        cube([2.5,2.5,96],center=true);
}

module cool(){
    delta=(rip_d+rip_h)/2;
translate([0,0,delta+2*rip_h])mirror([0,0,1])rip();
translate([0,0,delta+rip_h])mirror([0,0,1])rip();
translate([0,0,delta])mirror([0,0,1])rip();
rip_center();
translate([0,0,-delta])rip();
translate([0,0,-delta-rip_h])rip();
translate([0,0,-delta-2*rip_h])rip();
}

module rip_center(){
    rahmen(rip_a,rip_i,rip_d);
}

module rip(){
    rahmen(rip_a-2*rip_l,rip_i,rip_h);
     translate([0,0,(-rip_h+rip_d)/2])
        rahmen(rip_a,rip_i,rip_d);
    
}
module rahmen(aussen,innen,hoehe){
    difference(){
        cube([aussen,aussen,hoehe],center=true);
    cube([innen,innen,hoehe],center=true);
    }
}