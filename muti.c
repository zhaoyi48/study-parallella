#include <stdio.h>
#include <stdlib.h>
#include <host_bsp.h>
struct pixel
{
  int b, g, r;
  int core;
};
#define N sizeof(struct pixel)
int main(int argc, char **argv) {
  int test, test2;
  struct pixel **a;
  a = (struct pixel**)malloc(1920 * sizeof(struct pixel*));
  for (int i = 0; i < 1920; i++) {
    a[i] = (struct pixel*)malloc(1920 * sizeof(struct pixel));
  }
  int count = 0;
  int h, w;
  h = w = 0;
  FILE *file;
  file = fopen("../test.txt", "r+");
  if (feof(file)) {
    printf("no data \n");
  }
  else {
    rewind(file);
    fscanf(file, "(%d x %d)\n", &h, &w);
    printf("%d---%d", h, w);
    double all = h*w;
    if (h > w) {
      for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
          fscanf(file, "(%d,%d,%d)\n", &a[i][j].b, &a[i][j].g, &a[i][j].r);
          //printf("(%d,%d,%d)\n", a[i][j].b, a[i][j].g, a[i][j].r);
          count++;
          printf("Loading:%.2f \n", (count / all) * 100);
        }
      }
    }
    else {
      for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
          fscanf(file, "(%d,%d,%d)\n", &a[i][j].b, &a[i][j].g, &a[i][j].r);
          //printf("(%d,%d,%d)\n", a[i][j].b, a[i][j].g, a[i][j].r);
          count++;
          printf("Loading:%.2f \n", (count / all) * 100);
        }
      }
    }
  }
  printf("over %d \n", count);

  bsp_init("ecore_program.srec", argc, argv);
  bsp_begin(16);
  int n = bsp_nprocs();
  int tagsize = 4 * sizeof(int);
  ebsp_set_tagsize(&tagsize);

  struct pixel load;
  // row=560 col=790

  for (int s = 0; s < n; ++s) {
    int tag = 1;
    for (int i = 0; i<2; i++) {
      //printf("%d  %d--%d--%d\n", s,a[s][i].b,a[s][i].g,a[s][i].r);
      for (int j = 0; j < 2; j++){
        load = a[0 + i][0 + s];
        load.core = s;
        ebsp_send_down(s, &tag, &load, sizeof(struct pixel));
        tag++;
      }
    }
  }

  ebsp_spmd();

  int packets = 0; 
  int accum_bytes = 0; 
  ebsp_qsize(&packets, &accum_bytes); 
  int payload_in = 0; 
  int payload_size = 0; 
  int tag_in = 0; 
  for (int i = 0; i < packets; ++i) { 
    ebsp_get_tag(&payload_size, &tag_in); 
    ebsp_move(&payload_in, sizeof(struct pixel));
    printf("payload: %i, tag: %i", payload_in, tag_in); 
  }
  bsp_end();

  return 0;
}