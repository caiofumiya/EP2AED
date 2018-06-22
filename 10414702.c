
////////////////////////////////////////////////////////////////
/////////       Caio Massaharu Damaceno Fumiya     /////////////
/////////       N°USP : 10414702                   /////////////
////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define ORDEM 3
//Arquivo de entrada
#define namefile "entrada.txt"


int cont = -1, count = 0;

typedef struct
{
  int chave;
  int ApPage[2];
  int rank;
} Registro;

typedef struct Pagina_str *Apontador;
typedef struct Pagina_str
{
  int n;
  int pageNum;
  int num;
  Registro r[2*ORDEM];
  Apontador p[2*ORDEM+1];
} Pagina;

struct Node{
    Registro info;
    struct Node *prox;
};
typedef struct Node node;

//##########################################################
void Antecessor(Apontador Ap, int Ind, Apontador ApPai, int *Diminuiu);
void Busca(Registro Reg, Apontador Ap);
void imprime(Apontador raiz);
void Insere(Registro Reg, Apontador *Ap);
void InsereNaPagina(Apontador Ap, Registro Reg, Apontador ApDir);
void Ins(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno);
node *busca(Registro info, node *LISTA);
void Retira(int Ch, Apontador *Ap);
void Ret(int Ch, Apontador *Ap, int *Diminuiu);
void Reconstitui(Apontador ApPag, Apontador ApPai, int PosPai, int *Diminuiu);
void saveAux(Apontador p, int Nivel);

typedef Apontador TipoDic;
void Inicializa(TipoDic *Dic)
{
  *Dic = NULL;
}  /* Inicializa a estrutura */

void inicia(node *LISTA)
{
    LISTA->prox = NULL;
}

/////////////////    Btree    /////////////////////////////////

void Antecessor(Apontador Ap, int Ind, Apontador ApPai, int *Diminuiu)
{
  if (ApPai->p[ApPai->n] != NULL)
  {
    Antecessor(Ap, Ind, ApPai->p[ApPai->n], Diminuiu);
    if (*Diminuiu)
      Reconstitui(ApPai->p[ApPai->n], ApPai, ApPai->n, Diminuiu);
    return;
  }
  Ap->r[Ind - 1] = ApPai->r[ApPai->n - 1];
  ApPai->n--;
  *Diminuiu = ApPai->n < ORDEM;
}  /* Antecessor */

void InsereNaPagina(Apontador Ap, Registro Reg, Apontador ApDir)
{

  int k;
  int NaoAchouPosicao;

  k = Ap->n;
  NaoAchouPosicao = k > 0;
  while (NaoAchouPosicao)
  {
    if (Reg.chave >= Ap->r[k - 1].chave)
    {
      NaoAchouPosicao = 0;
      break;
    }

    Ap->r[k] = Ap->r[k - 1];

    Ap->p[k + 1] = Ap->p[k];

    k--;
    if (k < 1)
      NaoAchouPosicao = 0;
  }
  Ap->r[k] = Reg;
  Ap->p[k + 1] = ApDir;
  Ap->n++;
}

void Busca(Registro Reg, Apontador Ap)
{
  int i;

  if (Ap == NULL) //
  {
    //printf("chave nao encontrada: %d\n", Reg.chave);
    return;
  }
  i = 1;
  while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
    i++;
  if (Reg.chave == Ap->r[i - 1].chave)
  {
    //printf("chave: %d \n", Reg.chave);
    return;
  }
  if (Reg.chave < Ap->r[i - 1].chave)
    Busca(Reg, Ap->p[i-1]);
  else
    Busca(Reg, Ap->p[i]);
}

void imprime(Apontador raiz)
{
  //printf("Contagem de chamadas \n");
  int i;
  if (raiz != NULL)
   {
     for (i = 0; i < raiz->n; i++)
      {
        imprime(raiz->p[i]);
        printf("%d ", raiz->r[i].chave);
      }
     imprime(raiz->p[i]);
    }

}

void fim(Apontador raiz,FILE *fp)
{
  int i;
  if (raiz != NULL)
   {
     for (i = 0; i < raiz->n; i++)
      {
        fim(raiz->p[i],fp);
        fprintf(fp,"%i ",raiz->r[i].chave);
      }
     fim(raiz->p[i],fp);
    }

}

