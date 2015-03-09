#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <linux/fb.h>

static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo var;
unsigned long bpp;

static unsigned long get_addr(int x, int y);
void set_square(char *fbp, int x1, int y1, int x2, int y2, int color);

int main(void)
{
     int fd;
     int ret;
     char *fbp;

     fd = open("/dev/fb0", O_RDWR);
     if (-1 == fd) {
	  perror("open");
	  exit(1);
     }

     /* get fb_var_screeninfo */
     ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
     if (-1 == ret) {
	  perror("ioctl(var)");
	  close(fd);
	  exit(1);
     }

     bpp = var.bits_per_pixel / 8;

     /* get fb_fix_screeninfo */
     ioctl(fd, FBIOGET_FSCREENINFO, &fix);
     if (-1 == ret) {
	  perror("ioctl(var)");
	  close(fd);
	  exit(1);
     }

     /* get framebuffer start address */
     fbp = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     if ((void *)-1 == fbp) {
	  perror("mmap");
	  close(fd);
	  exit(1);
     }

     /* set background */
     memset(fbp, 0, fix.smem_len);

     /* set a pixel */
     //memset(fbp + 9000, 0xff, bpp);

     memset(fbp, 0xff, fix.smem_len);
     set_square(fbp, 400, 300, 500, 400, 0);

     ret = munmap(fbp, fix.smem_len);
     if (-1 == ret) {
	  perror("munmap");
	  close(fd);
	  exit(1);
     }



     close(fd);

     return 0;
}

/* return offset */
static unsigned long get_addr(int x, int y)
{
     /* return (var.bits_per_pixel / 8) * (x + y * var.xres) */
     return bpp * (x + y * var.xres);
}

void set_square(char *fbp, int x1, int y1, int x2, int y2, int color)
{
     int i, j;
     unsigned long start, end;

     for (i = y1; i <= y2; i++) {
	  start = get_addr(x1, i);
	  end = get_addr(x2, i);
	  memset(fbp + start, color, end - start + bpp);
     }

}


