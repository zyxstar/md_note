/*******************************
修改menu.lst
  title XXXXOS (2.6.XX)
  root (hd0,X)
  kernel /boot/vmlinuz-2.6.XXX ro root=XXX quiet vga=0x318

在kernel那行最后添加vga=0x318或vga=792 （前面十六进制，后面十进制，参考下表）

vga=可设置的值可以用工具fbset（没有的话用sudo apt-get install fbset装）,/etc/fb.modes,或sudo hwinfo --framebuffer参考，主要还是以hwinfo为主（sudo apt-get install hwinfo）

下表列出一些常用值：

# FRAMEBUFFER RESOLUTION SETTINGS
# +----------------------------------------------------------------+
# |    640x480    800x600   1024x768 1280x1024 1280x800   1600x1200
# ----+-------------------------------------------------------------
# 256 | 0x301=769 0x303=771 0x305=773 0x307=775            0x31C=796
# 32K | 0x310=784 0x313=787 0x316=790 0x319=793 0x360＝864 0x31D=797
# 64K | 0x311=785 0x314=788 0x317=791 0x31A=794 0x361＝865 0x31E=798
# 16M | 0x312=786 0x315=789 0x318=792 0x31B=795 0x362＝866 0x31F=799
# +----------------------------------------------------------------+
*******************************/


#include <stdio.h>
#include <stdlib.h>

#include "fb_draw.h"

int main(void)
{
     int x, y;
     int ret;

     ret = fb_open();
     if (ret != 0) {
	  fprintf(stderr, "fb_open() error.\n");
	  exit(1);
     }

//     for (x = 0; x < xres(); x++)
//	  for (y = 0; y < yres(); y++) {
//	       fb_draw_point(x, y, 0x30);
//	  }

     for (x = 300; x < 400; x++)
	  for (y = 300; y < 400; y++) {
	       fb_draw_point(x, y, 0xaf);
	  }

     fb_close();
     return 0;
}
