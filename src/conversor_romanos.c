#include <ctype.h>  //toupper()
#include <math.h>  //ceil()
#include <stdlib.h>
#include <string.h>
#include "conversor_romanos.h"

Tabela *kValores = NULL;  /**<Armazena a Tabela que relaciona caracter romano com seu valor arábico.*/
int kTamanhoValores = CriarTabela(&kValores);  /**<Armazena-se o tamanho da tabela para facilitar iteração.*/

/** Converte os algarismos de romano para arábico.
  \param romano ponteiro para char (atuando como string) contendo o número romano que se deseja converter.
  \return o valor inteiro do número romano. Retorna -1 se não for valor romano válido. Retorna 0 para string vazia.
 */
int Converter(char *romano)
{
  if(kValores == NULL)  //se não tiver a tabela, a função não tem como funcionar
    exit(4);  //fecha o programa

  if(!ValidarNumeroRomano(romano))
    return -1;

  unsigned int indice_letra = 0, soma = 0;
  while(indice_letra < strlen(romano) && romano[indice_letra] != '\0'){  //itera até o fim da string do número romano
    //Testa se está no fim antes (problemas de segmentation fault) e depois se ele se enquadra num caso de subtração
    if(!FimDaString(indice_letra+1, romano) && CasoEspecial(kValores, kTamanhoValores, romano[indice_letra], romano[indice_letra+1])){
      //caso se enquadre no caso, ele soma o valor do próximo(maior) menos o atual(menor)
      soma += FindAlgarismo(romano[indice_letra+1], kValores, kTamanhoValores) - FindAlgarismo(romano[indice_letra], kValores, kTamanhoValores);
      indice_letra++;  //pula 1 a mais por já ter processado duas letras de uma vez
    } else {
      //se for caso normal, apenas soma o valor correspondente
      soma += FindAlgarismo(romano[indice_letra], kValores, kTamanhoValores);
    }
    indice_letra++;  //itera para o proximo algarismo
  }

  return soma;  //o valor do romano é a soma de todos os valores
}

/** Aloca em memória uma lista com as relações entre todos os algarismos romanos com arábicos.
  \param relacao o endereço de memória de um ponteiro de Tabela.
  \return o tamanho da Tabela criada.
*/
int CriarTabela(Tabela **relacao)
{
  Tabela buffer_tabela[50];  //para criar a Tabela
  int valor, registro_buffer;
  char algarismo;
  
  /* Aqui tentamos abrir a Tabela de um .txt, caso não exista
   * ela chama uma função que gera essa Tabela
   */
  FILE *arquivo_relacao;
  arquivo_relacao = fopen("../valores.txt", "r");
  if(arquivo_relacao == NULL){  //se não houver arquivo
    FILE *numeros;
    numeros = fopen("../numeros_romanos.txt", "r");  //a partir de todos os romanos até 3999
    if(numeros == NULL)
      exit(2);
    DescobrirTabela(numeros, 3000);  //cria a Tabela
    fclose(numeros);
  }
  arquivo_relacao = fopen("../valores.txt", "r");  //tenta abrir de novo
  if(arquivo_relacao == NULL)
    exit(3);

  registro_buffer = 0;  //itera pelo buffer_tabela
  do{
    fscanf(arquivo_relacao, "%4d: %1c\n", &valor, &algarismo);  //le do arquivo valor e algarismo
    buffer_tabela[registro_buffer].valor = valor;  //associa valor
    buffer_tabela[registro_buffer].algarismo = algarismo;  //associa algarismo
    registro_buffer++;  //próximo registro
  }while(!feof(arquivo_relacao));  //até o fim do arquivo
  registro_buffer++;  //pra se equivaler ao tamanho

  *relacao = (Tabela*)malloc(sizeof(Tabela)*registro_buffer);  //gera o espaço para retorno por referencia
  int iterador_buffer;
  for(iterador_buffer = 0; iterador_buffer < registro_buffer; iterador_buffer++){  //copia todos os valores para o retorno
    (*relacao)[iterador_buffer].valor = buffer_tabela[iterador_buffer].valor;
    (*relacao)[iterador_buffer].algarismo = buffer_tabela[iterador_buffer].algarismo;
  }

  fclose(arquivo_relacao);
  return registro_buffer;   //retorna tamanho
}

