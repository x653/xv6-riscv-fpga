//Sagar: putc writes to led
void putc(char c)
{
	volatile char *p = (void*)0x10002000;
	*p = c;
}

//Sagar: getc reads from but
char getc()
{
	volatile char *p = (void*)0x10002000;
	return *p;
}

//Sagar: main loop reads from but and writes to led
int main(){
	while(1){
		putc(getc());
	}
	return 0;
}
