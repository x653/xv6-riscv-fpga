#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define MAX 0xfff
#define pair unsigned short
#define number int
#define float float
#define MASK_INDEX 0x0fff
#define MASK_TAG 0xf000
#define MASK_PAIR 0xffff
#define TAG_PAIR 0x4000
#define TAG_INT 0x1000
#define TAG_FLOAT 0x8000
#define TAG_SYM 0x2000
#define TAG_MARK 0xf000
/*
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#define MAX 0xfff

#define pair unsigned int
#define number long long
#define float double
#define MASK_INDEX 0x00ffffff
#define MASK_TAG 0xff000000
#define MASK_PAIR 0xffffffff
#define TAG_PAIR 0x04000000
#define TAG_INT 0x01000000
#define TAG_FLOAT 0x08000000
#define TAG_SYM 0x02000000
#define TAG_MARK 0xff000000
*/

pair apply(pair,pair);
pair eval(pair,pair);

//Memory
pair stack[MAX];	//the-stack
pair cars1[MAX];	//the-cars
pair cdrs1[MAX];	//the-cdrs
pair cars2[MAX];	//doubled for garbage collector
pair cdrs2[MAX];

pair* cars;			//link to working heap
pair* cdrs;
pair* ncars;		//link to free heap (for garbage collector)
pair* ncdrs;

int mfree;				//pointer to next free pair in heap: working memory
int nfree;				//pointer to next free pair in heap: copy memory (garbage collector)
int sfree=0;			//pointer to next free pair in stack

//global pairs
pair const nil=0;	//NULL
pair symbols;		//list of symbols
pair global_env;	//global environment

//elementary operation on pairs
pair car(pair p){return cars[p & MASK_INDEX];}
pair cdr(pair p){return cdrs[p & MASK_INDEX];}
pair ncar(pair p){return ncars[p & MASK_INDEX];}
pair ncdr(pair p){return ncdrs[p & MASK_INDEX];}

void setcar(pair p, pair x){cars[p & MASK_INDEX]=x;}
void setcdr(pair p, pair x){cdrs[p & MASK_INDEX]=x;}
void setncar(pair p, pair x){ncars[p & MASK_INDEX]=x;}
void setncdr(pair p, pair x){ncdrs[p & MASK_INDEX]=x;}

number getint(pair p){return (number)(car(p))<<16 | (number)cdr(p);}
float getfloat(pair p){number n = getint(p); return *(float*)(&n);}
char* getstr(pair p){return (char*)getint(p);}

//constructor for pairs
void checkmem(){
	if (MAX==mfree){
		printf("out of memory:\n");
		exit(1);
	}
}
pair cons(pair car, pair cdr){
	checkmem();
	pair p = TAG_PAIR | mfree++;
    setcar(p,car);
	setcdr(p,cdr);
    return p;
}
pair consint(number n){
	pair p=cons((pair)(n>>16),(pair)(n&(MASK_PAIR)));
    return (p&MASK_INDEX) | TAG_INT;
}
pair consfloat(float x){
    checkmem();
	pair p = consint(*(number*)(&x));
    return (p&MASK_INDEX) | TAG_FLOAT;
}
pair conssym(char* s){
	char* ss=malloc(sizeof(char)*(strlen(s)+1));
	strcpy(ss,s);
	pair p=consint((number)ss);
    return (p&MASK_INDEX) | TAG_SYM;
}
//test type of pair, these function return 0 (false) or 1 (true)
int nullq(pair l){return l==nil;}
int numq(pair p){return (p & TAG_INT);}
int floatq(pair p){return (p & TAG_FLOAT);}
int symq(pair p){return (p & TAG_SYM);}
int pairq(pair p){return (p & TAG_PAIR);}
int eqq(pair a, pair b){
	if (a==b) return 1;
	if (floatq(a) && floatq(b)) return getfloat(a)==getfloat(b);
	if (numq(a) && numq(b)) return getint(a)==getint(b);
	return 0;
}
int eq(pair l){
	if (floatq(car(l))) return (getfloat(car(l))==getfloat(car(cdr(l))));
	return (getint(car(l))==getint(car(cdr(l))));
}
int lt(pair l){
	if (floatq(car(l))) return (getfloat(car(l))<getfloat(car(cdr(l))));
	return (getint(car(l))<getint(car(cdr(l))));
}
int gt(pair l){
	if (floatq(car(l))) return (getfloat(car(l))>getfloat(car(cdr(l))));
	return (getint(car(l))>getint(car(cdr(l))));
}

