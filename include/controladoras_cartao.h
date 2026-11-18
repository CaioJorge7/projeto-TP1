#ifndef CONTROLADORAS_CARTAO_H_INCLUDED
#define CONTROLADORAS_CARTAO_H_INCLUDED

#include <stdexcept>
#include <string>
#include <vector>
#include "dominios.h"
#include "entidades.h"
#include "interfaces.h"
#include "containers.h"
#include "sessao.h"
#include "tempo.h"

/**
 * @file controladoras_cartao.h
 * @brief Controladora de servico de Cartao de atividade (CRUD, mover e metricas).
 */

/**
 * @brief Controladora dos servicos 16 a 26 (CRUD, listagem, movimentacao e
 *        metricas de Cartao de atividade).
 */
class CSCartao : public IServicoCartaoAtividade {
private:
    ContainerPessoas* pessoas;
    ContainerProjetos* projetos;
    ContainerQuadros* quadros;
    ContainerCartoes* cartoes;
    Sessao* sessao;

    // Devolve o cartao se ele existe.
    const CartaoAtividade& exigirCartao(const Identificador& id) const {
        const CartaoAtividade* cartao = cartoes->pesquisar(id);
        if (cartao == nullptr) {
            throw std::logic_error("Cartao nao encontrado.");
        }
        return *cartao;
    }

    // Devolve o quadro se ele existe.
    const Quadro& exigirQuadro(const Identificador& id) const {
        const Quadro* quadro = quadros->pesquisar(id);
        if (quadro == nullptr) {
            throw std::logic_error("Quadro nao encontrado.");
        }
        return *quadro;
    }

    // Confere se o quadro existe e se o usuario e o gestor do projeto dele.
    void exigirGestorDoQuadro(const Pessoa& usuario, const Identificador& idQuadro) const {
        const Quadro& quadro = exigirQuadro(idQuadro);
        const Projeto* projeto = projetos->pesquisar(quadro.getProjetoId());
        if (projeto == nullptr) {
            throw std::logic_error("Projeto do quadro nao encontrado.");
        }
        if (projeto->getGestorEmail().getValor() != usuario.getEmail().getValor()) {
            throw std::logic_error("Somente o gestor do projeto pode fazer esta operacao.");
        }
    }

    // Confere se a pessoa existe e tem papel DESENVOLVEDOR.
    void exigirDesenvolvedor(const Email& email) const {
        const Pessoa* pessoa = pessoas->pesquisar(email);
        if (pessoa == nullptr) {
            throw std::logic_error("Desenvolvedor nao encontrado.");
        }
        if (pessoa->getPapel().getValor() != "DESENVOLVEDOR") {
            throw std::logic_error("O responsavel pelo cartao deve ter papel DESENVOLVEDOR.");
        }
    }

    // Minutos entre dois timestamps. Falha se algum esta vazio ou se "ate" e
    // anterior a "de".
    static bool minutosEntre(const Timestamp& de, const Timestamp& ate, long* minutos) {
        long a, b;
        if (!timestampParaMinutos(de, &a) || !timestampParaMinutos(ate, &b)) return false;
        if (b < a) return false;
        *minutos = b - a;
        return true;
    }

    // Tempo de ciclo (lead = false) ou lead time (lead = true) de um cartao.
    long lerTempo(const Identificador& id, bool lead) const {
        sessao->exigirUsuario(*pessoas);
        const CartaoAtividade& cartao = exigirCartao(id);
        if (cartao.getEstado().getValor() != "FEITO") {
            throw std::logic_error("O cartao ainda nao foi concluido.");
        }
        long total;
        if (!minutosEntre(lead ? cartao.getEntrada() : cartao.getInicio(),
                          cartao.getTermino(), &total)) {
            throw std::logic_error("As datas do cartao estao inconsistentes.");
        }
        return total;
    }

