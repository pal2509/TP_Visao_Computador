#include "vc.h"
#include <string.h>

int BoundingBox(IVC* src, IVC* dst)
{
	IVC* gray = vc_image_new(src->width, src->height, 1, 255);//Imagem em cinza
	IVC* bin = vc_image_new(src->width, src->height, 1, 255);//Imagen em binario
	IVC* close = vc_image_new(src->width, src->height, 1, 255);//Imagem em bin depois de um close
	IVC* labeled = vc_image_new(src->width, src->height, 1, 255);//Imagem labeled

	vc_rgb_to_gray(src, gray);//Converter a imagem original para Escla de cinzentos
	//vc_write_image("gray.pgm", gray);
	vc_gray_to_binary_bernsen(gray, bin, 25, 240);//Converter para binário usando o metodo de bernsen
	vc_binary_close(bin, close, 3);//Fazer um fecho da imagem
	//vc_write_image("bin.pgm", bin);
	//vc_write_image("close.pgm", close);	
	//Pôr 100 pixeis a toda á volta da imagem em preto
	int pos = 0;//Posição na imagem
	//Faixas verticais
	for (int y = 0; y < close->height; y++)//Percorrer as linhas todas de cima para baixo
	{
		//Faixa da esquerda
		for (int x = 0; x < 100; x++)//Percorrer as primeiras 100 colunas da esquerda para a direita
		{
			pos = y * close->width + x;//Calculo da posição
			close->data[pos] = 0;//Por a preto
		}
		//Faixa da direita
		for (int x = close->width - 100; x < close->width; x++)//Percorrer as ultimas 100 colunas da esquerda para a direita
		{
			pos = y * close->width + x;//Calculo da posição
			close->data[pos] = 0;//Por a preto
		}
	}
	//Barra de 100 pixeis superior 
	pos = 0;//Iniciar na posição 0
	for (int y = 0; y < 100; y++)//Percorrer as primeiras 100 linhas
	{
		for (int x = 0; x < close->width; x++)//Percorrer todas as colunas da esquerda para a direita
		{
			pos = y * close->width + x;//Calculo da posição
			close->data[pos] = 0;//Por a preto
		}
	}
	//Barra de 100 pixeis superior 
	pos = 0;//Iniciar na posição 0
	for (int y = close->height - 100; y < close->height; y++)//Percorrer as ultimas 100 linhas
	{
		for (int x = 0; x < close->width; x++)//Percorrer todas as colunas da esquerda para a direita
		{
			pos = y * close->width + x;//Calculo da posição
			close->data[pos] = 0;//Por a preto
		}
	}
	//vc_write_image("closecut.pgm", close);
	int labels = 0;//Numero de labels
	OVC* blobs = vc_binary_blob_labelling(close, labeled, &labels);//Fazer o labelling da imagem
	//vc_write_image("labeled.pgm", labeled);
	//printf("Labels: %d\n", labels);

	vc_binary_blob_info(labeled, blobs, labels);//Preencher o array de blobs com a sua informação
	//Filtragem dos blobls encontrados
	for (int i = 0; i < labels; i++)//Percorrer o array de blobs
	{
		if (blobs[i].height != 0 && blobs[i].width != 0 && blobs[i].area != 0)//Filtrar os blobs com valores inválidos
		{
			float r = blobs[i].width / blobs[i].height;//Calculo do racio largura/altura
			if (r >= 4 && r <= 5 && blobs[i].area > 5000)//Se o racio estiver dentro dde 4 e 5 e a area for maior que 5000
			{
				//printf("_______________BLOB________________\n");
				//printf("Label: %d\n", blobs[i].label);
				//printf("Width: %d\n", blobs[i].width);
				//printf("Height: %d\n", blobs[i].height);
				//printf("Area: %d\n", blobs[i].area);
				//printf("X: %d\n", blobs[i].x);
				//printf("Y: %d\n", blobs[i].y);

				//Criar uma imagem nova do tamanho do blob e copia-lo para lá
				IVC* blob = vc_image_new(blobs[i].width, blobs[i].height, 1, 255);//Imagem nova
				int bwidth = blobs[i].width;
				int bheight = blobs[i].height;

				int pos = 0;
				int posb = 0;
				int xb = 0, yb = 0;
				//Copiar a imagem
				for (int y = blobs[i].y; y < bheight + blobs[i].y; y++)
				{
					for (int x = blobs[i].x; x < bwidth + blobs[i].x; x++)
					{
						pos = y * close->width + x;//Posição na imagem original
						posb = yb * bwidth + xb;//Posição na imagem do blob
						blob->data[posb] = gray->data[pos];
						xb++;
					}
					yb++;
					xb = 0;
				}
				//vc_write_image("blob.pgm", blob);

				IVC* matriculabin = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para a imagem binaria do blob
				IVC* matriculaclose = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para o fecho
				IVC* matriculalabel = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para os labels
				
				vc_gray_to_binary_bernsen(blob, matriculabin, 3,240);//Conversão para binario com o metodo de bernsen
				//vc_write_image("matriculabin.pgm", matriculabin);
				vc_binary_close(matriculabin, matriculaclose, 3);//Operação morfológica de fecho com kerner 3x3				
				/*int max = 0;
				int colprof[500] = { 0 };

				for (int w = 0; w < matriculaclose->width; w++)
				{
					for (int h = 0; h < matriculaclose->height; h++)
					{
						pos = h * matriculaclose->width + w;
						if (matriculaclose->data[pos] == 0)colprof[w]++;
					}
				}
				IVC* h = vc_image_new(500, 200, 1, 255);
				max = colprof[0];
				for (int i = 1; i < 256; i++) {
					if (max < colprof[i]) {
						max = colprof[i];
					}
				}

				int n = matriculaclose->height * matriculaclose->width;
				for (int x = 0; x < h->width; x++)
				{
					if (max != 0)
					{
						for (int y = h->height - ((h->height * colprof[x]) / max); y < h->height; y++)
						{
							pos = y * h->width + x * h->channels;
							h->data[pos] = 0;
						}

					}
				}
				vc_write_image("colprof.pgm", h);
				
				for (int j = 0; j < matriculaclose->width; j++)
				{
					printf("Colprod %d: %d\n",j, colprof[j]);
				}*/
				//vc_write_image("matriculaclose.pgm", matriculaclose);
				vc_bin_negative(matriculaclose);//Negativo do close
				int nletras = 0;//Variável para o numero de blobs

				OVC* letras = vc_binary_blob_labelling(matriculaclose, matriculalabel, &nletras);//Fazer o labelling da imagem
				//vc_write_image("matriculalabel.pgm", matriculalabel);

				vc_binary_blob_info(matriculalabel, letras, nletras);//Preencher o array com a informação de cada label

				//Verificar quais sao os blobs validos
				if (nletras >= 6)//Se o numero de blobs encontrados for menor 6 é descartado
				{
					//Desenho do limite superior do blob 
					for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
					{
						pos = blobs[i].y * src->bytesperline + x * src->channels;//Calculo da posiçao
						src->data[pos] = 0;//R
						src->data[pos + 1] = 255;//G
						src->data[pos + 2] = 255;//B
					}
					//Desenho do limite inferior do blob 
					for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)
					{
						pos = (blobs[i].y + blobs[i].height) * src->bytesperline + x * src->channels;//Calculo da posiçao
						src->data[pos] = 0;//R
						src->data[pos + 1] = 255;//G
						src->data[pos + 2] = 255;//B
					}
					//Desenho do limite esquerdo do blob 
					for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
					{
						pos = y * src->bytesperline + blobs[i].x * src->channels;//Calculo da posiçao
						src->data[pos] = 0;//R
						src->data[pos + 1] = 255;//G
						src->data[pos + 2] = 255;//B
					}
					//Desenho do limite direito do blob 
					for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)
					{
						pos = y * src->bytesperline + (blobs[i].x + blobs[i].width) * src->channels;//Calculo da posiçao
						src->data[pos] = 0;//R
						src->data[pos + 1] = 255;//G
						src->data[pos + 2] = 255;//B
					}

					for (int l = 0; l < nletras; l++)//Percorrer os array de labels
					{
						r = (float)letras[l].width / (float)letras[l].height;//Racio de largura/altura
						if (r > 0.25 && r < 0.9 && letras[l].area > 130)
						{
							//printf("_______________LETRA________________\n");
							//printf("Label: %d\n", letras[l].label);
							//printf("Width: %d\n", letras[l].width);
							//printf("Height: %d\n", letras[l].height);
							//printf("Area: %d\n", letras[l].area);
							//printf("X: %d\n", letras[l].x);
							//printf("Y: %d\n", letras[l].y);

							//Desenho do limite superior do blob 
							for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)
							{
								pos = (letras[l].y + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posiçao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite inferior do blob 
							for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)
							{
								pos = (letras[l].y + letras[l].height + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posiçao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite esquerdo do blob 
							for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)
							{
								pos = y * src->bytesperline + (blobs[i].x + letras[l].x) * src->channels;//Calculo da posiçao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
							//Desenho do limite direito do blob 
							for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)
							{
								pos = y * src->bytesperline + (letras[l].x + letras[l].width + blobs[i].x) * src->channels;//Calculo da posiçao
								src->data[pos] = 255;//R
								src->data[pos + 1] = 0;//G
								src->data[pos + 2] = 0;//B
							}
						}
					}
				}
			}
		}		
	}
}