//list operation
void push(pair s){if (sfree==MAX) printf("error stackoverflow"); stack[sfree++]=s;}
pair pop(){return stack[--sfree];}
pair top(){return stack[sfree-1];}
pair append(pair q, pair x){
	if (q==nil) return cons(x,nil);
	push(q);
	while(cdr(q)!=nil) q=cdr(q);
	setcdr(q,cons(x,nil));
	return pop();
}

//get symbol from list of constructed symbols
pair getsym(char* s){
	pair sym = symbols;
	while (sym!=nil){
		pair c = car(sym);
		char* ss = getstr(c);
		if (strcmp(s,ss)==0) return c;
		sym=cdr(sym);
	}
    pair p = conssym(s);
	symbols=append(symbols,p);
    return p;
}

//garbage collector
void mark(pair p,pair n){setcar(p,TAG_MARK); setcdr(p,n);}
int ismark(pair p){return (car(p)==TAG_MARK);}

pair copy(pair x){
		if(ismark(x)) return cdr(x);
		pair np=(MASK_TAG & x) | nfree++;
		setncar(np,car(x));
		setncdr(np,cdr(x));
		mark(x,np);
		return np;
}

void garbage(){
	push(symbols);
	push(global_env);
	if(cars==cars1) ncars=cars2;
	else ncars=cars1;
	if(cdrs==cdrs1) ncdrs=cdrs2;
	else ncdrs=cdrs1;
	for (int i=0;i<MAX;i++){
		ncars[i]=0;
		ncdrs[i]=0;
	}
	nfree=1;
	int nscan=1;
	for (int i=0;i<sfree;i++){
		pair x=stack[i];
		if(pairq(x)) stack[i] = copy(x);
	}	
	while(nscan<nfree){
		pair xcar=ncar(nscan);
		pair xcdr=ncdr(nscan);
		if (pairq(xcar)) setncar(nscan,copy(xcar));
		if (pairq(xcdr)) setncdr(nscan,copy(xcdr));
		nscan++;
	}
	nscan--;
	while (nscan>0){
		pair xcar=ncar(nscan);
		pair xcdr=ncdr(nscan);
		if((xcar!=nil) && (!pairq(xcar))) setncar(nscan,copy(xcar));
		if((xcdr!=nil) && (!pairq(xcdr))) setncdr(nscan,copy(xcdr));
		nscan--;	
	}
	for (int i=0;i<sfree;i++){
		pair x=stack[i];
		if((x!=nil) && (!pairq(x))) stack[i]=copy(x);
	}
	cars=ncars;
	cdrs=ncdrs;	
	mfree=nfree;
	global_env=pop();
	symbols=pop();
}

//internal procedures
pair f_add(pair l){
	if (l==nil) return consint(0);
	pair a=car(l);
	pair b=f_add(cdr(l));
	if (floatq(a) && floatq(b)) return consfloat(getfloat(a)+getfloat(b));
	if (floatq(a) && !floatq(b)) return consfloat(getfloat(a)+(float)getint(b));
	if (!floatq(a) && floatq(b)) return consfloat((float)getint(a)+getfloat(b));
	return consint(getint(a)+getint(b));
}

pair f_sub(pair l){
	if (l==nil) return consint(0);
	pair a=car(l);
	if (cdr(l)==nil){
		if (floatq(a)) return consfloat(-getfloat(a));
		else return consint(-getint(car(l)));	
	}
	pair b=f_add(cdr(l));
	if (floatq(a) && floatq(b)) return consfloat(getfloat(a)-getfloat(b));
	if (floatq(a) && !floatq(b)) return consfloat(getfloat(a)-(float)getint(b));
	if (!floatq(a) && floatq(b)) return consfloat((float)getint(a)-getfloat(b));
	return consint(getint(a)-getint(b));
}

pair f_mul(pair l){
	if (l==nil) return consint(1);
	pair a=car(l);
	pair b=f_mul(cdr(l));
	if (floatq(a) && floatq(b)) return consfloat(getfloat(a)*getfloat(b));
	if (floatq(a) && !floatq(b)) return consfloat(getfloat(a)*(float)getint(b));
	if (!floatq(a) && floatq(b)) return consfloat((float)getint(a)*getfloat(b));
	return consint(getint(a)*getint(b));
}

