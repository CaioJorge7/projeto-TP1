#ifndef APRESENTACAO_H_INCLUDED
#define APRESENTACAO_H_INCLUDED

/**
 * @file apresentacao.h
 * @brief Camada de Apresentacao do sistema Kanban (TP1).
 *
 * Esta camada depende EXCLUSIVAMENTE das cinco interfaces abstratas declaradas
 * em interfaces.h. Nenhum cabecalho de controladora de servico (CS*) ou de
 * containers e incluido aqui.
 *
 * Estrutura:
 *  - ApresentacaoPessoa   : CRUD de Pessoa (servicos 1-5)
 *  - ApresentacaoProjeto  : CRUD de Projeto (servicos 6-10)
 *  - ApresentacaoQuadro   : CRUD de Quadro (servicos 11-15)
 *  - ApresentacaoCartao   : CRUD + mover + metricas de Cartao (servicos 16-26)
 *  - Aplicacao            : Orquestrador; exibe menus raiz e delega aos
 *                           sub-controladores de apresentacao.
 */

#include <string>
#include "interfaces.h"


// ===========================================================================
// ApresentacaoPessoa
// ===========================================================================

/**
 * @brief Controladora de apresentacao para a entidade Pessoa.
 *
 * Implementa os menus e fluxos de entrada/saida das 5 operacoes CRUD de Pessoa.
 * Depende exclusivamente de IServicoPessoa.
 */
class ApresentacaoPessoa {
private:
    IServicoPessoa* servico; ///< Ponteiro para o servico de Pessoa injetado por main.cpp.

    /** @brief Fluxo de criacao de Pessoa (servico 1). */
    void criar();
    /** @brief Fluxo de leitura de Pessoa por e-mail (servico 2). */
    void ler();
    /** @brief Fluxo de atualizacao dos proprios dados (servico 3). */
    void atualizar();
    /** @brief Fluxo de exclusao da propria conta (servico 4). */
    void excluir();
    /** @brief Fluxo de listagem de Pessoas: exibe o e-mail (identificador) de cada uma (servico 5). */
    void listar();

public:
    /**
     * @brief Constroi a controladora de apresentacao de Pessoa.
     * @param servico Ponteiro para a implementacao de IServicoPessoa.
     */
    explicit ApresentacaoPessoa(IServicoPessoa* servico);

    /**
     * @brief Exibe e executa o submenu de Pessoa em loop ate o usuario voltar.
     */
    void exibirMenu();
};


// ===========================================================================
// ApresentacaoProjeto
// ===========================================================================

/**
 * @brief Controladora de apresentacao para a entidade Projeto.
 *
 * Implementa os menus e fluxos das 5 operacoes CRUD de Projeto (servicos 6-10).
 * Depende exclusivamente de IServicoProjeto.
 */
class ApresentacaoProjeto {
private:
    IServicoProjeto* servico; ///< Ponteiro para o servico de Projeto injetado por main.cpp.

    /** @brief Fluxo de criacao de Projeto (servico 6). */
    void criar();
    /** @brief Fluxo de leitura de Projeto por identificador (servico 7). */
    void ler();
    /** @brief Fluxo de atualizacao de Projeto (servico 8). */
    void atualizar();
    /** @brief Fluxo de exclusao de Projeto (servico 9). */
    void excluir();
    /** @brief Fluxo de listagem de Projetos: exibe o identificador de cada um (servico 10). */
    void listar();

public:
    /**
     * @brief Constroi a controladora de apresentacao de Projeto.
     * @param servico Ponteiro para a implementacao de IServicoProjeto.
     */
    explicit ApresentacaoProjeto(IServicoProjeto* servico);

    /**
     * @brief Exibe e executa o submenu de Projeto em loop ate o usuario voltar.
     */
    void exibirMenu();
};


// ===========================================================================
// ApresentacaoQuadro
// ===========================================================================

/**
 * @brief Controladora de apresentacao para a entidade Quadro.
 *
 * Implementa os menus e fluxos das 5 operacoes CRUD de Quadro (servicos 11-15).
 * Depende exclusivamente de IServicoQuadro.
 */
class ApresentacaoQuadro {
private:
    IServicoQuadro* servico; ///< Ponteiro para o servico de Quadro injetado por main.cpp.

    /** @brief Fluxo de criacao de Quadro (servico 11). */
    void criar();
    /** @brief Fluxo de leitura de Quadro por identificador (servico 12). */
    void ler();
    /** @brief Fluxo de atualizacao de Quadro (servico 13). */
    void atualizar();
    /** @brief Fluxo de exclusao de Quadro (servico 14). */
    void excluir();
    /** @brief Fluxo de listagem de Quadros: exibe o identificador de cada um (servico 15). */
    void listar();

public:
    /**
     * @brief Constroi a controladora de apresentacao de Quadro.
     * @param servico Ponteiro para a implementacao de IServicoQuadro.
     */
    explicit ApresentacaoQuadro(IServicoQuadro* servico);

    /**
     * @brief Exibe e executa o submenu de Quadro em loop ate o usuario voltar.
     */
    void exibirMenu();
};


// ===========================================================================
// ApresentacaoCartao
// ===========================================================================