/** Essa função gera um arquivo .txt relacionando o algarismo romano
  com o valor dele. Ele faz isso lendo a partir de uma Tabela com
  3999 números romanos e achando os que contém apenas 1 letra, tendo
  assim o valor verdadeiro do algarismo romano.
  \param fp Ponteiro para arquivo contendo o arquivo aberto com 3999 números romanos.
  \param maior_num Valor inteiro dizendo qual o maior número a ser olhado na lista.
  \return Nada.
*/
void DescobrirTabela(FILE *fp, int maior_num)
{
  int indice_linha, valor;
  char recebido[50];

  /* abre um arquivo para escrever a relação de algarismos e valores */
  FILE *arquivo_relacao;
  arquivo_relacao = fopen("../valores.txt", "w");
  if(arquivo_relacao == NULL)
    exit(1);

  /* Copia para o arquivo todos os números romanos que contém apenas um caracter,
   * que representa uma relação direta entre valor e algarismo
   */
  for(indice_linha = 0; indice_linha < maior_num; indice_linha++){
    fscanf(fp, "%4d: %49s", &valor, recebido);  //pega valor e algarismo
    if(strlen(recebido) == 1)  //checa se é apenas 1 caracter
      fprintf(arquivo_relacao, "%d: %s\n", valor, recebido);  //copia
  }

  fclose(arquivo_relacao);
}

/** Testa se o índice aponta para o fim da string.
  \param posicao Um valor inteiro representando o índice da string.
  \param romano Ponteiro para char atuando como uma string.
  \return Um inteiro representando verdadeiro ou falso.
*/
int FimDaString(int posicao, char *romano){return (unsigned int)posicao >= strlen(romano);}

/** Testa se o caracter atual e o próximo formam
  um caso de subtração dos algarismos romanos.
  Ele checa se o proximo tem valor maior que o anterior.
  \param relacao A Tabela relacionando algarismos romanos com os valores deles.
  \param tamanho_relacao Comprimento da Tabela.
  \param atual Caracter que vem antes.
  \param proximo Caracter que vem depois.
  \return Um inteiro representando verdadeiro ou falso.
*/
int CasoEspecial(Tabela *relacao, int tamanho_relacao, char atual, char proximo)
{
  int valor_atual = FindAlgarismo(atual, relacao, tamanho_relacao);
  int valor_prox =  FindAlgarismo(proximo, relacao, tamanho_relacao);
  return valor_atual < valor_prox;
}

/** Acha o valor correspondente ao algarismo romano passado.
  \param algarismo Algarismo da qual se quer o valor correspondente.
  \param relacao Tabela contendo as relações entre algarismo e valor.
  \param tamanho_relacao Comprimento da Tabela relacional.
  \return Retorna o valor do algarismo passado. Retorna -1 se não existir na Tabela.
*/
int FindAlgarismo(char algarismo, Tabela *relacao, int tamanho_relacao)
{
  int i = 0;
  algarismo = toupper(algarismo);
  while(i < tamanho_relacao && relacao[i].algarismo != algarismo)  //percorre a lista toda ou até achar o algarismo na lista.
    i++;
  if(relacao[i].algarismo == algarismo)  //testa se realmente é o algarismo procurado, poderia ter sido apenas o fim da lista.
    return relacao[i].valor;  //retorna o valor do algarismo.
  return -1;  //retorna -1 se não houver valor
}

