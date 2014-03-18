#include "common.h"
#include "imagefeatures.h"

#include <dirent.h>
#include <stb_image.h>
#include <string.h>

bool extract_features_matrix(DMat* matrix, const char* imagefolder, bool* transposed)
{
	struct dirent** namelist;
	int n = scandir(imagefolder, &namelist, 0, alphasort);
	
	if (n < 0)
	{
		printf("Echec du parcours du dossier d'images !");
		return false;
	}
	
	DMat mat = svdNewDMat(n-2, 1125);
	
	int i;
	for (i = 2; i < n; i++)
	{
		printf("Extraction des features de l'image %s...\n", namelist[i]->d_name);
		
		char filename[128] = {'\0'};
		sprintf(filename, "%s/%s", imagefolder, namelist[i]->d_name);
		
		int width;
		int height;
		int comp;
		
		unsigned char* imageData = stbi_load(filename, &width, &height, &comp, STBI_rgb);
		
		construct_vector(mat->value[i-2], imageData, width, height);
		
		stbi_image_free(imageData);
		free(namelist[i]);
	}
	
	if (mat->rows >= mat->cols)
	{
		*matrix = mat;
		*transposed = true;
	}
	
	else
	{
		*matrix = svdTransposeD(mat);
		*transposed = false;
		svdFreeDMat(mat);
	}
	
	free(namelist);
	return true;
}

void construct_vector(double* vector, unsigned char* imageData, int width, int height)
{
	int y,x;
	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 3; x++)
		{
			construct_histogram(vector, imageData, width, height, x ,y);
		}
	}
}

void construct_histogram(double* vector, unsigned char* imageData, int width, int height, int blocX, int blocY)
{
	double histogram[125] = {0.0};
	
	int yStart = blocY * (height / 3);
	int yEnd = yStart + (height / 3);
	int xStart = blocX * (width / 3);
	int xEnd = xStart + (width / 3);
	
	int y,x;
	for (y = yStart; y < yEnd; y++)
	{
		for (x = xStart; x < xEnd; x++)
		{
			int r = imageData[3 * (width * y + x)] / 52;
			int g = imageData[3 * (width * y + x) + 1] / 52;
			int b = imageData[3 * (width * y + x) + 2] / 52;
			
			histogram[25 * r + 5 * g + b] += 1.0;
		}
	}
	
	memcpy(&vector[125 * (3 * blocY + blocX)], histogram, 125 * sizeof(double));
}