void Reconstitui(Apontador ApPag, Apontador ApPai, int PosPai, int *Diminuiu)
{
  Apontador Aux;
  int DispAux, j;

  if (PosPai < ApPai->n) {  /* Aux = Pagina a direita de ApPag */
    Aux = ApPai->p[PosPai + 1];
    DispAux = (Aux->n - ORDEM + 1) / 2;
    ApPag->r[ApPag->n] = ApPai->r[PosPai];
    ApPag->p[ApPag->n + 1] = Aux->p[0];
    ApPag->n++;
    if (DispAux > 0) {  /* Existe folga: transfere de Aux para ApPag */
      for (j = 1; j < DispAux; j++)
        InsereNaPagina(ApPag, Aux->r[j - 1], Aux->p[j]);
      ApPai->r[PosPai] = Aux->r[DispAux - 1];
      Aux->n -= DispAux;
      for (j = 0; j < Aux->n; j++)
        Aux->r[j] = Aux->r[j + DispAux];
      for (j = 0; j <= Aux->n; j++)
        Aux->p[j] = Aux->p[j + DispAux];
      *Diminuiu = 0;
    }
    else
    { /* Fusao: intercala Aux em ApPag e libera Aux */
      for (j = 1; j <= ORDEM; j++)
        InsereNaPagina(ApPag, Aux->r[j - 1], Aux->p[j]);
      free(Aux);
      for (j = PosPai + 1; j < ApPai->n; j++)
      {   /* Preenche vazio em ApPai */
        ApPai->r[j - 1] = ApPai->r[j];
        ApPai->p[j] = ApPai->p[j + 1];
      }
      ApPai->n--;
      if (ApPai->n >= ORDEM)
        *Diminuiu = 0;
    }
  }
  else
  { /* Aux = Pagina a esquerda de ApPag */
    Aux = ApPai->p[PosPai - 1];
    DispAux = (Aux->n - ORDEM + 1) / 2;
    for (j = ApPag->n; j >= 1; j--)
      ApPag->r[j] = ApPag->r[j - 1];
    ApPag->r[0] = ApPai->r[PosPai - 1];
    for (j = ApPag->n; j >= 0; j--)
      ApPag->p[j + 1] = ApPag->p[j];
    ApPag->n++;
    if (DispAux > 0) {  /* Existe folga: transfere de Aux para ApPag */
      for (j = 1; j < DispAux; j++)
        InsereNaPagina(ApPag, Aux->r[Aux->n - j], Aux->p[Aux->n - j + 1]);
      ApPag->p[0] = Aux->p[Aux->n - DispAux + 1];
      ApPai->r[PosPai - 1] = Aux->r[Aux->n - DispAux];
      Aux->n -= DispAux;
      *Diminuiu = 0;
    }
    else
    {  /* Fusao: intercala ApPag em Aux e libera ApPag */
      for (j = 1; j <= ORDEM; j++)
        InsereNaPagina(Aux, ApPag->r[j - 1], ApPag->p[j]);
      free(ApPag);
      ApPai->n--;
      if (ApPai->n >= ORDEM)
        *Diminuiu = 0;
    }
  }
}  /* Reconstitui */

void Ins(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno)
{
  Apontador ApTemp;
  Registro Aux;
  int i, j;

  if (Ap == NULL)
  {
    *Cresceu = 1;
    *RegRetorno = Reg;
    *ApRetorno = NULL;
    return;
  }
  i = 1;
  while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
    i++;
  if (Reg.chave == Ap->r[i - 1].chave)
  {
    //printf("chave ja existente: %d \n", Reg.chave);
    *Cresceu = 0;
    return;
  }
  if (Reg.chave < Ap->r[i - 1].chave)
    Ins(Reg, Ap->p[i-1], Cresceu, RegRetorno, ApRetorno);
  else
    Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);
  if (!*Cresceu)
    return;
  if (Ap->n < 2*ORDEM)
  {  /* Verifica se tem espaço */
    InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    *Cresceu = 0;
    return;
  }
  /* Divide pagina */
  ApTemp = (Apontador) malloc(sizeof(Pagina));
  ApTemp->n = 0;
  ApTemp->p[0] = NULL;
  cont++;
  ApTemp->pageNum = cont;

  if (i <= ORDEM + 1)
  {
    InsereNaPagina(ApTemp, Ap->r[2*ORDEM - 1], Ap->p[2*ORDEM]);
    Ap->n--;
    InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
  }
  else{
    InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
    }
  for (j = ORDEM + 2; j <= 2*ORDEM; j++)
    InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);


  Ap->n = ORDEM;
  ApTemp->p[0] = Ap->p[ORDEM + 1];
  *RegRetorno = Ap->r[ORDEM];
  *ApRetorno = ApTemp;
 for (j = ORDEM; j < Ap->n+2; j++){
    Aux.chave = 0;
    Aux.rank = 0;
    Ap->r[j] = Aux;
        }
}  /*Ins*/

void Insere(Registro Reg, Apontador *Ap)
{
  int Cresceu;
  Registro RegRetorno;
  Apontador ApRetorno;
  Apontador ApTemp;

  Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
  if (Cresceu) { /* Se arvore cresce na altura pela raiz */
    ApTemp = (Apontador) malloc(sizeof(Pagina));
    ApTemp->n = 1;
    ApTemp->r[0] = RegRetorno;
    ApTemp->p[1] = ApRetorno;
    cont++;
    ApTemp->pageNum = cont;
    ApTemp->p[0] = *Ap;
    *Ap = ApTemp;
    //save in file
  }
  saveAux(*Ap, 2*ORDEM);
}  /*Insercao*/

