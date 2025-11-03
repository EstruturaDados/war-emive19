#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    Estrutura: Territorio
    - nome: nome do territorio
    - cor: cor do exercito que controla o territorio
    - tropas: quantidade de tropas
*/
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

/*
    Estrutura: Jogador
    - cor: cor do jogador
    - missao: ponteiro para a string da missão (alocada dinamicamente)
*/
typedef struct {
    char cor[10];
    char *missao; // alocada dinamicamente
} Jogador;

/* ---------- Protótipos das funções (modularização) ---------- */
Territorio* alocarMapa(int quantidade);
void cadastrarTerritorios(Territorio *mapa, int quantidade);
void exibirMapa(Territorio *mapa, int quantidade);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMapa(Territorio *mapa);
void atribuirMissao(char **destino, char *missoes[], int totalMissoes);
void exibirMissao(const char *missao); // passagem por valor (apenas leitura)
int verificarMissao(const char *missao, Territorio *mapa, int tamanho, const char *corJogador);
void liberarJogadores(Jogador *jogadores, int numJogadores);

/* ---------------- Implementações ---------------- */

Territorio* alocarMapa(int quantidade) {
    // calloc inicializa memória com zeros (boa prática)
    Territorio *mapa = (Territorio *)calloc(quantidade, sizeof(Territorio));
    if (!mapa) {
        printf("Erro: falha na alocacao de memoria para o mapa.\n");
        exit(1);
    }
    return mapa;
}

void cadastrarTerritorios(Territorio *mapa, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        printf("\n--- Cadastro do territorio %d ---\n", i);
        printf("Nome: ");
        scanf(" %[^\n]", mapa[i].nome); // lê até newline, aceita espaços

        printf("Cor do exercito: ");
        scanf(" %[^\n]", mapa[i].cor);

        printf("Quantidade de tropas: ");
        scanf("%d", &mapa[i].tropas);
    }
}

