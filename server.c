#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
int index_token=0;
struct result{
	int per;
	char ans[1000];
}r;
char *toLower(char *str){
	char *s;
	s=str;
	while(*s!='\0'){
		if(*s>64 && *s<92){
			*s=*s+32;
		}
		s++;
	}
	return str; 
}
int compare_tokens(char** u_tokens ,int u_index,char** f_tokens,int f_index){
	int per,i,j,t=0,max=0,min=0;
	int result[u_index];

	for(i=0;i<u_index;i++)
		for(j=0;j<f_index;j++){
			result[i]=(!strcmp(toLower(u_tokens[i]),toLower(f_tokens[j])))?1:0;
			if(result[i]==1){
				t++;
				break;
			}
		}

	per=(int)((t/(float)u_index)*100);
	return per;
}
char** tokenizer(char *str){
	int i=0,c=0;
	char **tokens,*s=NULL,*word,*ch;
	s=str;
	tokens=(char**)malloc(sizeof(char)*100);
	*tokens=(char*)malloc(sizeof(char)*100);
	word=(char*)malloc(sizeof(char)*50);
	while(*s!='\0'){
		//removing ? from string replace with space
		if(*s=='?'){
			*s=' ';
		}
		ch=s+1;
		if(*s==' ' || *ch=='\0'){
				if(*ch=='\0')
					word[c++]=*s;
			while(*s==' '){
				if(*(s+1)!=' ')
				break;
				s++;
			}
			word[c]='\0';
			tokens[i]=word;
			word=(char*)malloc(sizeof(char)*50);
			i++;
			c=0;
		}
		else{
			word[c++]=*s;
		}
		s++;
	}
		
	index_token=i;
	return tokens;

}
char* getOutput(char *str){
	FILE *f=NULL;
	int i=0,c=0,u_index,f_index,per,count=0,d=0,len=0;
	char ch;
	char *que=NULL,ans[1000],**u_tokens,**f_tokens;
	//for removeing \n from use que
	len=strlen(str);
	len=len-1;
	if(str[len]=='\n')
		str[len]='\0';

	u_tokens=tokenizer(str);
	u_index=index_token;
	index_token=0;
	f=fopen("gk_qna.txt","r");
	if(f==NULL)
		return "Internal problem";
	que=(char*)malloc(sizeof(char)*250);
	while((ch=fgetc(f))!=EOF){
		if(ch!='\n'){
			que[c++]=ch;
		}
		else{
			que[c]='\0';
			f_tokens=tokenizer(que);
			f_index=index_token;
			index_token=0;
			per=compare_tokens(u_tokens ,u_index,f_tokens,f_index);
			while((ch=fgetc(f))!='\n'){
				if(ch==EOF)
					break;
				ans[d++]=ch;
			}
			if(ch==EOF)
				break;
			ans[d]='\0';
			if(r.per<per){
				r.per=per;
				strcpy(r.ans,ans);
				if(per==100)
					break;
			}
			c=0;
			d=0; 
		}
	}
	printf("\npercentage of que match :   %d\n",r.per);
	if(r.per>80)
	return r.ans;
	return "Sry Ans not found "	;
}
int main(){
	int fd = 0;
	r.per=0;
	char buff[1024];
	char nbuff[1024];
	char *ans=NULL;
	//Setup Buffer Array
	memset(buff, '0',sizeof(buff));	
	//Create Socket
	fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
		perror("Client Error: Socket not created succesfully");
		return 0;
	}
	//Structure to store details
	struct sockaddr_in server; 
	memset(&server, '0', sizeof(server)); 
	//Initialize	
	server.sin_family = AF_INET;
	server.sin_port = htons(10011); 
    server.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(fd, (struct sockaddr*)&server, sizeof(server)); 
	int in;
	listen(fd, 10); 
	while(in = accept(fd, (struct sockaddr*)NULL, NULL)){		
		int childpid,n;
		if ( (childpid = fork ()) == 0 ) {
			close (fd);
			//Clear Zeroes
			bzero(buff,256);
			bzero(nbuff,256);			
			while ((n = recv(in, buff, 256,0)) > 0)  {
				ans=NULL;
				bzero(nbuff,256);
				char *nbuff=getOutput(buff);
    			send(in,nbuff,strlen(nbuff),0);	
				r.per=0;
				strcpy(r.ans,"");						
			}
			close(in);
			exit(0);
		}
	}
	return 0;
}
