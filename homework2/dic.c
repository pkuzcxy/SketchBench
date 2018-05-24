//CS50 Project 02
//Charlie Chu

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define VOLUME 143091
#define WORD_COUNT 24000
#define MAXLEN 50
#define UNDEF 1200

int main(void){

	char *dic[VOLUME+1];

	int i,j;
	i=0;
	while(1){
		scanf("%s",dic[i]);
		if(strcmp(dic[i],"zymurgy")==0) break;
		i++;
	}

	char *word[WORD_COUNT];

	int quant;
	for(quant=0;scanf("%s",word[quant])!=EOF;quant++);
	
	char *word_lc[WORD_COUNT];
	int inf=0;
	int sup=VOLUME;
	int med;
	int und=0;
	int undef[UNDEF];

	for(i=0;i<quant;i++){
		word_lc[i]=word[i];
		for(j=0;word[i][j]!=0;j++){
			word_lc[i][j]=tolower(word_lc[i][j]);
		}

		while(inf<=sup){
			med=(inf+sup)/2;
			if(strcmp(word_lc[i],dic[med])>0){
				inf=med+1;
			}
			else if(strcmp(word_lc[i],dic[med])<0){
				sup=med-1;
			}
			else if(strcmp(word_lc[i],dic[med])==0){
				break;
			}
		}
		if(strcmp(word_lc[i],dic[med])!=0){
			undef[und]=i;
			und++;
		}
	}

	for(i=0;i<und;i++){
		printf("%s\n",word[undef[und]]);
	}

	printf("Words Misspelled: %d\n",und);

}



