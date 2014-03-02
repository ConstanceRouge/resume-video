#include "common.h"
#include "videoutils.h"
#include "imagefeatures.h"

#include <unistd.h>

int main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		printf("Usage : resume-video video.avi\n");
		return EXIT_FAILURE;
	}
	
	char* filename = argv[1];
	
	if (access(filename, F_OK) == -1)
	{
		printf("Le fichier vidéo indiqué en paramètre n'existe pas !\n");
		return EXIT_FAILURE;
	}
	
	if (!extract_images(filename, 10, "tmp/"))
		return EXIT_FAILURE;
	
	DMat A;
	
	if (!extract_features_matrix(&A, "tmp/"))
		return EXIT_FAILURE;
	
	svdFreeDMat(A);
	
	return EXIT_SUCCESS;
}
