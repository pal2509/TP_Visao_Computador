#include <stdio.h>
#include "vc.h"



int main(void)
{
	IVC* image;
	int i;
	image = vc_read_image("Imagem03-6468QN.ppm");
	if (image == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}
	
	IVC* bounding = vc_image_new(image->width, image->height, 1, 255);

	BoundingBox(image, bounding);

	vc_write_image("M3.ppm", image);
	printf("press any key to exit...\n");
	getchar();

	return 0;
}