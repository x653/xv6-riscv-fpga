rip_i=94;
rip_a=100;
rip_h1=1.2;
rip_h2=4;
rip_d=2;

front();
translate([0,0,34])
cool();

module front(){
    translate([0,0,2])
    cube([100,100,4],center=true);
    rahmen(100,90,33);
    rahmen(96,90,34);
    rahmen(94,90,96);
    
    translate([45-1.25,45-1.25-2.5,48])
    cube([2.5,2.5,96],center=true);
    translate([45-1.25,-45+1.25+2.5,48])
    cube([2.5,2.5,96],center=true);
    translate([-45+1.25,45-1.25-2.5,48])
    cube([2.5,2.5,96],center=true);
    translate([-45+1.25,-45+1.25+2.5,48])
    cube([2.5,2.5,96],center=true);
    
}

module cool(){
    rip();
    translate([0,0,rip_h1+rip_h2])
    rip();
    translate([0,0,2*(rip_h1+rip_h2)])
    rip();
    translate([0,0,3*(rip_h1+rip_h2)])
    rip();
    translate([0,0,4*(rip_h1+rip_h2)])
    rip();
    translate([0,0,5*(rip_h1+rip_h2)])
    rip();
    translate([0,0,6*(rip_h1+rip_h2)])
    rahmen(rip_a,rip_i,rip_h1);
}


module rip(){
    rahmen(rip_a,rip_i,rip_h1);
    translate([0,0,rip_h1])
    rahmen(rip_i+rip_d,rip_i,rip_h2);
}
module rahmen(aussen,innen,hoehe){
    difference(){
        translate([0,0,hoehe/2])
        cube([aussen,aussen,hoehe],center=true);
    translate([0,0,hoehe/2])
        cube([innen,innen,hoehe+0.1],center=true);
    }
}