void exibirMapa(Territorio *mapa, int quantidade) {
    printf("\n=== MAPA ATUAL (%d territorios) ===\n", quantidade);
    for (int i = 0; i < quantidade; i++) {
        printf("[%d] Nome: %s | Cor: %s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/*
    Função de ataque:
    - usa rand() para simular um dado (1..6) para atacante e defensor
    - se atacante vencer: defensor muda de cor e recebe metade das tropas do atacante
    - se defensor vencer ou empatar: atacante perde 1 tropa (se > 0)
    OBS: recebe ponteiros para os territórios e os modifica diretamente.
*/
void atacar(Territorio *atacante, Territorio *defensor) {
    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;

    printf("\nBATALHA: %s (%s) vs %s (%s)\n",
           atacante->nome, atacante->cor,
           defensor->nome, defensor->cor);
    printf("Dado Atacante: %d  |  Dado Defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("Ataque bem-sucedido! %s conquista %s.\n", atacante->nome, defensor->nome);
        // defensor muda de dono e recebe metade das tropas do atacante
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = atacante->tropas / 2;
        // opcional: reduzir atacante (manter como estava nas regras que você pediu)
    } else {
        printf("Ataque falhou. %s perde 1 tropa.\n", atacante->nome);
        if (atacante->tropas > 0) atacante->tropas--;
    }
}

/* Libera memória do mapa (vetor de Territorio) */
void liberarMapa(Territorio *mapa) {
    free(mapa);
}

/*
    Atribui uma missão aleatória a *destino.
    - destino: ponteiro para char* (passagem por referência) para permitir
      que a função aloque memória e retorne o ponteiro.
    - missoes: vetor de C-strings disponíveis
*/
void atribuirMissao(char **destino, char *missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    size_t len = strlen(missoes[idx]);
    *destino = (char *)malloc(len + 1);
    if (!*destino) {
        printf("Erro: falha na alocacao de memoria para missao.\n");
        exit(1);
    }
    strcpy(*destino, missoes[idx]);
}

/* Exibe a missão (passagem por valor: apenas leitura da string) */
void exibirMissao(const char *missao) {
    printf("\nSua missao: %s\n", missao);
}

/*
    Verifica se a missão do jogador foi cumprida.
    - missao: C-string com a descrição da missão
    - mapa: vetor de territórios
    - tamanho: número de territórios
    - corJogador: cor do jogador (usada para avaliar se ele cumpriu a missão)

    Implementa regras simples (exemplos):
      1) "Conquistar 3 territorios seguidos" -> procura 3 territorios consecutivos com corJogador
      2) "Controlar 3 territorios no total"   -> contar territorios com corJogador >=3
      3) "Ter pelo menos 10 tropas no total"  -> somar tropas do jogador >=10
      4) "Eliminar todas as tropas inimigas"  -> soma tropas de cores diferentes == 0
      5) "Conquistar o territorio 0"         -> verificar se territorio[0] pertence ao jogador
    Retorna 1 se cumprida, 0 caso contrário.
*/
int verificarMissao(const char *missao, Territorio *mapa, int tamanho, const char *corJogador) {
    // 1) Conquistar 3 territorios seguidos
    if (strstr(missao, "Conquistar 3 territorios seguidos") != NULL) {
        int consec = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                consec++;
                if (consec >= 3) return 1;
            } else {
                consec = 0;
            }
        }
        return 0;
    }

    // 2) Controlar 3 territorios no total
    if (strstr(missao, "Controlar 3 territorios no total") != NULL) {
        int cnt = 0;
        for (int i = 0; i < tamanho; i++) if (strcmp(mapa[i].cor, corJogador) == 0) cnt++;
        return (cnt >= 3) ? 1 : 0;
    }

    // 3) Ter pelo menos 10 tropas no total
    if (strstr(missao, "Ter pelo menos 10 tropas no total") != NULL) {
        int soma = 0;
        for (int i = 0; i < tamanho; i++) if (strcmp(mapa[i].cor, corJogador) == 0) soma += mapa[i].tropas;
        return (soma >= 10) ? 1 : 0;
    }

    // 4) Eliminar todas as tropas inimigas
    if (strstr(missao, "Eliminar todas as tropas inimigas") != NULL) {
        int somaInimiga = 0;
        for (int i = 0; i < tamanho; i++) if (strcmp(mapa[i].cor, corJogador) != 0) somaInimiga += mapa[i].tropas;
        return (somaInimiga == 0) ? 1 : 0;
    }

    // 5) Conquistar o territorio 0 (exemplo simples)
    if (strstr(missao, "Conquistar o territorio 0") != NULL) {
        if (tamanho > 0 && strcmp(mapa[0].cor, corJogador) == 0) return 1;
        return 0;
    }

    // Missão desconhecida -> não cumprida
    return 0;
}

/* Libera missões alocadas e zera ponteiros */
void liberarJogadores(Jogador *jogadores, int numJogadores) {
    for (int i = 0; i < numJogadores; i++) {
        if (jogadores[i].missao) {
            free(jogadores[i].missao);
            jogadores[i].missao = NULL;
        }
    }
}

/* ------------------------ MAIN ------------------------ */

int main() {
    srand((unsigned int)time(NULL)); // inicializa RNG

    int numTerritorios;
    printf("Digite o numero de territorios do mapa: ");
    scanf("%d", &numTerritorios);

    if (numTerritorios <= 0) {
        printf("Numero invalido de territorios.\n");
        return 1;
    }

    // 1) Aloca mapa dinamicamente
    Territorio *mapa = alocarMapa(numTerritorios);

    // 2) Cadastro dos territórios (entradas do usuário)
    cadastrarTerritorios(mapa, numTerritorios);

    // 3) Cria missões possíveis (vetor de strings literais)
    char *missoesDisponiveis[] = {
        "Conquistar 3 territorios seguidos",
        "Controlar 3 territorios no total",
        "Ter pelo menos 10 tropas no total",
        "Eliminar todas as tropas inimigas",
        "Conquistar o territorio 0"
    };
    int totalMissoes = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);

    // 4) Número de jogadores (para este exemplo, permitimos 2 a 4 jogadores)
    int numJogadores;
    printf("\nDigite o numero de jogadores (2 a 4): ");
    scanf("%d", &numJogadores);
    if (numJogadores < 2 || numJogadores > 4) {
        printf("Numero de jogadores invalido. Usando 2 jogadores por padrao.\n");
        numJogadores = 2;
    }

    // 5) Aloca vetor de jogadores (estatico no stack mas com missao dinamica)
    Jogador *jogadores = (Jogador *)malloc(numJogadores * sizeof(Jogador));
    if (!jogadores) {
        printf("Erro: falha na alocacao de memoria para jogadores.\n");
        liberarMapa(mapa);
        return 1;
    }

    // 6) Para cada jogador: pede cor e atribui missão dinamicamente
    for (int i = 0; i < numJogadores; i++) {
        printf("\n--- Configuracao do Jogador %d ---\n", i + 1);
        printf("Digite a cor do jogador %d: ", i + 1);
        scanf(" %[^\n]", jogadores[i].cor);

        jogadores[i].missao = NULL; // inicializa
        atribuirMissao(&jogadores[i].missao, missoesDisponiveis, totalMissoes);

        printf("Missao sorteada para Jogador %d (cor %s):\n", i + 1, jogadores[i].cor);
        exibirMissao(jogadores[i].missao); // exibir apenas 1 vez (requisito)
    }

    // 7) Loop principal de turnos (menu simples)
    int continuar = 1;
    while (continuar) {
        exibirMapa(mapa, numTerritorios);

        printf("\nEscolha uma acao:\n");
        printf("1 - Realizar ataque\n");
        printf("2 - Verificar missoes agora\n");
        printf("3 - Encerrar jogo\n");
        printf("Opcao: ");
        int opc;
        scanf("%d", &opc);

        if (opc == 1) {
            // Ataque: escolhe indices
            int idxAtacante, idxDefensor;
            printf("Indice do territorio atacante: ");
            scanf("%d", &idxAtacante);
            printf("Indice do territorio defensor: ");
            scanf("%d", &idxDefensor);

            // Valida indices
            if (idxAtacante < 0 || idxAtacante >= numTerritorios ||
                idxDefensor < 0 || idxDefensor >= numTerritorios) {
                printf("Indice(es) invalido(s). Tente novamente.\n");
                continue;
            }

            // Não pode atacar seu proprio territorio
            if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                printf("Erro: nao e possivel atacar um territorio da mesma cor.\n");
                continue;
            }

            // Realiza ataque (passagem por ponteiro)
            atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

            // Após ataque, verificar missoes silenciosamente
            int vencedoresEncontrados = 0;
            for (int p = 0; p < numJogadores; p++) {
                if (verificarMissao(jogadores[p].missao, mapa, numTerritorios, jogadores[p].cor)) {
                    printf("\n>>> O JOGADOR %d (cor %s) cumpriu sua missao: %s\n",
                           p + 1, jogadores[p].cor, jogadores[p].missao);
                    vencedoresEncontrados = 1;
                }
            }
            if (vencedoresEncontrados) {
                printf("Jogo finalizado por cumprimento de missao.\n");
                break;
            }

        } else if (opc == 2) {
            // Verifica as missões agora (exibição condicional)
            int algumCumpriu = 0;
            for (int p = 0; p < numJogadores; p++) {
                if (verificarMissao(jogadores[p].missao, mapa, numTerritorios, jogadores[p].cor)) {
                    printf("\n>>> O JOGADOR %d (cor %s) cumpriu sua missao: %s\n",
                           p + 1, jogadores[p].cor, jogadores[p].missao);
                    algumCumpriu = 1;
                }
            }
            if (algumCumpriu) {
                printf("Jogo finalizado por cumprimento de missao.\n");
                break;
            } else {
                printf("Ninguem ainda cumpriu a missao.\n");
            }

        } else if (opc == 3) {
            printf("Encerrando o jogo por escolha do usuario.\n");
            break;
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    // 8) Limpeza de memoria
    liberarJogadores(jogadores, numJogadores);
    free(jogadores);
    liberarMapa(mapa);

    printf("Memoria liberada. Saindo...\n");
    return 0;
}
