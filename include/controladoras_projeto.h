#ifndef CONTROLADORAS_PROJETO_H_INCLUDED
#define CONTROLADORAS_PROJETO_H_INCLUDED

#include <stdexcept>
#include <vector>
#include "dominios.h"
#include "entidades.h"
#include "interfaces.h"
#include "containers.h"
#include "sessao.h"
#include "tempo.h"

/**
 * @file controladoras_projeto.h
 * @brief Controladora de servico de Projeto.
 */

/**
 * @brief Controladora dos servicos 6 a 10 (CRUD e listagem de Projeto).
 */
class CSProjeto : public IServicoProjeto {
private:
    /** @brief Ponteiro para o conteiner de pessoas. */
    ContainerPessoas* pessoas;
    /** @brief Ponteiro para o conteiner de projetos. */
    ContainerProjetos* projetos;
    /** @brief Ponteiro para o conteiner de quadros. */
    ContainerQuadros* quadros;
    /** @brief Ponteiro para a sessao compartilhada. */
    Sessao* sessao;

    /**
     * @brief Devolve o projeto se ele existe e o usuario autenticado e o gestor dele.
     * @param usuario Pessoa autenticada.
     * @param id Identificador do projeto.
     * @return Referencia constante para o projeto.
     * @throw std::logic_error Se o projeto nao existe ou se o usuario nao e o gestor.
     */
    const Projeto& exigirProjetoDoGestor(const Pessoa& usuario, const Identificador& id) const {
        const Projeto* projeto = projetos->pesquisar(id);
        if (projeto == nullptr) {
            throw std::logic_error("Projeto nao encontrado.");
        }
        if (projeto->getGestorEmail().getValor() != usuario.getEmail().getValor()) {
            throw std::logic_error("Somente o gestor do projeto pode fazer esta operacao.");
        }
        return *projeto;
    }

    /**
     * @brief Confere se o termino do projeto nao e anterior ao inicio.
     * @param projeto Projeto a conferir.
     * @throw std::logic_error Se as duas datas existem e o termino e anterior ao inicio.
     */
    void exigirDatasCoerentes(const Projeto& projeto) const {
        long inicio;
        long termino;
        if (timestampParaMinutos(projeto.getInicio(), &inicio) &&
            timestampParaMinutos(projeto.getTermino(), &termino) &&
            termino < inicio) {
            throw std::logic_error("O termino do projeto nao pode ser anterior ao inicio.");
        }
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas (para conferir existencia e papel).
     * @param projetos Container de projetos.
     * @param quadros Container de quadros (para impedir excluir projeto com quadros).
     * @param sessao Sessao compartilhada com as outras controladoras.
     */
    CSProjeto(ContainerPessoas* pessoas, ContainerProjetos* projetos,
              ContainerQuadros* quadros, Sessao* sessao)
        : pessoas(pessoas), projetos(projetos), quadros(quadros), sessao(sessao) {}

    // Decisao de projeto: so GESTOR cria projeto, e o gestor do projeto e quem o
    //   criou (como diz o enunciado). Por isso o gestorEmail recebido e
    //   trocado pelo email do usuario autenticado.
    /**
     * @brief Cria um projeto; o usuario autenticado vira o gestor dele.
     * @param projeto Dados do projeto (o gestorEmail e ignorado).
     * @throw std::logic_error Se o usuario nao esta autenticado como GESTOR, se
     *        o termino e anterior ao inicio ou se ja existe projeto com o mesmo
     *        identificador.
     */
    void criar(const Projeto& projeto) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        exigirDatasCoerentes(projeto);
        Projeto novo = projeto;
        novo.setGestorEmail(usuario.getEmail());
        if (!projetos->incluir(novo)) {
            throw std::logic_error("Ja existe projeto com este identificador.");
        }
    }

    // Decisao de projeto: qualquer pessoa autenticada le qualquer projeto
    //   (o enunciado libera LER PROJETO para os dois papeis).
    /**
     * @brief Le os dados de um projeto.
     * @param identificador Identificador do projeto.
     * @return Copia do projeto.
     * @throw std::logic_error Se ninguem esta autenticado ou o projeto nao existe.
     */
    Projeto ler(const Identificador& identificador) const override {
        sessao->exigirUsuario(*pessoas);
        const Projeto* projeto = projetos->pesquisar(identificador);
        if (projeto == nullptr) {
            throw std::logic_error("Projeto nao encontrado.");
        }
        return *projeto;
    }

    // Decisao de projeto: so o gestor DO projeto atualiza (nao basta ser
    //   GESTOR). O identificador e o gestorEmail nao mudam: valem os que ja
    //   estao guardados.
    // Decisao de projeto: o termino nao pode ser anterior ao inicio (vale
    //   tambem na criacao).
    /**
     * @brief Atualiza os dados de um projeto.
     * @param projeto Novos dados (o identificador identifica o projeto).
     * @throw std::logic_error Se o usuario nao e o gestor do projeto, o
     *        projeto nao existe ou o termino e anterior ao inicio.
     */
    void atualizar(const Projeto& projeto) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        const Projeto& atual = exigirProjetoDoGestor(usuario, projeto.getIdentificador());
        exigirDatasCoerentes(projeto);
        Projeto novo = projeto;
        novo.setGestorEmail(atual.getGestorEmail());
        projetos->atualizar(novo);
    }

    // Decisao de projeto: so o gestor DO projeto exclui, e so se o projeto nao
    //   tiver quadros. A exclusao e bloqueada (e nao feita em cascata) para
    //   nao apagar quadros e cartoes sem aviso e para nao deixar quadro sem
    //   projeto (requisito 15).
    /**
     * @brief Exclui um projeto.
     * @param identificador Identificador do projeto.
     * @throw std::logic_error Se o usuario nao e o gestor do projeto, o projeto
     *        nao existe ou ainda tem quadros.
     */
    void excluir(const Identificador& identificador) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        exigirProjetoDoGestor(usuario, identificador);
        if (quadros->contarPorProjeto(identificador) > 0) {
            throw std::logic_error("Projeto com quadros nao pode ser excluido.");
        }
        projetos->remover(identificador);
    }

    // Decisao de projeto: qualquer pessoa autenticada lista todos os projetos
    //   (o enunciado libera LISTAR PROJETO para os dois papeis e nao preve
    //   filtro). A apresentacao exibe o identificador de cada um.
    /**
     * @brief Lista todos os projetos.
     * @return Copias dos projetos, em ordem de identificador.
     * @throw std::logic_error Se ninguem esta autenticado.
     */
    std::vector<Projeto> listar() const override {
        sessao->exigirUsuario(*pessoas);
        return projetos->listar();
    }
};

#endif // CONTROLADORAS_PROJETO_H_INCLUDED
