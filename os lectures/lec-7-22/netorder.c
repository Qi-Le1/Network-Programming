#include <stdio.h>
#include <arpa/inet.h>

void print_by_byte(unsigned int i){
	//If we use int instead of unsigned, then we'll add 1s to the front
	//instead of 0s :(
	unsigned int high_mask = 0xFF << 24; //Start with 0xFF000000
	printf("BYTES: ");
	for(int j=0; j<4; j++){
		/*
		 * Apply a mask of...
		 * FF000000
		 * 00FF0000
		 * 0000FF00
		 * 000000FF
		 * giving us 0s except for in the 8 bits we care about
		 */
		unsigned int postmask = i & (high_mask >> 8*j);
		//Have to shift accordingly or else we have a lot
		//of trailing 0s at the end after our 8 "interesting" bits
		//(which may also be 0s)
		printf(" %02x", postmask >> 24-(8*j));
	}
	printf("\n");
}

int main(){
	unsigned int x = 0xDEADBEEF;
	unsigned int y = htonl(x);
	printf("   Host order: %08x\nNetwork order: %x\n",x,y);
	#ifdef PRINT_BY_BYTE
	print_by_byte(x);
	print_by_byte(y);
	#endif
	return 0;
}
