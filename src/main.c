#include "common.h"
#include "videoutils.h"
#include "imagefeatures.h"
#include "clustering.h"

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
	
	/* Etape 1 : Extraction des images et construction de la matrice A */
	
	//if (!extract_images(filename, 10, "tmp/"))
	//	return EXIT_FAILURE;
	
	DMat A;
	bool transposed;
	
	if (!extract_features_matrix(&A, "tmp/", &transposed))
		return EXIT_FAILURE;
	
	/* Etape 2 : SVD sur A pour obtenir Vt */
	
	SMat A_sparse = svdConvertDtoS(A);
	svdFreeDMat(A);
	
	SVDRec rec = svdLAS2A(A_sparse, A_sparse->cols);
	svdFreeSMat(A_sparse);
	
	DMat Vt = transposed ? rec->Ut : rec->Vt;
	
	/* Etape 3 : Regroupement des images en clusters */
	
	clustering(Vt, rec->S);
	
	svdFreeSVDRec(rec);
	
	return EXIT_SUCCESS;
}
