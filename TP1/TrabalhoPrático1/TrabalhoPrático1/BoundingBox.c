#include "vc.h"


int BoundingBox(IVC* src, IVC* dst)
{
	IVC* gray = vc_image_new(src->width, src->height, 1, 255);
	IVC* bin = vc_image_new(src->width, src->height, 1, 255);
	IVC* close = vc_image_new(src->width, src->height, 1, 255);

	vc_rgb_to_gray(src, gray);
	//vc_gray_negative(gray);
	vc_write_image("gray.pgm", gray);
	vc_gray_to_binary_bersen(gray, bin, 25, 100);
	vc_binary_close(bin, close, 3);
	vc_write_image("bin.pgm", bin);
	vc_write_image("close.pgm", close);


	for (int y = 0; y < bin->height; y++)
	{
		for (int x = 0; x < bin->width; x++)
		{

		}
	}

}



