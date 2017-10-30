#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct node
{
	int occ;
	char *word;
	struct node *next;
}node;

int inputs(FILE *in, char ***words)
{
	char string[50];
	int j = 0;
	char **word = (char**)malloc(0);
	while (fscanf(in, "%s", string) != EOF)
	{
		word = (char**)realloc(word,8*(j+1));
		word[j] = strdup(string);
		j++;
	}
	*words = word;
	return j;
}

void printFreeList(node* lst, char* outs)
{
	node *temp;
	FILE *out;
	out = fopen(outs,"w");
	while (lst)
	{
		temp = lst;
		lst = lst->next;
		fprintf(out,"%s,%d\n",temp->word,temp->occ);
		free(temp->word);
		free(temp);
	}
	fclose(out);
}

void freeArr(char ** arr, int size)
{
	while (size > 0)
	{
		free(arr[size-1]);
		size--;
	}
	free(arr);
}

void swap(char **str1, char **str2)
{
	char *temp = *str1;
	*str1 = *str2;
	*str2 = temp;
}

int split(char **word, int first, int last)
{
	int pivInd = (first + last)/2, storeInd = first, i;	
	swap(&word[pivInd],&word[last]);
	for (i = first;i < last; i++)
	{
		if (strcmp(word[i],word[last]) < 0)
		{
			swap(&word[i], &word[storeInd]);
			storeInd++;
		}
	}
	swap(&word[storeInd],&word[last]);
	return storeInd;
}

void quickSort(char **word, int first, int last)
{
	int mid = split(word, first, last);
	if (mid-1 > first)
	{
		quickSort(word, first, mid-1);
	}
	if (mid + 1 < last)
	{	
		quickSort(word, mid+1, last);
	}
}

node* cList(char **word1, char **word2, int size1, int size2)
{
	int i, j, count, common;
	char *word;
	node *curr, *root = NULL;
	for (i = 0, j = 0;i < size1 && j < size2;i++)
	{
		word = strdup(word1[i]);
		count = 1;
		common = 0;
		while (i < size1 - 1 && strcmp(word,word1[i+1]) == 0)
		{
			i++;
			count++;
		}
		while (j < size2 && strcmp(word,word2[j]) >= 0)
		{
			if (strcmp(word,word2[j]) == 0)
			{
				common = 1;
				count++;
			}
			j++;
		}
		if (common == 1)
		{
			if (!root)
			{
				root = (node*)malloc(sizeof(node));
				root->occ = count;
				root->word = strdup(word);
				root->next = 0;
				curr = root;
			}
			else
			{
				curr->next = (node*)malloc(sizeof(node));
				curr = curr->next;
				curr->occ = count;
				curr->word = strdup(word);
				curr->next = 0;
			}
		}
		free(word);
	}
	return root;
}

int main(int argc, char *argv[])
{
	FILE *in1, *in2;
	char **words1, **words2;
	int size1, size2;
	node *n;
	in1 = fopen(argv[1],"r");
	in2 = fopen(argv[2],"r");
	size1 = inputs(in1, &words1);
	size2 = inputs(in2, &words2);
	quickSort(words1, 0, size1-1);
	quickSort(words2, 0, size2-1);
	n = cList(words1, words2, size1, size2);
	printFreeList(n,argv[3]);
	fclose(in1);
	fclose(in2);
	freeArr(words1, size1);
	freeArr(words2, size2);
	return 0;
}
