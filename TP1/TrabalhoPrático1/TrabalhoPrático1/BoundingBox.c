#include "vc.h"


int BoundingBox(IVC* src, IVC* dst)
{
	IVC* gray = vc_image_new(src->width, src->height, 1, 255);
	IVC* bin = vc_image_new(src->width, src->height, 1, 255);
	IVC* close = vc_image_new(src->width, src->height, 1, 255);
	IVC* labeled = vc_image_new(src->width, src->height, 1, 255);

	vc_rgb_to_gray(src, gray);
	vc_write_image("gray.pgm", gray);
	vc_gray_to_binary_bersen(gray, bin, 25,240);
	vc_binary_close(bin, close, 3);
	vc_write_image("bin.pgm", bin);
	vc_write_image("close.pgm", close);
	
	int pos = 0;
	for (int y = 0; y < close->height; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			pos = y * close->width + x;
			close->data[pos] = 0;
		}

		for (int x = close->width - 100; x < close->width; x++)
		{
			pos = y * close->width + x;
			close->data[pos] = 0;
		}
	}
	pos = 0;
	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < close->width; x++)
		{
			pos = y * close->width + x;
			close->data[pos] = 0;
		}		
	}

	pos = 0;

	for (int y = close->height - 100; y < close->height; y++)
	{
		for (int x = 0; x < close->width; x++)
		{
			pos = y * close->width + x;
			close->data[pos] = 0;
		}
	}

	vc_write_image("closecut.pgm", close);

	int labels = 0;

	OVC *blobs= vc_binary_blob_labelling(close, labeled, &labels);

	vc_write_image("labeled.pgm", labeled);
	printf("Labels: %d\n", labels);

	vc_binary_blob_info(labeled, blobs, labels);


	for (int i = 0; i < labels; i++)
	{
		if (blobs[i].height != 0)
		{
			float r = blobs[i].width / blobs[i].height;
			if (r >= 4 && r <= 5 && blobs[i].area > 5000)
			{
				for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
				{
					pos = blobs[i].y * src->bytesperline + x * src->channels;
					src->data[pos] = 0;
					src->data[pos + 1] = 255;
					src->data[pos + 1] = 255;
				}
				for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
				{
					pos = (blobs[i].y + blobs[i].height) * src->bytesperline + x * src->channels;
					src->data[pos] = 0;
					src->data[pos + 1] = 255;
					src->data[pos + 1] = 255;
				}
				for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
				{
					pos = y * src->bytesperline + blobs[i].x * src->channels;
					src->data[pos] = 0;
					src->data[pos + 1] = 255;
					src->data[pos + 1] = 255;
				}
				for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
				{
					pos = y * src->bytesperline + (blobs[i].x + blobs[i].width) * src->channels;
					src->data[pos] = 0;
					src->data[pos + 1] = 255;
					src->data[pos + 1] = 255;
				}

				printf("_____________________________\n");
				printf("Label: %d\n", blobs[i].label);
				printf("Width: %d\n", blobs[i].width);
				printf("Height: %d\n", blobs[i].height);
				printf("Area: %d\n", blobs[i].area);
				printf("X: %d\n", blobs[i].x);
				printf("Y: %d\n", blobs[i].y);

				//IVC* blob = vc_image_new(blobs[i].width, blobs[i].height, 1, 255);
				//int bwidth = blobs[i].width;
				//int bheight = blobs[i].height;

				//int pos = 0;
				//int posb = 0;
				//int xb = 0, yb = 0;
				//for (int y = blobs[i].y; y < bheight + blobs[i].y; y++)
				//{
				//	for (int x = blobs[i].x; x < bwidth + blobs[i].x; x++)
				//	{
				//		pos = y * close->width + x;
				//		posb = yb * bwidth + xb;
				//		blob->data[posb] = gray->data[pos];
				//		xb++;
				//	}
				//	yb++;
				//	xb = 0;
				//}
				//vc_write_image("mgray.pgm", blob);
				//IVC* mabin = vc_image_new(blob->width, blob->height, 1, 255);
				//IVC* blobgd = vc_image_new(blob->width, blob->height, 1, 255);

				//vc_gray_negative(blob);
				////vc_grayscale_dilate(blob,blobgd,3);
				////vc_write_image("mgraydilate.pgm", blobgd);
				//vc_gray_to_binary_bersen(blob, mabin, 3,200);
				//vc_write_image("mb.pgm", mabin);
				//
				//IVC* mopen = vc_image_new(blob->width, blob->height, 1, 255);
				//IVC* merode = vc_image_new(blob->width, blob->height, 1, 255);

				//vc_binary_open(mabin, mopen, 3);
				//vc_write_image("mopen.pgm", mopen);
			
				//pos = 0;
				//for (int y = 0; y < mopen->height; y++)
				//{
				//	for (int x = 0; x < 10; x++)
				//	{
				//		pos = y * mopen->width + x;
				//		mopen->data[pos] = 0;
				//	}

				//	for (int x = mopen->width - 10; x < mopen->width; x++)
				//	{
				//		pos = y * mopen->width + x;
				//		mopen->data[pos] = 0;
				//	}
				//}
				//pos = 0;
				//for (int y = 0; y < 5; y++)
				//{
				//	for (int x = 0; x < mopen->width; x++)
				//	{
				//		pos = y * mopen->width + x;
				//		mopen->data[pos] = 0;
				//	}
				//}
				//pos = 0;
				//for (int y = mopen->height - 5; y < mopen->height; y++)
				//{
				//	for (int x = 0; x < mopen->width; x++)
				//	{
				//		pos = y * mopen->width + x;
				//		mopen->data[pos] = 0;
				//	}
				//}
				//vc_write_image("mopencut.pgm", mopen);
				//IVC* mopen2 = vc_image_new(mopen->width, mopen->height, 1, 255);
				//vc_binary_open(mopen, mopen2, 3);
				//vc_write_image("mopen2.pgm", mopen2);

				//IVC* letraslabeled = vc_image_new(mopen2->width, mopen2->height, 1, 255);
				//int nletras = 0;

				//OVC* letras = vc_binary_blob_labelling(mopen2, letraslabeled, &nletras);
				//vc_write_image("letraslabel.pgm", letraslabeled);

				//vc_binary_blob_info(letraslabeled, letras, nletras);
				//printf("NLetras: %d\n", nletras);

				//Filtrar o array de letras
				
			}
		}




		/*printf("Label: %d\n", blobs[i].label);
		printf("Width: %d\n", blobs[i].width);
		printf("Height: %d\n", blobs[i].height);
		printf("Area: %d\n", blobs[i].area);*/
	}



}



