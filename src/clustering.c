#include "common.h"
#include "clustering.h"

#include <math.h>

int compare_normes(const void* a, const void* b)
{
	const IntDoublePair* da = (const IntDoublePair*) a;
	const IntDoublePair* db = (const IntDoublePair*) b;
	
	return (da->v > db->v) - (da->v < db->v);
}

int compare_ints(const void* a, const void* b)
{
	return (*(int*) a - *(int*) b);
}

Cluster* clustering(DMat Vt, double* S, int* clusterCount)
{
	printf("Clustering...\n");
	
	/* Etape 1 : Tri des colonnes par ordre croissant de norme */
	
	IntDoublePair normes[Vt->cols];
	int i,j;
	
	for (i = 0; i < Vt->cols; i++)
	{
		normes[i].i = i;
		normes[i].v = col_norme(Vt, S, i);
	}
	
	qsort(normes, Vt->cols, sizeof(IntDoublePair), compare_normes);
	
	DMat Vt_sorted = svdNewDMat(Vt->rows, Vt->cols);
	
	for (i = 0; i < Vt->cols; i++)
	{
		for (j = 0; j < Vt->rows; j++)
		{
			Vt_sorted->value[j][i] = Vt->value[j][normes[i].i];
		}
	}
	
	bool clustered[Vt->cols];
	int clusterCounter = 0;
	
	for (i = 0; i < Vt->cols; i++)
		clustered[i] = false;
	
	Cluster* clusters;
	
	while (1)
	{
		/* Etape 2 : Sélection du vecteur non-clusteré le plus proche de l'origine */
		
		int closestVector = 0;
		while (closestVector < Vt->cols && clustered[closestVector])
			closestVector++;
		
		if (closestVector >= Vt->cols)
			break;
		
		clusterCounter++;
		
		if (clusterCounter == 1)
			clusters = malloc(sizeof(Cluster));
		else
			clusters = realloc(clusters, clusterCounter * sizeof(Cluster));
		
		Cluster cluster;
		cluster.averageInternalDistance = 0.0;
		cluster.frameCount = 1;
		cluster.frames = malloc(sizeof(int));
		cluster.frames[0] = closestVector;
		
		clustered[closestVector] = true;
		
		/* Etape 3 : Calcul de la distance au cluster courant pour chaque vecteur non-clusteré et ajout */
		
		for (i = closestVector + 1; i < Vt->cols; i++)
		{
			if (clustered[i])
				continue;
			
			double minDistance = min_distance_to_cluster(Vt_sorted, S, i, cluster);
			
			if (clusterCounter == 1)
			{				
				if (cluster.averageInternalDistance == 0.0 || minDistance / cluster.averageInternalDistance < 5.0)
				{
					add_frame_to_cluster(Vt_sorted, i, &cluster, normes, minDistance);
					clustered[i] = true;
				}
			}
			
			else
			{
				if (cluster.averageInternalDistance == 0.0 || cluster.contentValue < clusters[0].contentValue || minDistance / cluster.averageInternalDistance < 2.0)
				{
					add_frame_to_cluster(Vt_sorted, i, &cluster, normes, minDistance);
					clustered[i] = true;
				}
			}
		}
		
		clusters[clusterCounter-1] = cluster;
	}
	
	for (i = 0; i < clusterCounter; i++)
	{
		for (j = 0; j < clusters[i].frameCount; j++)
		{
			clusters[i].frames[j] = normes[clusters[i].frames[j]].i;
		}
		
		qsort(clusters[i].frames, clusters[i].frameCount, sizeof(int), compare_ints);
	}
	
	svdFreeDMat(Vt_sorted);
	
	printf("%d cluster(s) obtenu(s).\n", clusterCounter);
	
	*clusterCount = clusterCounter;
	return clusters;
}

Shot find_longest_shot(Cluster cluster)
{
	int shotCount = 0;
	Shot* shots = find_shots(cluster, &shotCount);
	
	int longestShot = 0;
	int i;
	
	for (i = 1; i < shotCount; i++)
	{
		if (shots[i].frameCount > shots[longestShot].frameCount)
			longestShot = i;
	} 
	
	return shots[longestShot];
}

Shot* find_shots(Cluster cluster, int* shotCount)
{
	Shot* shots = malloc(sizeof(Shot));
	
	shots[0].frameCount = 1;
	shots[0].frames = malloc(sizeof(int));
	shots[0].frames[0] = cluster.frames[0];
	
	int shotCounter = 1;
	
	int i;
	for (i = 1; i < cluster.frameCount; i++)
	{
		if (cluster.frames[i] == cluster.frames[i-1] + 1)
		{
			shots[shotCounter-1].frameCount++;
			shots[shotCounter-1].frames = realloc(shots[shotCounter-1].frames, shots[shotCounter-1].frameCount * sizeof(int));
			shots[shotCounter-1].frames[shots[shotCounter-1].frameCount-1] = cluster.frames[i];
		}
		
		else
		{
			shotCounter++;
			shots = realloc(shots, shotCounter * sizeof(Shot));
			
			shots[shotCounter-1].frameCount = 1;
			shots[shotCounter-1].frames = malloc(sizeof(int));
			shots[shotCounter-1].frames[0] = cluster.frames[i];
		}
	}
	
	*shotCount = shotCounter;
	return shots;
}

double col_norme(DMat Vt, double* S, int i)
{
	double sum = 0.0;
	
	int n = Vt->rows;
	if (n > 150)
		n = 150;
	
	int j;
	for (j = 0; j < n; j++)
	{
		sum += S[j] * Vt->value[j][i] * Vt->value[j][i];
	}
	
	return sqrt(sum);
}

double col_distance(DMat Vt, double* S, int i, int j)
{
	double sum = 0.0;
	
	int n = Vt->rows;
	if (n > 150)
		n = 150;
	
	int l;
	for (l = 0; l < n; l++)
	{
		sum += S[l] * (Vt->value[l][i] - Vt->value[l][j]) * (Vt->value[l][i] - Vt->value[l][j]);
	}
	
	return sqrt(sum);
}

double min_distance_to_cluster(DMat Vt, double* S, int i, Cluster cluster)
{
	double minDistance = col_distance(Vt, S, i, cluster.frames[0]);
	
	int k;
	for (k = 1; k < cluster.frameCount; k++)
	{
		double d = col_distance(Vt, S, i, cluster.frames[k]);
		
		if (d < minDistance)
			minDistance = d;
	}
	
	return minDistance;
}

void add_frame_to_cluster(DMat Vt, int i, Cluster* cluster, IntDoublePair* normes, double minDistance)
{
	cluster->frames = realloc(cluster->frames, (cluster->frameCount + 1) * sizeof(int));
	cluster->frames[cluster->frameCount] = i;
	
	cluster->frameCount++;
	cluster->contentValue = 0.0;
	
	int k;
	for (k = 0; k < cluster->frameCount; k++)
	{
		cluster->contentValue += normes[cluster->frames[k]].v * normes[cluster->frames[k]].v;
	}
	
	cluster->averageInternalDistance = ((((double) cluster->frameCount) - 1.0) * cluster->averageInternalDistance + minDistance) / ((double) cluster->frameCount);
}