pair f_div(pair l){
	if (l==nil) return consint(1);
	pair a=car(l);
	pair b=f_mul(cdr(l));
	if (floatq(a) && floatq(b)) return consfloat(getfloat(a)/getfloat(b));
	if (floatq(a) && !floatq(b)) return consfloat(getfloat(a)/(float)getint(b));
	if (!floatq(a) && floatq(b)) return consfloat((float)getint(a)/getfloat(b));
	return consint(getint(a)/getint(b));
}

pair and(pair l){
	if (l==nil) return getsym("#t");
	if (car(l)==getsym("#f")) return getsym("#f");
	return and(cdr(l));
}

pair or(pair l){
	if (l==nil) return getsym("#f");
	if (car(l)==getsym("#t")) return getsym("#t");
	return or(cdr(l));
}

pair not(pair l){
	if (car(l)==getsym("#t")) return getsym("#f");
	return getsym("#t");
}

//print pair according to type
void print(pair p){
	if (p==nil) printf("()");
	else if (floatq(p)){
		float a = getfloat(p);
		printf("%d.",(number)a);
		a=a-(number)a;
		for (int i=0;i<6;i++){
			a=a*10;
			printf("%d",(number)a);
			a=a-(number)a;
		}
	}

	else if (numq(p)) printf("%d",getint(p));
	else if (symq(p)) printf("%s",getstr(p));
	else if ((pairq(p)) && (eqq(car(p),getsym("procedure")))) printf("(procedure)");
	else if (pairq(p)){
		printf("(");
		print(car(p));
		p=cdr(p);
		while (pairq(p)){
			printf(" ");
			print(car(p));
			p=cdr(p);
		}
		if (p!=nil){
			printf(" . ");
			print(p);
		}
		printf(")");
	}
}

//lookup
pair lookup(pair var,pair env){
    if (env==nil){
		printf("lookup error");
		print(var);
		exit(1);
    }
	pair frame = car(env);
    pair vars = car(frame);
    pair vals = cdr(frame);
    while (vars!=nil){
        if ((symq(var)) && (eqq(var,car(vars)))) return car(vals);
        vars = cdr(vars);
        vals = cdr(vals);
    }
	return lookup(var,cdr(env));
}
//set
pair set(pair var,pair val,pair env){
    if (env==nil){
		printf("error");
		exit(1);
    }
    pair frame = car(env);
    pair vars = car(frame);
    pair vals = cdr(frame);
    while (vars!=nil){
        if (eqq(var,car(vars))){
            setcar(vals,val);
            return getsym("ok");
		}
        vars = cdr(vars);
        vals = cdr(vals);
    }
	return set(var,val,cdr(env));
}
//define
pair define(pair var,pair val,pair env){
    pair frame = car(env);
    pair vars = car(frame);
    pair vals = cdr(frame);
    pair xvars = vars;
    pair xvals = vals;
    while (xvars!=nil){
        if (eqq(var,car(xvars))){
            setcar(xvals,val);
            return getsym("ok");
		}
        xvars = cdr(xvars);
        xvals = cdr(xvals);
    }
	vars = cons(var,vars);
    vals = cons(val,vals);
    setcar(env,cons(vars,vals));
    return getsym("ok");
}