    // Media do tempo de ciclo (lead = false) ou do lead time (lead = true) dos
    // cartoes concluidos de um quadro.
    double lerMedia(const Identificador& idQuadro, bool lead) const {
        sessao->exigirUsuario(*pessoas);
        exigirQuadro(idQuadro);
        std::vector<CartaoAtividade> todos = cartoes->listarPorQuadro(idQuadro);
        long soma = 0;
        int concluidos = 0;
        for (size_t i = 0; i < todos.size(); ++i) {
            if (todos[i].getEstado().getValor() != "FEITO") continue;
            long total;
            if (!minutosEntre(lead ? todos[i].getEntrada() : todos[i].getInicio(),
                              todos[i].getTermino(), &total)) continue;
            soma += total;
            ++concluidos;
        }
        if (concluidos == 0) {
            throw std::logic_error("Nenhum cartao concluido neste quadro.");
        }
        return static_cast<double>(soma) / concluidos;
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas.
     * @param projetos Container de projetos (para conferir o gestor).
     * @param quadros Container de quadros (para conferir existencia e limite de WIP).
     * @param cartoes Container de cartoes.
     * @param sessao Sessao compartilhada com as outras controladoras.
     */
    CSCartao(ContainerPessoas* pessoas, ContainerProjetos* projetos,
             ContainerQuadros* quadros, ContainerCartoes* cartoes, Sessao* sessao)
        : pessoas(pessoas), projetos(projetos), quadros(quadros), cartoes(cartoes),
          sessao(sessao) {}

    // ESCOLHA(Membro3): so o gestor do projeto do quadro cria cartao nele.
    //   O desenvolvedor precisa existir e ter papel DESENVOLVEDOR.
    // TODO(Membro2): confirmar se um GESTOR tambem pode ser desenvolvedor de
    //   cartao (aqui nao pode).
    // ESCOLHA(Membro3): o cartao novo sempre entra em A FAZER, com inicio e
    //   termino vazios, seja qual for o estado recebido. A "entrada" precisa
    //   vir preenchida (sem ela nao da pra calcular o lead time).
    /**
     * @brief Cria um cartao em A FAZER em um quadro existente.
     * @param cartao Dados do cartao (quadroId, desenvolvedorEmail e entrada preenchidos).
     * @throw std::logic_error Se o usuario nao e o gestor do projeto do quadro,
     *        o quadro ou o desenvolvedor nao existem, o desenvolvedor nao tem papel
     *        DESENVOLVEDOR, a entrada esta vazia ou o identificador ja existe.
     */
    void criar(const CartaoAtividade& cartao) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        exigirGestorDoQuadro(usuario, cartao.getQuadroId());
        exigirDesenvolvedor(cartao.getDesenvolvedorEmail());
        long entrada;
        if (!timestampParaMinutos(cartao.getEntrada(), &entrada)) {
            throw std::logic_error("O cartao novo precisa de data de entrada.");
        }

        CartaoAtividade novo = cartao;
        Estado aFazer;
        aFazer.setValor("A FAZER");
        novo.setEstado(aFazer);
        novo.setInicio(Timestamp());
        novo.setTermino(Timestamp());
        if (!cartoes->incluir(novo)) {
            throw std::logic_error("Ja existe cartao com este identificador.");
        }
    }

    // ESCOLHA(Membro3): qualquer pessoa autenticada le qualquer cartao.
    /**
     * @brief Le os dados de um cartao.
     * @param identificador Identificador do cartao.
     * @return Copia do cartao.
     * @throw std::logic_error Se ninguem esta autenticado ou o cartao nao existe.
     */
    CartaoAtividade ler(const Identificador& identificador) const override {
        sessao->exigirUsuario(*pessoas);
        return exigirCartao(identificador);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro atualiza. Valem os
    //   dados guardados para identificador, quadroId, estado, entrada, inicio e
    //   termino (estado e datas so mudam pelo "mover"). O desenvolvedor pode ser
    //   trocado, desde que o novo exista e tenha papel DESENVOLVEDOR.
    /**
     * @brief Atualiza nome, descricao, prioridade, tamanho e desenvolvedor de um cartao.
     * @param cartao Novos dados (o identificador identifica o cartao).
     * @throw std::logic_error Se o usuario nao e o gestor do projeto do quadro do
     *        cartao, o cartao nao existe ou o novo desenvolvedor e invalido.
     */
    void atualizar(const CartaoAtividade& cartao) override {
        Pessoa usuario = sessao->exigirGestor(*pessoas);
        const CartaoAtividade& atual = exigirCartao(cartao.getIdentificador());
        exigirGestorDoQuadro(usuario, atual.getQuadroId());
        exigirDesenvolvedor(cartao.getDesenvolvedorEmail());

        CartaoAtividade novo = cartao;
        novo.setQuadroId(atual.getQuadroId());
        novo.setEstado(atual.getEstado());
        novo.setEntrada(atual.getEntrada());
        novo.setInicio(atual.getInicio());
        novo.setTermino(atual.getTermino());