/** Acha o valor correspondente ao algarismo romano passado.
  \param algarismo Algarismo da qual se quer o indice da posição correspondente.
  \param relacao Tabela contendo as relações entre algarismo e valor.
  \param tamanho_relacao Comprimento da Tabela relacional.
  \return Retorna o indice da posição do algarismo passado na tabela. Retorna -1 se não existir na Tabela.
*/
int FindIndice(char algarismo, Tabela *relacao, int tamanho_relacao)
{
  int indice_tabela = 0;
  algarismo = toupper(algarismo);
  while(indice_tabela < tamanho_relacao && relacao[indice_tabela].algarismo != algarismo)  //percorre a lista toda ou até achar o algarismo na lista.
    indice_tabela++;
  if(relacao[indice_tabela].algarismo == algarismo)  //testa se realmente é o algarismo procurado, poderia ter sido apenas o fim da lista.
    return indice_tabela;  //retorna a posição na tabela do algarismo.
  return -1;  //retorna -1 se não houver algarismo
}

/**
  Analiza uma string de números romanos para ver se é válido.
  \param romano ponteiro para char representando uma string de um número romano. Tamanho máximo do vetor: 31.
  \return 1 para romano válido, 0 para romano inválido.
*/
int ValidarNumeroRomano(char *romano)
{
  if(strlen(romano) == 0)
    return 1;

  char verificado[31], nao_verificado[31];
  char char_atual;
  int indice_atual;

  verificado[0] = '\0';
  strcpy(nao_verificado, romano);
  for(indice_atual = 0; (unsigned int)indice_atual < strlen(nao_verificado); indice_atual++){
    
    char_atual = nao_verificado[indice_atual];
    
    if(FindAlgarismo(char_atual, kValores, kTamanhoValores) < 0){  //Não faz parte dos algarismos romanos
      return 0;
    }

    //Teste para repetição:
    if(indice_atual - 1 >= 0 && verificado[indice_atual-1] == char_atual){//Testa primeira repetição
      if(FindIndice(char_atual, kValores, kTamanhoValores) % 2 == 1)  //Somente alguns algarismos podem repetir
        return 0;
      if(indice_atual - 2 >= 0 && indice_atual - 3 >= 0  //Garante que não acessará áreas proibidas da memória
        && verificado[indice_atual-2] == char_atual && verificado[indice_atual-3] == char_atual)  //só pode repetir 3 vezes
        return 0;
    }

    if(indice_atual - 1 >= 0  //Garante não acessar o -1
      && FindAlgarismo(verificado[indice_atual-1], kValores, kTamanhoValores)
      < FindAlgarismo(nao_verificado[indice_atual], kValores, kTamanhoValores))  //Anterior menor
    {
      if(FindIndice(verificado[indice_atual-1], kValores, kTamanhoValores)
        != ceil((FindIndice(nao_verificado[indice_atual], kValores, kTamanhoValores)-1)/2)*2)
          return 0;

      if(indice_atual - 2 >= 0
        && FindAlgarismo(verificado[indice_atual-2], kValores, kTamanhoValores)
        < FindAlgarismo(nao_verificado[indice_atual], kValores, kTamanhoValores))
          return 0;

    }

    if(indice_atual - 1 >= 0
      && FindAlgarismo(verificado[indice_atual-1], kValores, kTamanhoValores)
      > FindAlgarismo(nao_verificado[indice_atual], kValores, kTamanhoValores))
    {
      if(indice_atual - 2 >= 0
        && FindAlgarismo(verificado[indice_atual-2], kValores, kTamanhoValores)
        <= FindAlgarismo(nao_verificado[indice_atual], kValores, kTamanhoValores))
          return 0;
    }

    if(indice_atual - 2 >= 0
      && FindAlgarismo(verificado[indice_atual-2], kValores, kTamanhoValores)
       < FindAlgarismo(verificado[indice_atual-1], kValores, kTamanhoValores)
      && FindAlgarismo(nao_verificado[indice_atual], kValores, kTamanhoValores)
       >= FindAlgarismo(verificado[indice_atual-1], kValores, kTamanhoValores))
      return 0;

    verificado[indice_atual] = nao_verificado[indice_atual];
    verificado[indice_atual+1] = '\0';

  }


  return 1; 
}

/**
  \file
  Pacote de funções para Converter números romanos em arábicos.
*/