//eval
pair eval(pair exp,pair env){
	push(exp);push(env);
	if(MAX-mfree<500){
			garbage();
	}
	env=pop();
	exp=pop();
	//self evaluating (boolean, int,float)
    if (exp==nil) return nil;
    else if (numq(exp)) return exp;
    else if (floatq(exp)) return exp;
    //variable (str as symbol)
    else if (symq(exp)) return lookup(exp,env);
	else if (pairq(exp)){
		pair sym = car(exp);
		//quotiation
		if (eqq(sym,getsym("quote"))) return car(cdr(exp));
    	//assignment
        if (eqq(sym,getsym("set!"))){
			pair var = car(cdr(exp));
			push(env);
			push(var);
            pair val = eval(car(cdr(cdr(exp))),env);
            var=pop();
			env=pop();
			return set(var,val,env);
		}
        //definition
        if (eqq(sym,getsym("define"))){
            pair var = car(cdr(exp));
            if (symq(var)){
				push(var);
				push(env);
                pair val = eval(car(cdr(cdr(exp))),env);
				env=pop();
				var=pop();
				return define(var,val,env);
			}
            if (pairq(var)){
                pair f = car(var);
                pair par = cdr(var);
                pair body = cdr(cdr(exp));
                pair p = cons(getsym("lambda"),cons(par,body));
				push(f);
				push(env);
				pair proc = eval(p,env);
				env=pop();
				f=pop();
				return define(f,proc,env);
			}
		}
        //if
        if (eqq(sym,getsym("if"))){
			push(exp);
			push(env);	
            pair pred = eval(car(cdr(exp)),env);
			env=pop();
			exp=pop();
			if (pred==getsym("#t")){
                pair if_con = car(cdr(cdr(exp)));
            	return eval(if_con,env);
			}else{
                pair if_alt = car(cdr(cdr(cdr(exp))));
            	return eval(if_alt,env);
			}
		}
        //lambda
        if (eqq(sym,getsym("lambda"))){
            return cons(getsym("procedure"),cons(car(cdr(exp)),cons(cdr(cdr(exp)),cons(env,nil))));
		}        
		//begin
        if (eqq(sym,getsym("begin"))){
            pair sequence = cdr(exp);
            while (cdr(sequence)){
				push(sequence);
				push(env);
                eval(car(sequence),env);
				env=pop();
				sequence=pop();
				sequence = cdr(sequence);
			}
            return eval(car(sequence),env);
		}
        //condition
        if (eqq(sym,getsym("cond"))){
            pair conds = cdr(exp);
            while (conds!=nil){
                pair cond = car(conds);
				push(conds);
				push(cond);
				push(env);
                if (eval(car(cond),env)==getsym("#t")){
					env=pop();
					cond=pop();
					pop();
					return eval(car(cdr(cond)),env);
				}
				env=pop();
				cond=pop();
				conds=pop();
				conds = cdr(conds);
			}
            return getsym("#f");
		}
        //application
		push(cdr(exp));
		push(env);
        pair op = eval(sym,env);
		env=pop();
		exp=pop();
		push(op);
        pair args = nil;
        while (exp!=nil){
			push(exp);
			push(env);
			push(args);
			pair r=eval(car(exp),env);
			args=pop();
			env=pop();
			exp=pop();
			args=append(args,r);
			exp=cdr(exp);
		}
		op=pop();
        return apply(op,args);
	}
	return nil;
}

//apply
pair apply(pair proc,pair args){
    if (proc==getsym("+")) return f_add(args);
    if (proc==getsym("-")) return f_sub(args);
    if (proc==getsym("*")) return f_mul(args);
    if (proc==getsym("/")) return f_div(args);
	if (proc==getsym("=")) return (eq(args)? getsym("#t"): getsym("#f"));
	if (proc==getsym("<")) return (lt(args)? getsym("#t"): getsym("#f"));
	if (proc==getsym(">")) return (gt(args)? getsym("#t"): getsym("#f"));
	if (proc==getsym("and")) return and(args);
    if (proc==getsym("or")) return or(args);
    if (proc==getsym("not")) return not(args);
    if (proc==getsym("null?")) return (nullq(car(args))? getsym("#t"): getsym("#f"));
    if (proc==getsym("pair?")) return (pairq(car(args))? getsym("#t"): getsym("#f"));
	if (proc==getsym("eq?")) return (eqq(car(args),car(cdr(args)))? getsym("#t"): getsym("#f"));
	if (proc==getsym("cons")) return cons(car(args),car(cdr(args)));
	if (proc==getsym("car")) return car(car(args));
	if (proc==getsym("cdr")) return cdr(car(args));
	if (proc==getsym("setcar!")) {setcar(car(args),car(cdr(args)));return getsym("ok");}
	if (proc==getsym("setcdr!")) {setcdr(car(args),car(cdr(args)));return getsym("ok");}
	if (proc==getsym("eval")) return eval(car(args),global_env);
	if (proc==getsym("display"))	{
				print(car(args));
				return getsym("");
				}
    if (car(proc) == getsym("procedure")){
        pair param = car(cdr(proc));
        if (symq(param)){
            param = cons(param,nil);
            args = cons(args,nil);
		}
        pair body = car(cdr(cdr(proc)));
        pair frame = cons(param,args);
        pair env =car(cdr(cdr(cdr(proc))));
        pair newenv = cons(frame,env);
        while (cdr(body)){
			push(body);
			push(newenv);
            eval(car(body),newenv);
			newenv=pop();
			body=pop();
            body = cdr(body);
		}
        return eval(car(body),newenv);
	}
	printf("error: apply");
	exit(1);
}