/**
 * @brief Controladora de apresentacao para a entidade CartaoAtividade.
 *
 * Cobre as 11 operacoes do servico de Cartao (servicos 16-26):
 *  - CRUD basico (criar, ler, atualizar, excluir, listar)
 *  - Movimentacao Kanban (mover: A FAZER->FAZENDO->FEITO)
 *  - Metricas: Tempo de Ciclo, Tempo de Ciclo Medio, Lead Time,
 *              Lead Time Medio e WIP
 *
 * Depende exclusivamente de IServicoCartaoAtividade.
 */
class ApresentacaoCartao {
private:
    IServicoCartaoAtividade* servico; ///< Ponteiro para o servico de Cartao.

    /** @brief Fluxo de criacao de Cartao (servico 16). */
    void criar();
    /** @brief Fluxo de leitura de Cartao por identificador (servico 17). */
    void ler();
    /** @brief Fluxo de atualizacao de Cartao (servico 18). */
    void atualizar();
    /** @brief Fluxo de exclusao de Cartao (servico 19). */
    void excluir();
    /** @brief Fluxo de listagem de Cartoes: exibe o identificador de cada um (servico 20). */
    void listar();
    /** @brief Fluxo de movimentacao Kanban: A FAZER->FAZENDO ou FAZENDO->FEITO (servico 21). */
    void mover();
    /** @brief Exibe Cycle Time de um Cartao concluido (servico 22). */
    void lerTempoCiclo();
    /** @brief Exibe media dos Cycle Times concluidos de um Quadro (servico 23). */
    void lerTempoCicloMedio();
    /** @brief Exibe Lead Time de um Cartao concluido (servico 24). */
    void lerLeadTime();
    /** @brief Exibe media dos Lead Times concluidos de um Quadro (servico 25). */
    void lerLeadTimeMedio();
    /** @brief Exibe quantidade de Cartoes em FAZENDO de um Quadro - WIP (servico 26). */
    void lerWip();

public:
    /**
     * @brief Constroi a controladora de apresentacao de Cartao.
     * @param servico Ponteiro para a implementacao de IServicoCartaoAtividade.
     */
    explicit ApresentacaoCartao(IServicoCartaoAtividade* servico);

    /**
     * @brief Exibe e executa o submenu de Cartao em loop ate o usuario voltar.
     */
    void exibirMenu();
};


// ===========================================================================
// Aplicacao
// ===========================================================================

/**
 * @brief Orquestrador principal da aplicacao.
 *
 * Gerencia os menus raiz (inicial e principal) e delega as operacoes
 * especificas de cada entidade para as sub-controladoras de apresentacao.
 *
 * Login e logout usam apenas o contrato IServicoAutenticacao
 * (autenticar e encerrarSessao), sem dependencia das classes CS*.
 */
class Aplicacao {
private:
    IServicoAutenticacao*     sAuth;      ///< Servico de autenticacao (login).
    IServicoPessoa*           sPessoa;    ///< Servico de Pessoa (usado em criarConta).
    ApresentacaoPessoa        apPessoa;   ///< Sub-controladora de Pessoa.
    ApresentacaoProjeto       apProjeto;  ///< Sub-controladora de Projeto.
    ApresentacaoQuadro        apQuadro;   ///< Sub-controladora de Quadro.
    ApresentacaoCartao        apCartao;   ///< Sub-controladora de Cartao.
    bool                      autenticado; ///< Estado local: ha usuario autenticado?

    /**
     * @brief Exibe o menu inicial (nao autenticado) em loop.
     *
     * Retorna quando o usuario faz login (autenticado passa a true)
     * ou escolhe sair (autenticado permanece false).
     */
    void menuInicial();

    /**
     * @brief Exibe o menu principal (autenticado) em loop.
     *
     * Retorna quando o usuario faz logout (autenticado passa a false).
     */
    void menuPrincipal();

    /**
     * @brief Fluxo de criacao de conta a partir do menu inicial.
     *
     * Reutiliza IServicoPessoa::criar, que nao exige autenticacao.
     */
    void criarConta();

    /**
     * @brief Fluxo de autenticacao (login).
     *
     * Chama IServicoAutenticacao::autenticar e, em caso de sucesso,
     * define autenticado = true.
     */
    void autenticar();

    /**
     * @brief Realiza o logout: chama IServicoAutenticacao::encerrarSessao e
     *        define autenticado = false.
     */
    void logout();

public:
    /**
     * @brief Constroi o orquestrador.
     * @param sa        Servico de autenticacao.
     * @param sp        Servico de Pessoa (tambem passado para apPessoa).
     * @param spj       Servico de Projeto.
     * @param sq        Servico de Quadro.
     * @param sc        Servico de Cartao.
     */
    Aplicacao(IServicoAutenticacao*    sa,
              IServicoPessoa*          sp,
              IServicoProjeto*         spj,
              IServicoQuadro*          sq,
              IServicoCartaoAtividade* sc);

    /**
     * @brief Inicia o loop principal da aplicacao.
     *
     * Alterna entre menuInicial e menuPrincipal enquanto o usuario nao
     * escolher Sair no menu inicial.
     */
    void executar();
};

#endif // APRESENTACAO_H_INCLUDED
