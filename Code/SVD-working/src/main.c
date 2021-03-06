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
	
	int fps;
	int interval = 10;
	
	if (!get_video_fps(filename, &fps))
		return EXIT_FAILURE;
	
	if (!extract_images(filename, interval, fps, "tmp/"))
		return EXIT_FAILURE;
	
	DMat A;
	bool transposed;
	
	if (!extract_features_matrix(&A, "tmp/", &transposed))
		return EXIT_FAILURE;
	
	/* Etape 2 : SVD sur A pour obtenir Vt */
	
	SMat A_sparse = svdConvertDtoS(A);
	svdFreeDMat(A);
	
	SVDRec rec = svdLAS2A(A_sparse, 0);
	svdFreeSMat(A_sparse);
	
	DMat Vt = transposed ? rec->Ut : rec->Vt;
	
	/* Etape 3 : Regroupement des images en clusters */
	
	int clusterCount = 0;
	Cluster* clusters = clustering(Vt, rec->S, &clusterCount);
	
	/* Etape 4 : On ne garde que les clusters dont la prise la plus longue dure au moins une seconde */
	
	int numValidClusters = 0;
	system("mkdir -p resume");
	system("rm -rf resume/*");
	
	int i;
	for (i = 0; i < clusterCount; i++)
	{
		Shot longestShot = find_longest_shot(clusters[i]);
		double length = ((double) (interval * longestShot.frameCount)) / ((double) fps);
		
		{
			char command[128] = {'\0'};
			
			sprintf(command, "mkdir -p resume/%d", i);
			system(command);
			
			int j;
			for (j = 0; j < clusters[i].frameCount; j++)
			{
				sprintf(command, "cp tmp/output%08d.png resume/%d", clusters[i].frames[j] + 1, i);
				system(command);
			}
		}
		
		if (length >= 0.75)
		{
			char command[128] = {'\0'};
			sprintf(command, "cp tmp/output%08d.png resume/", find_best_frame(clusters[i], Vt, rec->S) + 1);
			system(command);
			
			numValidClusters++;
		}
	}
	
	printf("%d cluster(s) retenu(s).\n", numValidClusters);
	
	svdFreeSVDRec(rec);
	
	return EXIT_SUCCESS;
}
