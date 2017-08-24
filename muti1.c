#include "e_bsp.h"

struct pixel
{
	int b, g, r;
	int core;
};

int main() {
	bsp_begin();
	int s = bsp_pid();
	int packets = 0; 
	int accum_bytes = 0; 
	bsp_qsize(&packets, &accum_bytes);

	struct pixel load_in[4]; 
	int load_size = sizeof(struct pixel); 
	int tag_in = 0;

	for (int i = 0; i < packets; ++i) { 
		bsp_get_tag(&load_size, &tag_in); 
		bsp_move(&load_in[i], sizeof(struct pixel)); 
		ebsp_message("load: %i--%i--%i, tag: %i", load_in[i].b, load_in[i].g, load_in[i].r, tag_in); 
	}
	bsp_sync();
	ebsp_message("test %i", s); 
	bsp_end();
	return 0;
}