#ifndef INTERFACES_H_INCLUDED
#define INTERFACES_H_INCLUDED

#include "entidades.h"
#include <vector>

/**
 * @file interfaces.h
 * @brief Contratos abstratos entre Apresentacao e Servico para o TP1 Kanban.
 *
 * A camada de apresentacao depende somente destes contratos. Os metodos
 * serao implementados pelas controladoras da camada de servico (Membro 3).
 * Os papeis, multiplicidades, WIP e regras de estado sao verificados no servico.
 * As falhas de validacao/integridade devem ser comunicadas por excecoes.
 */

/** @brief Contrato dos 5 servicos CRUD de Pessoa. */
class IServicoPessoa {
public:
    /** @brief Destrutor virtual padrao. */
    virtual ~IServicoPessoa() {}
    /** @brief Cria pessoa; exige email nao cadastrado. */
    virtual void criar(const Pessoa& pessoa) = 0;
    /** @brief Consulta pessoa pela chave Email. */
    virtual Pessoa ler(const Email& email) const = 0;
    /** @brief Atualiza dados da pessoa preservando a chave Email. */
    virtual void atualizar(const Pessoa& pessoa) = 0;
    /** @brief Exclui pessoa sem deixar associacoes inconsistentes. */
    virtual void excluir(const Email& email) = 0;
    /** @brief Lista pessoas registradas. */
    virtual std::vector<Pessoa> listar() const = 0;
};

/** @brief Contrato de autenticacao; requisito preliminar ao acesso. */
class IServicoAutenticacao {
public:
    /** @brief Destrutor virtual padrao. */
    virtual ~IServicoAutenticacao() {}
    /** @brief Autentica usuario por email e senha. */
    virtual bool autenticar(const Email& email, const Senha& senha) const = 0;
};

/** @brief Contrato dos 5 servicos CRUD de Projeto. */
class IServicoProjeto {
public:
    /** @brief Destrutor virtual padrao. */
    virtual ~IServicoProjeto() {}
    /** @brief Cria projeto associado a Pessoa gestora existente. */
    virtual void criar(const Projeto& projeto) = 0;
    /** @brief Consulta projeto por identificador. */
    virtual Projeto ler(const Identificador& identificador) const = 0;
    /** @brief Atualiza projeto preservando a chave primaria. */
    virtual void atualizar(const Projeto& projeto) = 0;
    /** @brief Exclui projeto de forma consistente. */
    virtual void excluir(const Identificador& identificador) = 0;
    /** @brief Lista projetos registrados. */
    virtual std::vector<Projeto> listar() const = 0;
};

/** @brief Contrato dos 5 servicos CRUD de Quadro. */
class IServicoQuadro {
public:
    /** @brief Destrutor virtual padrao. */
    virtual ~IServicoQuadro() {}
    /** @brief Cria quadro de projeto existente. */
    virtual void criar(const Quadro& quadro) = 0;
    /** @brief Consulta quadro por identificador. */
    virtual Quadro ler(const Identificador& identificador) const = 0;
    /** @brief Atualiza quadro e verifica limite de WIP. */
    virtual void atualizar(const Quadro& quadro) = 0;
    /** @brief Exclui quadro sem inconsistencias. */
    virtual void excluir(const Identificador& identificador) = 0;
    /** @brief Lista quadros registrados. */
    virtual std::vector<Quadro> listar() const = 0;
};

/**
 * @brief Contrato dos 11 servicos de Cartao: CRUD, mover, metricas e WIP.
 * @details Prazos de cycle time e lead time serao expressos em minutos.
 * A especificacao define os intervalos, mas nao a unidade; o minuto e
 * convencao de interface proposta aqui e pode ser ajustado em equipe.
 */
class IServicoCartaoAtividade {
public:
    /** @brief Destrutor virtual padrao. */
    virtual ~IServicoCartaoAtividade() {}
    /** @brief Cria cartao em quadro existente com desenvolvedor existente. */
    virtual void criar(const CartaoAtividade& cartao) = 0;
    /** @brief Consulta cartao por identificador. */
    virtual CartaoAtividade ler(const Identificador& identificador) const = 0;
    /** @brief Atualiza dados sem modificar sua chave primaria. */
    virtual void atualizar(const CartaoAtividade& cartao) = 0;
    /** @brief Exclui cartao sem inconsistencia. */
    virtual void excluir(const Identificador& identificador) = 0;
    /** @brief Lista cartoes registrados. */
    virtual std::vector<CartaoAtividade> listar() const = 0;
    /**
     * @brief Move cartao de A FAZER a FAZENDO ou FAZENDO a FEITO.
     * @param idCartao Chave primaria do cartao.
     * @param novoEstado Novo estado permitido.
     * @param instante Instante da mudanca; define inicio ou termino.
     * @throws std::logic_error Se a transicao ou limite WIP for invalido.
     */
    virtual void mover(const Identificador& idCartao,
                       const Estado& novoEstado,
                       const Timestamp& instante) = 0;
    /** @brief Cycle Time = termino - inicio, em minutos. */
    virtual long lerTempoCiclo(const Identificador& idCartao) const = 0;
    /** @brief Media dos cycle times concluidos, em minutos. */
    virtual double lerTempoCicloMedio(const Identificador& idQuadro) const = 0;
    /** @brief Lead Time = termino - entrada, em minutos. */
    virtual long lerLeadTime(const Identificador& idCartao) const = 0;
    /** @brief Media dos lead times concluidos, em minutos. */
    virtual double lerLeadTimeMedio(const Identificador& idQuadro) const = 0;
    /** @brief Contagem atual de cartoes em FAZENDO no quadro. */
    virtual unsigned int lerWip(const Identificador& idQuadro) const = 0;
};

#endif // INTERFACES_H_INCLUDED
