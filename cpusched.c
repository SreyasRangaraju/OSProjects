#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
	int pid, arv, burst, fnsh, wait, count;
	struct node *next;
}node;

int complete(node *front, int time, FILE *out)
{
	node *temp = front->next;
	time += temp->count;
	temp->fnsh = time;
	temp->wait = temp->fnsh - temp->arv - temp->burst;
	fprintf(out, "%d %d %d %d\n",temp->pid, temp->arv, temp->fnsh, temp->wait);
	front->next = temp->next;
	free(temp);
	return time;
}

void addPrc(int pid, int arv, int burst, node *cur, node *front)
{
	while(cur->next && (burst > cur->next->count || (burst == cur->next->count && pid > cur->next->pid)))
	{
		cur = cur->next;
	}
	node *temp = (node*)malloc(sizeof(node));
	temp->pid = pid, temp->arv = arv, temp->burst = burst, temp->count = burst;
	temp->next = cur->next;
	cur->next = temp;
	cur = front;
}

void SRTF(FILE *in, FILE *out, int limit)
{
	node *front, *cur;	
	int pid, arv, burst, time = 0, end = 0;
	front = (node*)malloc(sizeof(node));
	front->next = 0;
	cur = front;
	while (front->next || !end)
	{
		if(end == 0)
		{	
			if(fscanf(in, "%d %d %d", &pid, &arv, &burst) == EOF || !limit)
			{
				end = 1;
			}
		}
		while(front->next && (front->next->count <= arv - time || end == 1))
		{
			time = complete(front, time, out);
		}
		if(front->next && time < arv)
		{
			front->next->count -= arv - time;
			time = arv;
		}
		if(time == arv && !end)
		{
			addPrc(pid, arv, burst, cur, front);
			limit--;
		}
		
	}
	free(front);
}

void FCFS(FILE *in, FILE *out, int limit)
{
	int time = 0, pid, arv, burst, wait;
	while(fscanf(in,"%d %d %d", &pid, &arv, &burst) != EOF && limit)
	{
		time += burst;
		wait = time - (arv + burst);
		fprintf(out, "%d %d %d %d\n", pid, arv, time, wait);
		limit--;
	}
}
int main(int argc, char *argv[])
{
	FILE *in, *out;
	int limit = -1;
	in = fopen(argv[1],"r");
	out = fopen(argv[2],"w");
	if(argc == 5)
	{
		limit = atoi(argv[4]);
	}
	if(!strcmp(argv[3],"SRTF"))
	{
		SRTF(in, out, limit);
	}
	if(!strcmp(argv[3],"FCFS"))
	{
		FCFS(in, out, limit);
	}
	fclose(in);
	fclose(out);
	return 0;
}