pair token=nil;
float ten=1.0;
char* s;
//parse input buffer and push parse tree to stack
void repl(char* expr){
	char c;
	while((c=*expr)!='\0'){
			if ((c=='\'')||(c=='(')||(c==')')||(c==' ')||(c=='\t')||(c=='\n')){
				//terminate token
				if (numq(token)) push(append(pop(),token));
				else if (floatq(token)) push(append(pop(),token));
				else if (symq(token)){*expr='\0';push(append(pop(),getsym(s)));}
				token=nil;
				if (c=='('){
					//new list
					push(nil);
				}else if (c==')'){
					//end list
					while (car(top())==getsym("\'")){
						setcar(top(),getsym("quote"));
						pair last = pop();
						push(append(pop(),last));
					}
					pair last = pop();
					push(append(pop(),last));
				}else if ((c==' ')||(c=='\t')||(c=='\n')){
					//separator
					while (car(top())==getsym("\'")){
						setcar(top(),getsym("quote"));
						pair last = pop();
						push(append(pop(),last));
					}
					//evaluate
					if ((sfree==1) && (top()!=nil)){
						print(eval(car(top()),global_env));
						printf("\nlisp> ");
						push(cdr(pop()));
					}
				}else if (c=='\''){
					//start quote
					push(nil);
					push(append(pop(),getsym("\'")));
				}
			}else {
				if ((c>='0')&&(c<='9')){
					//number
					if (token==nil) token = consint(c-'0');
					else if (numq(token)) token=consint(getint(token)*10+c-'0');
					else if (floatq(token)){
						token=consfloat(getfloat(token)+(float)(c-'0')/ten);
						ten=ten*10.0;
					}
				}else if (c=='.'){
					//float
					if (numq(token)){
						token=consfloat(getint(token));
						ten=10.0;
					}
				}else if (token==nil){
					//new symbol
					s=expr;
					token=TAG_SYM;
				}
			}
			expr++;
	}
}

//initialise global environment
void init(){
	define(getsym("nil"),nil,global_env);
	define(getsym("#f"),getsym("#f"),global_env);
	define(getsym("#t"),getsym("#t"),global_env);
	define(getsym("and"),getsym("and"),global_env);
	define(getsym("or"),getsym("or"),global_env);
	define(getsym("not"),getsym("not"),global_env);
	define(getsym("+"),getsym("+"),global_env);
	define(getsym("-"),getsym("-"),global_env);
	define(getsym("*"),getsym("*"),global_env);
	define(getsym("/"),getsym("/"),global_env);
	define(getsym("="),getsym("="),global_env);
	define(getsym(">"),getsym(">"),global_env);
	define(getsym("<"),getsym("<"),global_env);
	define(getsym("else"),getsym("#t"),global_env);
	define(getsym("eq?"),getsym("eq?"),global_env);
	define(getsym("pair?"),getsym("pair?"),global_env);
	define(getsym("null?"),getsym("null?"),global_env);
	define(getsym("cons"),getsym("cons"),global_env);
	define(getsym("car"),getsym("car"),global_env);
	define(getsym("cdr"),getsym("cdr"),global_env);
	define(getsym("setcar!"),getsym("setcar!"),global_env);
	define(getsym("setcdr!"),getsym("setcdr!"),global_env);
	define(getsym("eval"),getsym("eval"),global_env);
	define(getsym("display"),getsym("display"),global_env);
}

//main: initialise read from stdin and start read evaluate print loop
void main(){
	printf("lisp v6\nsizeof pair: %d\nsize of number: %d\nsize of float: %d\nsize of (char*): %d\n",sizeof(pair),sizeof(number),sizeof(float),sizeof(char*));
//	setbuf(stdout, NULL);
	cars=cars1;
	cdrs=cdrs1;
	cars[0]=0;
	cdrs[0]=0;
	mfree=1;
	symbols=nil;//symbols
	global_env = cons(cons(nil,nil),nil);//global_env
	char buf[512];
	int n;
	init();	
	printf("lisp> ");
	push(nil);
	while((n=read(0,&buf,511))>0){
		buf[n]='\0';
		repl(buf);
	}
	printf("bye\n");
	exit(0);
}
