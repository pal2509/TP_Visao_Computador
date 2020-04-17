#include <stdio.h>
#include "vc.h"

int vc_perfil_columna(IVC *src, IVC *dst) {

    int x, y, soma, pos_src, pos_dst;

    for (x = 0; x < src->width; x++)
    {
        soma = 0;

        for (y = 0; y < src->height; y++)
        {
            pos_src = y * src->bytesperline + x;

            soma += src->data[pos_src];
        }

        pos_dst = (dst->height-1-(soma / y)) * dst->bytesperline +x;
        dst->data[pos_dst] = 1;
    }


    return 0;
}

int vc_perfil_rows(IVC *src, IVC *dst) {

    int x, y, soma, pos_src, pos_dst;

    for (y = 0; y < src->height; y++)
    {
        soma = 0;

         for (x = 0; x < src->width; x++)
        {
            pos_src = y * src->bytesperline + x;

            soma += src->data[pos_src];
        }

        pos_dst = y * dst->bytesperline +(soma/x);
        dst->data[pos_dst] = 1;
    }


    return 0;
}

int main(void)
{
	IVC* image;
	int i;
	image = vc_read_image("Imagem02-8281DH.ppm");
	if (image == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		getchar();
		return 0;
	}
	
	IVC* bounding = vc_image_new(image->width, image->height, 1, 255);

	BoundingBox(image, bounding);


	printf("press any key to exit...\n");
	getchar();

	/*
	    new_image = vc_image_new(image->width, 256, 1, 1);
	    vc_perfil_columna(image, new_image);
	    vc_write_image("output/perfil_columna.pbm", new_image);

	    new_image2 = vc_image_new(256, image->height, 1, 1);
	    vc_perfil_rows(image, new_image2);
	    vc_write_image("output/perfil_rows.pbm", new_image2);
	*/
	
	return 0;
