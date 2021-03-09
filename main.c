#include <stdio.h>
#include <stdlib.h>
//exceldeki butun verilerin tutuldugu struct
typedef struct user{
	char user[30];
	char bookname[50][50];
	int points[8];
	struct user *next;
}user;
//userlara ait similarityleri kaybetmemek adina bir structta tuttum.
typedef struct stsim{
	char user[30];
	char target[30];
	float sim;
	struct stsim *next;
}stsim;
//klasik linkli listeye ekleme islemi.
//Root degiskenini mainde tanimlayip surekli fonksiyonlara pointerla gonderiyorum.
user *createnode(user *r,char readert[20],char bookn[][200],int p[8]){
	int i;
	if(r == NULL){
		r = (user*)malloc(sizeof(user));
		strcpy(r->user,readert);
		for(i=0;i<8;i++){
			r->points[i] = p[i];
			strcpy(r->bookname[i],bookn[i]);
		}
		r->next = NULL;
	}else{
		user *iter;
		iter = r;
		while (iter->next != NULL){
			iter= iter->next;
		}
		user *temp = (user*)malloc(sizeof(user));
		temp->next = iter->next;
		strcpy(temp->user,readert);
		for(i=0;i<8;i++){
			temp->points[i] = p[i]	;
			strcpy(temp->bookname[i],bookn[i]);
		}
		iter->next = temp;	
	}	
	return r;
}
//ilgili kullanicilar icin olusturulacak similarty degerlerinin, bir struct'a atilmasi.
stsim *createsim(stsim *r,char reader[20],char readert[20],float s){
	int i;
	if(r == NULL){
		r = (stsim*)malloc(sizeof(stsim));
		strcpy(r->user,reader);
		strcpy(r->target,readert);
		r->sim = s;
		r->next = NULL;
	}else{
		int counter=0;
		stsim *iter;
		iter = r;
		while (iter->next != NULL){
			iter= iter->next;
			counter ++;
		}
		stsim *temp = (stsim*)malloc(sizeof(stsim));
		temp->next = iter->next;
		strcpy(temp->user,reader);
		strcpy(temp->target,readert);
		temp->sim = s;
		iter->next = temp;	
	}	
	return r;
}
//Kullanicidan alinan NU'lu deger icin butun Ux'ler icin similarity
//hesaplar ve bunlari struct'ta tutmak uzere createsim fonksiyonuna gonderir.
stsim *sim(user *r,char ad[20]){
	user *temp = r;
	user *temp2 = r;
	stsim *root=NULL;
	float sim;
	int rap[8],rbp[8],counter;
	float ra,rb;
	int i,j;
	while(temp->next != NULL){
		if(!strcmp(temp->user,ad))
			break;
		temp = temp->next;
	}
	for(i=0;i<20;i++){
		counter = 0;
		ra=0,rb=0;
		for(j=0;j<8;j++){
			if(temp->points[j] != 0 && temp2->points[j] != 0){
				ra += (float)(temp->points[j]);
				rb += (float)(temp2->points[j]);
				rap[counter] = temp->points[j];
				rbp[counter] = temp2->points[j];
				counter++;
			}
		}
		ra = ra/counter;
		rb = rb/counter;
		float pay=0,payda=0,a=0,b=0;
		int k=0;
		while(k<counter){
			pay += (rap[k]-ra) * (rbp[k]-rb);
			a += (rap[k]-ra) * (rap[k]-ra);
			b += (rbp[k]-rb) * (rbp[k]-rb);
			k++;
		}
		payda = sqrt(a*b);
		sim = pay / payda;
		root = createsim(root,ad,temp2->user,sim);
		temp2 = temp2->next;
	}
	return root;	
}
//---------------3. soru icin olan kisim; burada bir cok degisik counter olmasinin sebebi
//similarityleri tutarken dizi veya matris olarak degil, kisiye ozel olarak tutmus olmamdi.
//Ra, rb, rbp gibi degerleri almak biraz mesakatli oldugundan dolayi hepsini ayri ayri almak zorunda kaldim
//daha sonrasinda zaten okunmamis kitap kadar gezip pred hesapliyorum, daha sonra hesaplanan bu predlerden herhangi bir tanesini
//en buyuk kabul edip aralarinda bi siralama yapiyorum ve daha sonrasinda okuyucuya ilk structta tuttugum kitap isimlerine gore oneri yapiyorum.
void recommend(stsim *r,user *root,char ad[20],int kuser){
	int i=0,ucounter=0,kcounter=0,control=0,j=0,k=0,rac=0,counter2=0;
	stsim *iter = r;
	user *iter2 = root;
	user *temp;
	float simc=0,pay=0,ra=0,rb=0;
	float rbarray[20];
	int rbp[20][8];	
	while(iter2->next != NULL){
		if(!strcmp(iter2->user,ad))
			break;
		iter2 = iter2->next;
	}	
	i = 0;
	for(j=0;j<8;j++){
		if(iter2->points[j] != 0){
			ra += (float)(iter2->points[j]);
			rac++;
		}
	}
	ra = ra/rac;
	for(i=0;i<kuser;i++){
		temp = root;
		rb = 0;
		for(j=0;j<20;j++){
			if(!strcmp(temp->user,iter->target)){	
				ucounter=0;
				counter2 = 0;	
				for(k=0;k<8;k++){
					if(iter2->points[k] == 0){						
						rbp[kcounter][ucounter] = temp->points[k];
						ucounter++;
					}
					if(temp->points[k] != 0){
						rb += (float)(temp->points[k]);
						counter2++;
					}
				}
				rbarray[i] = rb/counter2;
				kcounter++;					
			}			
			temp = temp->next;					
		}
		iter = iter->next;	
	}
	i=0;
	float pred[kcounter];
	i=0,j=0;
	while(i < kcounter-1){
		iter = r;
		simc = 0;
		pay = 0;
		for(k=0;k<kuser;k++){
			simc += iter->sim;			
			pay += iter->sim * (rbp[k][i] - rbarray[k]);
			iter = iter->next;
		}
		pred[i]= ra + (pay/simc);
		i++;
	}k=0;
	float predtemp = 0; // en buyuk kabul edilen pred.
	char string[50];
	for(j=0;j<8;j++){
		if(iter2->points[j] == 0){
			if(pred[k] > predtemp){
				strcpy(string,iter2->bookname[j]);
				predtemp = pred[k];	
			}
			printf("\n%s - %.4f",iter2->bookname[j],pred[k]);
			k++;
		}
	}
	printf("\nOnerilen kitap ---> %s",string);
}
//---------------1. sik icin olan kisim;
//Similarityleri tuttugumuz struct arasinda karsilastirmalar yaparak similarity'si en buyuk olan K kisiyi yazdirir.
void mostsimilar(stsim *r,int k){
	stsim *temp=r;
	stsim *temp2;
	float sim;
	char user[30],target[30];
	int i,j;
	for(i=0;i<20;i++){
		temp2 = temp->next;
		for(j=i+1;j<20;j++){
			if(temp->sim < temp2->sim){
				sim = temp->sim;
				strcpy(user,temp->user);
				strcpy(target,temp->target);
				temp->sim = temp2->sim;
				strcpy(temp->user,temp2->user);
				strcpy(temp->target,temp2->target);
				temp2->sim = sim;
				strcpy(temp2->user,user);
				strcpy(temp2->target,target);
				
			}
			temp2 = temp2->next;
		}
		temp = temp->next;
	}
	temp=r;
	for(i=0;i<k;i++){
		printf("%s - %s - %.4f\n",temp->user,temp->target,temp->sim);
		temp = temp->next;
	}
}
//Kullanicidan reader name(NU) ve K degerlerini aldigim ana kisim.
//Excel dosyasinin ilk satirindan kitap isimleri daha sonrakilerden username ve kitaba gore puanlari aldim.
//Bunlari linkli listeye eklemek icin her seferinde user struct'u icin rootu yollayip createnode ile olusturma islemleri gerceklestirdim.
int main(int argc, char *argv[]) {
	user *root = NULL;
	stsim *root2 = NULL;
	FILE *fp;
	fp = fopen("RecommendationDataSet.csv","r");
	if(fp == NULL)
		printf("Dosya bulunamadi!");
	else{
		char readername[20];
		int kuser,k,j=0;
		printf("Reader name -->(NUx): ");
		scanf("%s",readername);
		printf("K: ");
		scanf("%d",&kuser);
		
		char *tick;
		char readert[20];
		int point[8],i=0;
		char csvf[512];
		char booknames[200];
		char booktemp[200];
		char booktemp2[200][200];
		fgets(booknames,200,fp);
		while(booknames[i] != ';'){
			i++;
		}
		i++;
		while(booknames[i] != '\n'){
			if(booknames[i] != ';'){
				booktemp[j] = booknames[i];
				j++;
			}else{
				strcpy(booktemp2[k],booktemp);
				k++;
				j=0;
				for(j=0;j<200;j++){
					booktemp[j] = '\0';
				}
				j=0;
			}
			i++;
		}
		strcpy(booktemp2[k],booktemp);
		while(fgets(csvf,512,fp)){
			tick = strtok(csvf,";");
			strcpy(readert,tick);
			int i=0;		
			while(tick != NULL){
				tick = strtok(NULL,";");				
				if (i < 8){
					if(tick == NULL){
						point[i] = 0;
					}
					else
						point[i] = atoi(tick);
				}				
				i++;
			}
			root = createnode(root,readert,booktemp2,point);
		}
		root2=sim(root,readername);
		mostsimilar(root2,kuser);
		recommend(root2,root,readername,kuser);
	}
	return 0;
}
