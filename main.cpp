#include <iostream>
#include <string>
#include <deque>
#include <stack>
#include <algorithm>

typedef struct Retangulo{
	int comprimento;
	int largura;

    int area(){
        return comprimento*largura;
    }
}Retangulo;

/**
* -Descricao: funcao auxiliar do metodo sort
* -Entrada: a, b
* -Saida: true, se a deve preceder b
*         false, caso contrario
*/
bool ordenaRetangulosDecrescenteArea(Retangulo a, Retangulo b){
	int areaA = a.comprimento * a.largura;
	int areaB = b.comprimento * b.largura;
    if(areaA > areaB){
        return true;
    }
	
    else if(areaA == areaB){
        if(a.largura > b.largura){
            return true;
        }
        else{
            return false;
        }
    }
    
	else{
        return false;
    }
}

/**
* -Descricao: encontra todas as areas possiveis ate o momento em que linha
*  e as linhas acima dela em planta foram processadas
* -Entrada: linha, M, retangulosLivres (referencia)
* -Saida: N/A
*/
void encontraAreasParciais(int linha[], int M, std::deque<Retangulo> &retangulosLivres){
    int topo, i;
    Retangulo areaLocal;
    areaLocal.comprimento = 0;
    areaLocal.largura = 0;

    //pilha que armazena indices de elementos de linha
    //armazenados os maiores componentes a cada visao local
    std::stack<int> pilha;
    
    i = 0;
    while (i < M){
        //se linha[i] for maior que o topo da pilha, linha[i] e o novo topo da pilha
        if (pilha.empty() || linha[pilha.top()] <= linha[i]){
            pilha.push(i++);
        }

        //se linha[i] eh menor que o topo da pilha, calcula-se a areaLocal conside-
        //rando-se o topo da pilha como o gargalo
        //i eh o limite da direta e o elemento que precede o topo da pilha eh o
        //limite da esquerda
        else{
            topo = pilha.top(); 
            pilha.pop();

            areaLocal.comprimento = linha[topo];

            if(pilha.empty()){
                areaLocal.largura = i;
            }

            else{
                areaLocal.largura = i - pilha.top() - 1;
            }

            if(areaLocal.comprimento != 0 && areaLocal.largura != 0){
                retangulosLivres.push_back(areaLocal);
            }
        }
    }
    
    //calculado a area local para cada componente restante na pilha
    while (!pilha.empty()){
        topo = pilha.top();
        pilha.pop();
        
        areaLocal.comprimento = linha[topo];

        if(pilha.empty()){
            areaLocal.largura = i;
        }
        else{
            areaLocal.largura = i - pilha.top() - 1;
        }

        if(areaLocal.comprimento != 0 && areaLocal.largura != 0){
            retangulosLivres.push_back(areaLocal);
        }       
    }
}

/**
* -Descricao: encontra e retorna todas as areas disponiveis em planta
* -Entrada: planta, N, M
* -Saida: retangulosLivres
*/
std::deque<Retangulo> encontraAreasDisponiveis(char** planta, int N, int M){
    int i,j;
    std::deque<Retangulo> retangulosLivres;
    retangulosLivres.clear();

    //criacao de uma matriz binaria, onde '.' -> 1 e '#' -> 0
    int matrizBinaria[N][M];
    for(i=0; i<N; i++){
        for(j=0; j<M; j++){
            if(planta[i][j] == '.'){
                matrizBinaria[i][j] = 1;
            }
            else{
                matrizBinaria[i][j] = 0;
            }
        }
    }

    encontraAreasParciais(matrizBinaria[0], M, retangulosLivres);

	//iterando por linhas
    for(i=1; i<N; i++){
        for(j=0; j<M; j++){
            if(matrizBinaria[i][j] == 1){
                matrizBinaria[i][j] += matrizBinaria[i-1][j];
            }
        }
        encontraAreasParciais(matrizBinaria[i], M, retangulosLivres);
    }
	
	return retangulosLivres;	
}

/**
* -Descricao: seleciona e retorna a mesa com maior area possivel 
*  e que caiba em um dos comodos da casa
* -Entrada: planta, N, M, mesas
* -Saida: a maior mesa possivel que caiba na maior area possivel
*/
Retangulo selecionaMaiorRetangulo(char** planta, int N, int M, std::deque<Retangulo> mesas){
	int i,j;
    std::deque<Retangulo> retangulosLivres;
    std::deque<Retangulo> mesasViaveis;

	retangulosLivres = encontraAreasDisponiveis(planta, N, M);

    std::sort(mesas.begin(), mesas.end(), ordenaRetangulosDecrescenteArea);
    std::sort(retangulosLivres.begin(), retangulosLivres.end(), ordenaRetangulosDecrescenteArea);

    //percorre o deque de mesas, buscando aquela de maior area possivel
    //que caiba em algum retangulo disponivel
	for(i=0; i<int(mesas.size()); i++){
        for(j=0; j<int(retangulosLivres.size()); j++){
            if(mesas[i].area() > retangulosLivres[j].area()){
                break;
            } 
            else if(mesas[i].comprimento <=  retangulosLivres[j].comprimento &&
                mesas[i].largura <=  retangulosLivres[j].largura){
                return mesas[i];
            }
            else if(mesas[i].comprimento <=  retangulosLivres[j].largura &&
                    mesas[i].largura <= retangulosLivres[j].comprimento){
                return mesas[i];
            }
            else{
                continue;
            }
        }
    }

    Retangulo mesaNull;
    mesaNull.comprimento = 0;
    mesaNull.largura = 0;

    return mesaNull;
}

/**
* -Descricao: funcao principal, processa entrada e chama metodos principais
* -Entrada: N/A
* -Saida: 0
*/
int main(){
    //variaveis auxiliares
    std::string buf, aux;
    int M, N, K, i, j;
	std::deque<Retangulo> mesas;

    //processando parametros de entrada
    getline(std::cin, buf);
    std::size_t pos = buf.find_first_of(" ");
    aux = buf;
    N = stoi(buf.erase(pos));
    aux = aux.substr(pos+1);
    M = stoi(aux);
    
    //alocacao dinamica de matriz que representa planta
    char **planta;
    planta = new char* [N];
    for(i=0; i<N; i++){ 
        planta[i] = new char[M];
    }

    //processando planta
    for(i=0; i<N; i++){
        getline(std::cin, buf);
        for(j=0; j<M; j++){
            planta[i][j] = buf[j];
        }
    }

	getline(std::cin, buf);
	K = stoi(buf);
	
	//processando lista de mesas
    for(i=0; i<K; i++){
		getline(std::cin, buf);
		
		Retangulo novaRetangulo;
		
		std::size_t pos = buf.find_first_of(" ");
		aux = buf;
		novaRetangulo.comprimento = stoi(buf.erase(pos));
		aux = aux.substr(pos+1);
		novaRetangulo.largura = stoi(aux);
		
		mesas.push_back(novaRetangulo);
	}
	
	//selecao da maior mesa possivel
	Retangulo mesaSelecionada;
	mesaSelecionada = selecionaMaiorRetangulo(planta, N, M, mesas);
	
	std::cout << std::endl << mesaSelecionada.comprimento << " " 
	<< mesaSelecionada.largura << std::endl;
	
	//desalocacao da matriz dinamica planta
    for(i=0; i<N; i++){ 
        delete(planta[i]);
    }
    delete(planta);

    return 0;
}