#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define N 200 //numero de nós
#define M 4 //numero de comunidade
#define K 16 //numero de grau medio
#define PIN 0.9 //numero de proabilidade de intra-comunidade
#define POUT 0.0 //numero de proabilidade de inter-comunidade
#define GNUPLOT "gnuplot -persist"  //plotar grafico

//preenche a matriz com um valor passado
int fillMatrix(int x[N][N], int y,int value){
	for(int i=0; i<y; i++){
		for(int j=0; j<y; j++){
			x[i][j] = value;
		}
	}
}

//exibe a matriz
void printMatrix(int x[N][N],int y){
	for(int i=0; i<y; i++){
		for(int j=0; j<y; j++){
			if (x[i][j] == 1){
				printf(" * ");
			}
			else{
				printf(" . ");
			}
		}
		printf("\n");
	}
}

//encontra a comunidade de algum nó aleatorio
int findCommunity(int random,int size){
	int community,begin_c,end_c;
	if ((random >= 0) && (random < size)){
		return 1;
	}
	else {
		community = 2;
		begin_c = size-1;
		end_c = (size*2)-1;

		while(true){
			if((random > begin_c) && (random <= end_c)){
				return community;
			}
			else{
				begin_c = begin_c + size;
				end_c = end_c + size;
				community++;
			}
		}
	}
}

//encontra um nó em uma determinada comunidade
int findNode(int community,int size){
	int step;
	step = rand() % size;
	return (((community*size)-size) + step);
}

//encontra o valor do grau medio do grafo
int mediumDegree(int x[N][N],int y){
	int mediumdegree = 0;
	for(int i = 0;i < y;i++){
		for(int j = 0; j < y;j++){
			if (x[i][j] == 1){
				mediumdegree++;
			} 
		}
	}
	return (2*mediumdegree)/y;
}

//inicio do algoritmo
void main(){
	srand(time(NULL));
	int G[N][N],C[N],medium_degree;
	int size = N/M;
	int random1,random2,random_community,random_node1,random_node2,community_random1,community_random2;
	float probability_pin = PIN,probability_pout = POUT,random_pin,random_pout;

	fillMatrix(G,N,0);//preenche a mtriz G com 0
	medium_degree = mediumDegree(G,N);//retorna o valor do grau medio do grafo

	while(medium_degree < K){
		//-------------------faz as ligacoes intra-comunidade---------------------------------

		random_community = rand() % M+1;//gera alguma comunidade aleatoria
		random_node1 = findNode(random_community,size);//gera um vertice aleatorio em uma comunidade
		random_node2 = findNode(random_community,size);//gera um vertice aleatorio em uma comunidade
		random_pin = ((rand()%100)/100.0);//cria um numero aleatorio entre 0 e 0.9
		
		//se houver probabiliade PIn de se conectarem e nao houver conexao, entao se conectam
		if ((probability_pin > random_pin) && (G[random_node1][random_node2]==0)){
			G[random_node1][random_node2]=1;//vertices conectados
		}
		//------------------------------------------------------------------------------------------


		//-------------------faz as ligacoes inter-comunidade---------------------------------

		community_random1 = 0;//atribui 0 para entrar no while
		community_random2 = 0;//atribui 0 para entrar no while
		while(community_random1 == community_random2){//garante que os vertices serao de comunidades diferentes
			random1 = rand() % N;//cria um numero aleatorio entre 0 e N
			random2 = rand() % N;//cria um numero aleatorio entre 0 e N
			community_random1 = findCommunity(random1,size);//encontra a comunidade desse vertice aleatorio
			community_random2 = findCommunity(random2,size); //encontra a comunidade desse vertice aleatorio
		}
		random_pout = ((rand()%100)/100.0);//cria um numero aleatorio entre 0 e 0.9

		//se houver probabiliade POut de se conectarem e nao houver conexao, entao se conectam
		if ((probability_pout > random_pout) && (G[random1][random2]==0)){
			G[random1][random2]=1;//vertices conectados
		}
		//------------------------------------------------------------------------------------------
		medium_degree = mediumDegree(G,N);//retorna o grau medio do grafo, para ser usado no while
		printf("Grau medio do grafo: %d\n",medium_degree);
	}
	//printMatrix(G,N);//printa a matriz de adjacencia

	FILE *arq;//cria ponteiro para gerar txt de saida
    arq = fopen ("plot_data.txt", "w");//gera o arquivo
    fprintf(arq,"# x \t f(x) \n");//escreve o cabecalho no txt

    for (int i = 0;i < N;i++){
    	for (int j = 0;j < N;j++){
    		if (G[i][j] == 1){
    			fprintf(arq,"%d \t %d \n",i,j);//salva as coordenadas das ligacoes em um txt para o grafico
    		} 
    	}
    }
    fclose(arq);//fecha ponteiro do txt de saida

	int resp;//variavel respota
    printf("\nDeseja plotar o grafico? Insira 1 para sim, 0 para nao: ");//pergunta
    scanf("%d",&resp);//le a resposta e guarda na variavel
    if (resp == 1){//se a resposta for sim, entao:
        FILE *gp;//cria ponteiro
        gp = popen(GNUPLOT, "w");//executa comando no terminal 'gnuplot -persist'
        if (gp == NULL) {//caso nao rode, exibir messagem de erro
            printf("Erro ao abrir pipe para o GNU plot.\n" //mensagem de erro
                   "Instale com 'sudo apt-get install gnuplot'.\n");
            exit(0);//termina execucao
        }//mas caso rode, ira executar o codigo abaixo:
        fprintf(gp, "set title 'Matriz de Adjacência [%d][%d]; M = %d; K = %d; POut = %.2f; PIn = %.2f' \n",N,N,M,K,POUT,PIN);//escreve o titulo do grafico
        fprintf(gp, "set xlabel 'Nós'\n");//altera legenda do eixo x
        fprintf(gp, "set ylabel 'Nós'\n");//altera legenda do eixo y
        //fprintf(gp, "set mytics\n");//coloca reguinha no eixo y
        fprintf(gp, "set yrange [%d:0]\n",N); //intervalo do eixo y
        fprintf(gp, "set xrange [0:%d]\n",N); //intervalo do eixo x
        //fprintf(gp, "set grid\n");//coloca um gride no fundo do grafico
        fprintf(gp, "plot 'plot_data.txt' t '' with points lc 8\n");//plota o grafico, 
        //t eh titulo e o with lines é pra ser um grafico de linhas, 'lc 6' para cor azul
        fclose(gp);//fecha o ponteiro
	}
}
