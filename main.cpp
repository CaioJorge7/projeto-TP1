/**
 * @file main.cpp
 * @brief Ponto de entrada do sistema Kanban - TP1.
 *
 * Responsabilidades deste arquivo:
 *  1. Instanciar os quatro conteineres em memoria (ContainerPessoas,
 *     ContainerProjetos, ContainerQuadros, ContainerCartoes).
 *  2. Instanciar a Sessao compartilhada.
 *  3. Instanciar as cinco controladoras de servico concretas (CS*).
 *  4. Expor cada controladora apenas pelo ponteiro de sua interface abstrata
 *     (IServico*) e injetar esses ponteiros na Aplicacao.
 *  5. Iniciar o loop principal da Aplicacao.
 *
 * A camada de apresentacao (Aplicacao e sub-controladoras) NUNCA acessa
 * diretamente CS*, containers.h ou sessao.h: ela depende exclusivamente
 * das cinco interfaces abstratas definidas em interfaces.h.
 */

#include <iostream>

// Dominios e entidades
#include "dominios.h"
#include "entidades.h"

// Camada de servico - incluida SOMENTE em main.cpp
#include "containers.h"
#include "sessao.h"
#include "controladoras_pessoa.h"
#include "controladoras_projeto.h"
#include "controladoras_quadro.h"
#include "controladoras_cartao.h"

// Camada de apresentacao
#include "apresentacao.h"

/**
 * @brief Funcao principal do sistema Kanban.
 *
 * Cria os conteineres de persistencia em memoria, a sessao e as controladoras
 * de servico; injeta os servicos na camada de apresentacao e dispara o loop
 * interativo da aplicacao.
 *
 * @return Codigo de retorno de saida do programa (0 para sucesso).
 */
int main() {

    // -----------------------------------------------------------------------
    // 1. Conteineres em memoria (armazenamento da camada de servico)
    // -----------------------------------------------------------------------
    ContainerPessoas  pessoas;
    ContainerProjetos projetos;
    ContainerQuadros  quadros;
    ContainerCartoes  cartoes;

    // -----------------------------------------------------------------------
    // 2. Sessao compartilhada entre todas as controladoras de servico
    // -----------------------------------------------------------------------
    Sessao sessao;

    // -----------------------------------------------------------------------
    // 3. Controladoras de servico concretas (camada de servico)
    //    Cada uma recebe ponteiros para os conteineres e para a sessao.
    // -----------------------------------------------------------------------
    CSAutenticacao autenticacao(&pessoas, &sessao);
    CSPessoa       servicoPessoa(&pessoas, &projetos, &cartoes, &sessao);
    CSProjeto      servicoProjeto(&pessoas, &projetos, &quadros, &sessao);
    CSQuadro       servicoQuadro(&pessoas, &projetos, &quadros, &cartoes, &sessao);
    CSCartao       servicoCartao(&pessoas, &projetos, &quadros, &cartoes, &sessao);

    // -----------------------------------------------------------------------
    // 4. Ponteiros de interface (a apresentacao so enxerga estes ponteiros)
    // -----------------------------------------------------------------------
    IServicoAutenticacao*    sAuth    = &autenticacao;
    IServicoPessoa*          sPessoa  = &servicoPessoa;
    IServicoProjeto*         sProjeto = &servicoProjeto;
    IServicoQuadro*          sQuadro  = &servicoQuadro;
    IServicoCartaoAtividade* sCartao  = &servicoCartao;

    // -----------------------------------------------------------------------
    // 5. Orquestrador da apresentacao
    // -----------------------------------------------------------------------
    Aplicacao app(sAuth, sPessoa, sProjeto, sQuadro, sCartao);
    app.executar();

    return 0;
}