void Pesquisa(Registro *x, Apontador Ap)
{
  int i;

  if (Ap == NULL)
  {
    //printf("Erro: Registro nao existe\n");

    return;
  }
  i = 1;
  while (i < Ap->n && x->chave > Ap->r[i - 1].chave)
    i++;
  if (x->chave == Ap->r[i - 1].chave)
  {
    *x = Ap->r[i - 1];
    return;
  }
  if (x->chave < Ap->r[i - 1].chave)
    Pesquisa(x, Ap->p[i - 1]);
  else
    Pesquisa(x, Ap->p[i]);
} /* Pesquisa */

void Ret(int Ch, Apontador *Ap, int *Diminuiu)
{
  int Ind, j;
  Apontador WITH;
  Registro Reg;

  if (*Ap == NULL)
  {
    //printf("chave nao encontrada: %i\n", Ch);
    *Diminuiu = 0;
    return;
  }
  WITH = *Ap;
  Ind = 1;
  while (Ind < WITH->n && Ch > WITH->r[Ind - 1].chave)
    Ind++;
  if (Ch == WITH->r[Ind - 1].chave)
  {
    Reg.chave = 0;
    Reg.rank = 0;
    WITH->r[Ind -1] = Reg;
    if (WITH->p[Ind - 1] == NULL) {  /* Pagina folha */
      WITH->n--;
      *Diminuiu = WITH->n < ORDEM;
      for (j = Ind; j <= WITH->n; j++)
      {
        WITH->r[j - 1] = WITH->r[j];
        WITH->p[j] = WITH->p[j + 1];
      }
      return;
    }
    Antecessor(*Ap, Ind, WITH->p[Ind - 1], Diminuiu);
    if (*Diminuiu)
      Reconstitui(WITH->p[Ind - 1], *Ap, Ind - 1, Diminuiu);
    return;
  }
  if (Ch > WITH->r[Ind - 1].chave)
    Ind++;
  Ret(Ch, &WITH->p[Ind - 1], Diminuiu);
  if (*Diminuiu)
    Reconstitui(WITH->p[Ind - 1], *Ap, Ind - 1, Diminuiu);
}  /* Ret */

void Retira(int Ch, Apontador *Ap)
{
  int Diminuiu;
  Apontador Aux;

  Ret(Ch, Ap, &Diminuiu);
  if (Diminuiu && (*Ap)->n == 0) { /* Arvore diminui na altura */
    Aux = *Ap;
    *Ap = Aux->p[0];
    free(Aux);

  }

}  /* Retira */

void saveAux(Apontador p, int Nivel)
{
  int i,j;

  if (p == NULL)
    return;
  for (j = 0; j <= p->n; j++)
    saveAux(p->p[j], Nivel + 1);
}

/////////////////      Btree      /////////////////////

int main()
{
  Apontador *arv;
  Registro reg;
  //char tecla;
  int chave, i, num = 0;
  arv=(Apontador*) malloc(sizeof(Apontador));
  node *LISTA = (node *) malloc(sizeof(node));
  Inicializa(arv);
  inicia(LISTA);
  FILE *arq;
  FILE *saida;
  char comando[]="";

  saida = fopen("saida.txt","w");
  arq = fopen(namefile, "r");
  if(arq == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");

  while( (fscanf(arq,"%s",comando))!=EOF ){

    if(strcmp(comando,"imprime")==0){
            imprime(*arv);
            printf("\n");
    }
    else{

          if(strcmp(comando,"insere")==0){
            fscanf(arq,"%d",&chave);
            reg.chave = chave;
            count++;
            reg.rank = count;
            if (reg.chave <= 0){
              count--;
              break;
            }
            Insere(reg,arv);
          }
          else{ 

              if(strcmp(comando,"remove")==0){

                fscanf(arq,"%d",&chave);
                reg.chave = chave;
                Retira(reg.chave, arv);
                saveAux(*arv,2*ORDEM);
                }
              else{
                
                if(strcmp(comando,"fim")==0){
                  fim(*arv,saida);
                }
              } 
            }  
    }
  }
}
  // Apenas para teste de mesa 


  /*while(1)
  {
    scanf("%c", &tecla);
    if (tecla=='e')
      break;
    switch(tecla)
    {

      ////////////////////     Insere
      case 'i':
           scanf("%d", &chave);
           reg.chave = chave;
           count++;
           reg.rank = count;
           getchar();
           if (reg.chave <= 0){
               count--;
            break;
            }
            Insere(reg,arv);
      break;

      //////////////////////      Remove
      case 'r':
           scanf("%d", &chave);
           reg.chave = chave;
           Retira(reg.chave, arv);
            saveAux(*arv,2*ORDEM);
      break;

      //////////////////////      Imprime
      case 'o':
            imprime(*arv);
      break;

  }
  return 0;
  }
*/
//##############################################