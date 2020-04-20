#include "vc.h"
#include <string.h>

int BoundingBox(IVC* src)
{
	IVC* gray = vc_image_new(src->width, src->height, 1, 255);//Imagem em cinzento para converter a imagem para cinzento
	IVC* bin = vc_image_new(src->width, src->height, 1, 255);//Imagen em binario para converter a imagem para binario
	IVC* close = vc_image_new(src->width, src->height, 1, 255);//Imagem em bin para fazer um fecho close 
	IVC* labeled = vc_image_new(src->width, src->height, 1, 255);//Imagem para os labels 
 
	vc_rgb_to_gray(src, gray);//Converter a imagem original para Escala de cinzentos
	//vc_write_image("gray.pgm", gray);
	vc_gray_to_binary_bernsen(gray, bin, 25, 240);//Converter para binário usando o metodo de bernsen com um cmin de 240 para encontrar zonas de alto
												 //contraste na imagem
	vc_binary_close(bin, close, 5);//Fazer um fecho da imagem com kernerl de 5
	//vc_write_image("bin.pgm", bin);
	//vc_write_image("close.pgm", close);	
	int labels = 0;//Variável para o número de labels encontrados
	OVC* blobs = vc_binary_blob_labelling(close, labeled, &labels);//Fazer o labelling da imagem para uma variável que vai conter a informação
																//dos blobs que podem se matriculass
	//vc_write_image("labeled.pgm", labeled);
	//printf("Labels: %d\n", labels);

	vc_binary_blob_info(labeled, blobs, labels);//Preencher o array de blobs com a sua informação
	int foundany = 0;
	//Filtragem dos blobls encontrados
	for (int i = 0; i < labels; i++)//Percorrer o array de blobs
	{
		if (blobs[i].height != 0 && blobs[i].width != 0 && blobs[i].area != 0)//Filtrar os blobs com valores inválidos
		{
			float r = blobs[i].width / blobs[i].height;//Calculo do racio largura/altura
			if (r >= 4 && r <= 5 && blobs[i].area > 5000)//Se o racio estiver dentro dde 4 e 5 e a area for maior que 5000
			{
				foundany = 1;
				//printf("_______________BLOB________________\n");
				//printf("Label: %d\n", blobs[i].label);
				//printf("Width: %d\n", blobs[i].width);
				//printf("Height: %d\n", blobs[i].height);
				//printf("Area: %d\n", blobs[i].area);
				//printf("X: %d\n", blobs[i].x);
				//printf("Y: %d\n", blobs[i].y);

				//Criar uma imagem nova do tamanho do blob e copia-lo para lá
				IVC* blob = vc_image_new(blobs[i].width, blobs[i].height, 1, 255);//Imagem nova para analisar o blob e determinar se pode ser
																				//uma matricula ou não, com as dimensões do blob
				int bwidth = blobs[i].width;
				int bheight = blobs[i].height;

				int pos = 0;//Posição na imagem original
				int posb = 0;//Posição na imagem para o blob
				int xb = 0, yb = 0;
				//Copiar o blob encontrado, da imagem original para um imagem nova para ser analisado
				for (int y = blobs[i].y; y < bheight + blobs[i].y; y++)
				{
					for (int x = blobs[i].x; x < bwidth + blobs[i].x; x++)
					{
						pos = y * close->width + x;//Posição na imagem original
						posb = yb * bwidth + xb;//Posição na imagem do blob
						blob->data[posb] = gray->data[pos];//Copiar os dados
						xb++;//Incrementar uma coluna á posição no blob
					}
					yb++;//Incrementar uma linha á posição no blob
					xb = 0;//Por a posição nas colunas do blob a zero
				}
				//vc_write_image("blob.pgm", blob);

				IVC* matriculabin = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para a imagem binaria do blob
				IVC* matriculaclose = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para o fecho
				IVC* matriculalabel = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para os labels
				
				vc_gray_to_binary_bernsen(blob, matriculabin, 3,240);//Conversão para binario com o metodo de bernsen
				//vc_write_image("matriculabin.pgm", matriculabin);
				vc_binary_close(matriculabin, matriculaclose, 3);//Operação morfológica de fecho com kerner 3x3				
				//vc_write_image("matriculaclose.pgm", matriculaclose);
				vc_bin_negative(matriculaclose);//Negativo do close
				int nletras = 0;//Variável para o numero de blobs

				OVC* letras = vc_binary_blob_labelling(matriculaclose, matriculalabel, &nletras);//Fazer o labelling da imagem
				//vc_write_image("matriculalabel.pgm", matriculalabel);

				vc_binary_blob_info(matriculalabel, letras, nletras);//Preencher o array com a informação de cada label

				//Verificar quais sao os blobs validos
				if (nletras >= 6)//Se o numero de blobs encontrados for menor 6 é descartado
				{
					/*
					O desenho da bounding box da matricula é feita por partes, fazendo primeiro o desenho da reta superior percorrendo toda a
					largura da matricula na mesma linha, depois a reta inferior da bounding box pelo mesmo processom. O desenho das laterais
					da bounding box é feito percorrendo toda a altura da matricula numa coluna fixa.
					*/
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
							/*
							O desenho das letras é o mesmo processo da matricula sendo simplesmente com as dimensões das letras
							*/
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
				free(letras);//Libertar a memoria usada para a estrutura que guarda a informação das letras
			}
		}
	}
	free(blobs);//Libertar a memoria usada para a estrutura que guarda a informação das matriculas

	if (foundany == 0)//Se não for encontrado nenhum blob é feito o msm algoritmo com alguns tweaks
	{
		IVC* gray = vc_image_new(src->width, src->height, 1, 255);//Imagem em cinzento para converter a imagem para cinzento
		IVC* bin = vc_image_new(src->width, src->height, 1, 255);//Imagen em binario para converter a imagem para binario
		IVC* close = vc_image_new(src->width, src->height, 1, 255);//Imagem em bin para fazer um fecho
		IVC* labeled = vc_image_new(src->width, src->height, 1, 255);//Imagem para os labels 
		IVC* erode = vc_image_new(src->width, src->height, 1, 255);//Imagem para uma erosão

		vc_rgb_to_gray(src, gray);//Converter a imagem original para Escala de cinzentos
		//vc_write_image("gray.pgm", gray);
		vc_gray_to_binary_bernsen(gray, bin, 25, 240);//Converter para binário usando o metodo de bernsen com um cmin de 240 para encontrar zonas de alto												 //contraste na imagem
		vc_binary_close(bin, close, 3);//Fazer um close da imagem com kernerl de 3
		vc_binary_erode(close, erode, 3);//Fazer um erosão da imagem com kernerl de 3
		int labels = 0;//Número de labels
		OVC* blobs = vc_binary_blob_labelling(erode, labeled, &labels);//Fazer o labelling da imagem
		//vc_write_image("labeled.pgm", labeled);
		//vc_write_image("close.pgm", close);
		//vc_write_image("erode.pgm", close);
		//printf("Labels: %d\n", labels);
		vc_binary_blob_info(labeled, blobs, labels);//Preencher o array de blobs com a sua informação

		int foundany = 0; //Variável para guardar se foi encontrado algum blob com os requesitos
		//Filtragem dos blobls encontrados
		for (int i = 0; i < labels; i++)//Percorrer o array de blobs
		{
			if (blobs[i].height != 0 && blobs[i].width != 0 && blobs[i].area != 0)//Filtrar os blobs com valores inválidos
			{
				float r = blobs[i].width / blobs[i].height;//Calculo do racio largura/altura do blob para saber se tem as proporções corretas
				if (r >= 4 && r <= 5 && blobs[i].area > 5000)//Se o racio estiver dentro dde 4 e 5 e a area for maior que 5000
				{
					foundany = 1;//Por a 1 para indicar que foi encontrado 1

					//printf("_______________BLOB________________\n");
					//printf("Label: %d\n", blobs[i].label);
					//printf("Width: %d\n", blobs[i].width);
					//printf("Height: %d\n", blobs[i].height);
					//printf("Area: %d\n", blobs[i].area);
					//printf("X: %d\n", blobs[i].x);
					//printf("Y: %d\n", blobs[i].y);

					//Criar uma imagem nova do tamanho do blob e copia-lo para lá
					IVC* blob = vc_image_new(blobs[i].width, blobs[i].height, 1, 255);//Imagem nova para o blob com as dimensões do blob
					int bwidth = blobs[i].width;
					int bheight = blobs[i].height;

					int pos = 0;//Posição na imagem original
					int posb = 0;//Posição na imagem para o blob
					int xb = 0, yb = 0;//
					//Copiar a imagem
					for (int y = blobs[i].y; y < bheight + blobs[i].y; y++)
					{
						for (int x = blobs[i].x; x < bwidth + blobs[i].x; x++)
						{
							pos = y * close->width + x;//Posição na imagem original
							posb = yb * bwidth + xb;//Posição na imagem do blob
							blob->data[posb] = gray->data[pos];//Copiar os dados
							xb++;//Incrementar uma coluna á posição no blob
						}
						yb++;//Incrementar uma linha á posição no blob
						xb = 0;//Por a posição nas colunas do blob a zero
					}
					//vc_write_image("blob.pgm", blob);

					IVC* matriculabin = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para a imagem binaria do blob
					IVC* matriculaclose = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para o fecho
					IVC* matriculalabel = vc_image_new(blob->width, blob->height, 1, 255);//Imagem para os labels

					vc_gray_to_binary_bernsen(blob, matriculabin, 3, 240);//Conversão para binario com o metodo de bernsen com um kernel de 3 e um Cmin de 240
					//vc_write_image("matriculabin.pgm", matriculabin);
					vc_binary_close(matriculabin, matriculaclose, 3);//Operação morfológica de fecho com kerner 3x3				
					//vc_write_image("matriculaclose.pgm", matriculaclose);
					vc_bin_negative(matriculaclose);//Negativo do close
					int nletras = 0;//Variável para o numero de blobs

					OVC* letras = vc_binary_blob_labelling(matriculaclose, matriculalabel, &nletras);//Fazer o labelling da imagem
					//vc_write_image("matriculalabel.pgm", matriculalabel);

					vc_binary_blob_info(matriculalabel, letras, nletras);//Preencher o array com a informação de cada label

					int req = 0;//Variável para contar o número de blobs encontrados que cumprem certos requsitos
					for (int l = 0; l < nletras; l++)//Percorrer os array de labels
					{
						r = (float)letras[l].width / (float)letras[l].height;//Racio de largura/altura
						if (r > 0.25 && r < 0.9 && letras[l].area > 130)//Requisitos
						{
							req++;//Incrementar a variável
						}
					}
					//Verificar quais são os blobs validos
					if (nletras >= 6 && req >= 6)//Se o numero de blobs encontrados for menor 6 é descartado
					{
						/*
						O desenho da bounding box da matricula é feita por partes, fazendo primeiro o desenho da reta superior percorrendo toda a
						largura da matricula na mesma linha, depois a reta inferior da bounding box pelo mesmo processom. O desenho das laterais
						da bounding box é feito percorrendo toda a altura da matricula numa coluna fixa.
						*/
						//Desenho do limite superior do blob na imagem original
						for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)//Ciclo para percorrer as colunas da imagem original
						{
							pos = blobs[i].y * src->bytesperline + x * src->channels;//Calculo da posiçao na imagem
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite inferior do blob 
						for (int x = blobs[i].x; x < blobs[i].x + blobs[i].width; x++)//Ciclo para percorrer as colunas da imagem original
						{
							pos = (blobs[i].y + blobs[i].height) * src->bytesperline + x * src->channels;//Calculo da posiçao na imagem
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite esquerdo do blob 
						for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)//Ciclo para percorrer as linhas da imagem original
						{
							pos = y * src->bytesperline + blobs[i].x * src->channels;;//Calculo da posiçao na imagem
							src->data[pos] = 0;//R
							src->data[pos + 1] = 255;//G
							src->data[pos + 2] = 255;//B
						}
						//Desenho do limite direito do blob 
						for (int y = blobs[i].y; y < blobs[i].y + blobs[i].height; y++)//Ciclo para percorrer as linhas da imagem original
						{
							pos = y * src->bytesperline + (blobs[i].x + blobs[i].width) * src->channels;;//Calculo da posiçao na imagem
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
								/*
								O desenho das letras é o mesmo processo da matricula sendo simplesmente com as dimensões das letras
								*/
								//Desenho do limite superior do blob
								for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)// Ciclo para percorrer as colunas da imagem original
								{
									pos = (letras[l].y + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posiçao na imagem original
									src->data[pos] = 255;//R
									src->data[pos + 1] = 0;//G
									src->data[pos + 2] = 0;//B
								}
								//Desenho do limite inferior do blob 
								for (int x = letras[l].x + blobs[i].x; x < letras[l].x + letras[l].width + blobs[i].x; x++)// Ciclo para percorrer as colunas da imagem original
								{
									pos = (letras[l].y + letras[l].height + blobs[i].y) * src->bytesperline + x * src->channels;//Calculo da posiçao na imagem original
									src->data[pos] = 255;//R
									src->data[pos + 1] = 0;//G
									src->data[pos + 2] = 0;//B
								}
								//Desenho do limite esquerdo do blob 
								for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)// Ciclo para percorrer as linhas da imagem original
								{
									pos = y * src->bytesperline + (blobs[i].x + letras[l].x) * src->channels;//Calculo da posiçao na imagem original
									src->data[pos] = 255;//R
									src->data[pos + 1] = 0;//G
									src->data[pos + 2] = 0;//B
								}
								//Desenho do limite direito do blob 
								for (int y = letras[l].y + blobs[i].y; y < letras[l].y + letras[l].height + blobs[i].y; y++)// Ciclo para percorrer as linhas da imagem original
								{
									pos = y * src->bytesperline + (letras[l].x + letras[l].width + blobs[i].x) * src->channels;//Calculo da posiçao na imagem original
									src->data[pos] = 255;//R
									src->data[pos + 1] = 0;//G
									src->data[pos + 2] = 0;//B
								}
							}
						}
						
					}
					free(letras);//Libertar a memoria usada para a estrutura que guarda a informação das letras
				}
			}
		}
		free(blobs);//Libertar a memoria usada para a estrutura que guarda a informação das matriculas
	}	
}


