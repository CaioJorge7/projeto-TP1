#ifndef INTERFACES_H_INCLUDED
#define INTERFACES_H_INCLUDED

#include <vector>
#include "dominios.h"
#include "entidades.h"

// TODO(Membro2): ARQUIVO PROVISORIO. Interfaces escritas so para o Membro 3
// poder construir a camada de servico. Nomes, assinaturas e convencoes abaixo
// sao todos a definir.
//
// Convencoes assumidas (TODO(Membro2): confirmar ou trocar):
//  - Metodos retornam bool: true = sucesso, false = falha.
//  - Resultados de leitura saem por parametro ponteiro.
//  - "solicitante" e o email de quem esta logado. O servico confere se ele
//    existe e se o papel dele tem direito ao servico.

/**
 * @brief Interface do servico de autenticacao.
 */
class ISAutenticacao {
public:
    /**
     * @brief Confere se existe uma pessoa com este email e esta senha.
     * @return true se a autenticacao teve sucesso.
     */
    virtual bool autenticar(const Email& email, const Senha& senha) = 0;
    virtual ~ISAutenticacao() = default;
};

/**
 * @brief Interface dos servicos 1 a 5 (CRUD e listagem de Pessoa).
 */
class ISPessoa {
public:
    // TODO(Membro2): criar nao tem solicitante, pois a conta e criada antes do login.
    virtual bool criar(const Pessoa& pessoa) = 0;
    virtual bool ler(const Email& solicitante, const Email& email, Pessoa* pessoa) = 0;
    // TODO(Membro2): definir se uma pessoa so pode atualizar/excluir a si mesma.
    virtual bool atualizar(const Email& solicitante, const Pessoa& pessoa) = 0;
    virtual bool excluir(const Email& solicitante, const Email& email) = 0;
    virtual bool listar(const Email& solicitante, vector<Email>* emails) = 0;
    virtual ~ISPessoa() = default;
};

/**
 * @brief Interface dos servicos 6 a 10 (CRUD e listagem de Projeto).
 */
class ISProjeto {
public:
    // O gestor do projeto e o solicitante que o cria.
    virtual bool criar(const Email& solicitante, const Projeto& projeto) = 0;
    virtual bool ler(const Email& solicitante, const Identificador& id, Projeto* projeto) = 0;
    virtual bool atualizar(const Email& solicitante, const Projeto& projeto) = 0;
    virtual bool excluir(const Email& solicitante, const Identificador& id) = 0;
    virtual bool listar(const Email& solicitante, vector<Identificador>* ids) = 0;
    virtual ~ISProjeto() = default;
};

/**
 * @brief Interface dos servicos 11 a 15 (CRUD e listagem de Quadro).
 */
class ISQuadro {
public:
    // O quadro pertence a um projeto (multiplicidade 1 projeto : 0..* quadros).
    virtual bool criar(const Email& solicitante, const Identificador& projeto, const Quadro& quadro) = 0;
    virtual bool ler(const Email& solicitante, const Identificador& id, Quadro* quadro) = 0;
    virtual bool atualizar(const Email& solicitante, const Quadro& quadro) = 0;
    virtual bool excluir(const Email& solicitante, const Identificador& id) = 0;
    // TODO(Membro2): listar todos os quadros ou so os de um projeto?
    virtual bool listar(const Email& solicitante, vector<Identificador>* ids) = 0;
    virtual ~ISQuadro() = default;
};

/**
 * @brief Interface dos servicos 16 a 26 (Cartao de atividade, movimentacao e metricas).
 */
class ISCartao {
public:
    // O cartao pertence a um quadro e tem um desenvolvedor (uma Pessoa).
    // O servico coloca o cartao em A FAZER; "entrada" vem preenchido no cartao.
    virtual bool criar(const Email& solicitante, const Identificador& quadro,
                       const Email& desenvolvedor, const Cartao& cartao) = 0;
    virtual bool ler(const Email& solicitante, const Identificador& id, Cartao* cartao) = 0;
    // TODO(Membro2): atualizar nao deve mexer em estado nem nos timestamps (isso e do mover).
    virtual bool atualizar(const Email& solicitante, const Cartao& cartao) = 0;
    virtual bool excluir(const Email& solicitante, const Identificador& id) = 0;
    virtual bool listar(const Email& solicitante, vector<Identificador>* ids) = 0;

    // Servico 21. Transicoes validas: A FAZER -> FAZENDO e FAZENDO -> FEITO.
    // TODO(Membro2): "momento" vem de fora para facilitar o teste; a apresentacao
    // pode preencher com o relogio do sistema ou pedir ao usuario.
    virtual bool mover(const Email& solicitante, const Identificador& id,
                       const Estado& destino, const Timestamp& momento) = 0;

    // Servicos 22 a 26. TODO(Membro2): unidade dos tempos assumida em minutos.
    virtual bool lerTempoCiclo(const Email& solicitante, const Identificador& cartao, int* minutos) = 0;
    virtual bool lerTempoCicloMedio(const Email& solicitante, const Identificador& quadro, double* minutos) = 0;
    virtual bool lerLeadTime(const Email& solicitante, const Identificador& cartao, int* minutos) = 0;
    virtual bool lerLeadTimeMedio(const Email& solicitante, const Identificador& quadro, double* minutos) = 0;
    virtual bool lerWip(const Email& solicitante, const Identificador& quadro, int* wip) = 0;
    virtual ~ISCartao() = default;
};

#endif // INTERFACES_H_INCLUDED
