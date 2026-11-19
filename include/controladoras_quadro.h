#ifndef CONTROLADORAS_QUADRO_H_INCLUDED
#define CONTROLADORAS_QUADRO_H_INCLUDED

#include <stdexcept>
#include <vector>
#include "dominios.h"
#include "entidades.h"
#include "interfaces.h"
#include "containers.h"
#include "sessao.h"

/**
 * @file controladoras_quadro.h
 * @brief Controladora de servico de Quadro.
 */

/**
 * @brief Controladora dos servicos 11 a 15 (CRUD e listagem de Quadro).
 */
class CSQuadro : public IServicoQuadro {
private:
    /** @brief Ponteiro para o conteiner de pessoas. */
    ContainerPessoas* pessoas;
    /** @brief Ponteiro para o conteiner de projetos. */
    ContainerProjetos* projetos;
    /** @brief Ponteiro para o conteiner de quadros. */
    ContainerQuadros* quadros;
    /** @brief Ponteiro para o conteiner de cartoes. */
    ContainerCartoes* cartoes;
    /** @brief Ponteiro para a sessao compartilhada. */
    Sessao* sessao;

    /**
     * @brief Confere se o projeto existe e se o usuario e o gestor dele.
     * @param usuario Pessoa autenticada.
     * @param idProjeto Identificador do projeto.
     * @throw std::logic_error Se o projeto nao existe ou usuario nao e o gestor.
     */
    void exigirGestorDoProjeto(const Pessoa& usuario, const Identificador& idProjeto) const {
        const Projeto* projeto = projetos->pesquisar(idProjeto);
        if (projeto == nullptr) {
            throw std::logic_error("Projeto nao encontrado.");
        }
        if (projeto->getGestorEmail().getValor() != usuario.getEmail().getValor()) {
            throw std::logic_error("Somente o gestor do projeto pode fazer esta operacao.");
        }
    }

    /**
     * @brief Devolve o quadro caso exista.
     * @param id Identificador do quadro.
     * @return Referencia constante para o quadro.
     * @throw std::logic_error Se o quadro nao existe.
     */
    const Quadro& exigirQuadro(const Identificador& id) const {
        const Quadro* quadro = quadros->pesquisar(id);
        if (quadro == nullptr) {
            throw std::logic_error("Quadro nao encontrado.");
        }
        return *quadro;
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas (para conferir existencia e papel).
     * @param projetos Container de projetos (para conferir o gestor).
     * @param quadros Container de quadros.
     * @param cartoes Container de cartoes (para conferir WIP e exclusao).
     * @param sessao Sessao compartilhada com as outras controladoras.
     */
    CSQuadro(ContainerPessoas* pessoas, ContainerProjetos* projetos,
             ContainerQuadros* quadros, ContainerCartoes* cartoes, Sessao* sessao)
        : pessoas(pessoas), projetos(projetos), quadros(quadros), cartoes(cartoes),
          sessao(sessao) {}

    // ESCOLHA(Membro3): so o gestor DO projeto cria quadro nele (nao basta ser
    //   GESTOR). Falha se o projeto nao existe ou se o identificador do quadro
    //   ja existe. Assim a multiplicidade 1 projeto : 0..* quadros e mantida.
    /**
     * @brief Cria um quadro em um projeto existente.
     * @param quadro Dados do quadro (o projetoId indica o projeto).
     * @throw std::logic_error Se o usuario nao e o gestor do projeto, o projeto
     *        nao existe ou ja existe quadro com o mesmo identificador.
     */
    void criar(const Quadro& quadro) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        exigirGestorDoProjeto(usuario, quadro.getProjetoId());
        if (!quadros->incluir(quadro)) {
            throw std::logic_error("Ja existe quadro com este identificador.");
        }
    }

    // ESCOLHA(Membro3): qualquer pessoa autenticada le qualquer quadro.
    /**
     * @brief Le os dados de um quadro.
     * @param identificador Identificador do quadro.
     * @return Copia do quadro.
     * @throw std::logic_error Se ninguem esta autenticado ou o quadro nao existe.
     */
    Quadro ler(const Identificador& identificador) const override {
        sessao->exigirUsuario(*pessoas);
        return exigirQuadro(identificador);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro atualiza. O
    //   identificador e o projetoId nao mudam: valem os que ja estao guardados.
    // ESCOLHA(Membro3): o novo limite nao pode ser menor que o WIP atual
    //   (cartoes em FAZENDO), senao o quadro ficaria acima do limite.
    /**
     * @brief Atualiza os dados de um quadro.
     * @param quadro Novos dados (o identificador identifica o quadro).
     * @throw std::logic_error Se o usuario nao e o gestor do projeto do quadro,
     *        o quadro nao existe ou o novo limite e menor que o WIP atual.
     */
    void atualizar(const Quadro& quadro) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        const Quadro& atual = exigirQuadro(quadro.getIdentificador());
        exigirGestorDoProjeto(usuario, atual.getProjetoId());
        if (quadro.getLimite().getValor() <
            cartoes->contarPorEstado(atual.getIdentificador(), "FAZENDO")) {
            throw std::logic_error("O novo limite e menor que o WIP atual do quadro.");
        }
        Quadro novo = quadro;
        novo.setProjetoId(atual.getProjetoId());
        quadros->atualizar(novo);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro exclui, e so se o
    //   quadro nao tiver cartoes (evita inconsistencia, requisito 15).
    // TODO(Membro2): se preferir cascata, mudar aqui.
    /**
     * @brief Exclui um quadro.
     * @param identificador Identificador do quadro.
     * @throw std::logic_error Se o usuario nao e o gestor do projeto do quadro,
     *        o quadro nao existe ou ainda tem cartoes.
     */
    void excluir(const Identificador& identificador) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        const Quadro& atual = exigirQuadro(identificador);
        exigirGestorDoProjeto(usuario, atual.getProjetoId());
        if (cartoes->contarPorQuadro(identificador) > 0) {
            throw std::logic_error("Quadro com cartoes nao pode ser excluido.");
        }
        quadros->remover(identificador);
    }

    // TODO(Membro2): decidir se deve listar so os quadros de um projeto. Aqui
    //   qualquer pessoa autenticada ve todos os quadros.
    /**
     * @brief Lista todos os quadros.
     * @return Copias dos quadros, em ordem de identificador.
     * @throw std::logic_error Se ninguem esta autenticado.
     */
    std::vector<Quadro> listar() const override {
        sessao->exigirUsuario(*pessoas);
        return quadros->listar();
    }
};

#endif // CONTROLADORAS_QUADRO_H_INCLUDED
