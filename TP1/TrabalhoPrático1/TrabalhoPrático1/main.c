#include <stdio.h>
#include "vc.h"



int main(void)
{
	IVC* image;
	int i;
	image = vc_read_image("Imagem01-8449FS.ppm");
	if (image == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}
	
	IVC* bounding = vc_image_new(image->width, image->height, 1, 255);

	BoundingBox(image, bounding);
	vc_write_image("M1.ppm", image);
	system("cmd /c start FilterGear M1.ppm");
	image = vc_read_image("Imagem02-8281DH.ppm");
	BoundingBox(image, bounding);
	vc_write_image("M2.ppm", image);
	system("FilterGear M2.ppm");
	image = vc_read_image("Imagem03-6468QN.ppm");
	BoundingBox(image, bounding);
	vc_write_image("M3.ppm", image);
	system("FilterGear M3.ppm");
	image = vc_read_image("Imagem04-11GF03.ppm");
	BoundingBox(image, bounding);
	vc_write_image("M4.ppm", image);
	system("FilterGear M4.ppm");

	printf("press any key to exit...\n");
	getchar();

	return 0;
}