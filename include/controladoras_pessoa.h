#ifndef CONTROLADORAS_PESSOA_H_INCLUDED
#define CONTROLADORAS_PESSOA_H_INCLUDED

#include <stdexcept>
#include <vector>
#include "dominios.h"
#include "entidades.h"
#include "interfaces.h"
#include "containers.h"
#include "sessao.h"

/**
 * @file controladoras_pessoa.h
 * @brief Controladoras de servico de autenticacao e de Pessoa.
 */

// Decisao de projeto: nomes das controladoras: "CS" (Controladora de Servico) +
//   nome do servico. Cada uma realiza a interface IServico... correspondente.
// Decisao de projeto: cada controladora recebe por ponteiro os containers e a
//   Sessao de que precisa. Assim todas compartilham os mesmos dados em memoria.
// Decisao de projeto: falhas (sem login, papel sem direito, duplicidade, nao
//   encontrado, regra violada) viram std::logic_error com uma mensagem, como
//   definido em interfaces.h. Falha de formato continua sendo
//   std::invalid_argument, lancada pelos dominios (e que tambem e logic_error).

/**
 * @brief Controladora do servico de autenticacao.
 */
class CSAutenticacao : public IServicoAutenticacao {
private:
    /** @brief Ponteiro para o conteiner de pessoas. */
    ContainerPessoas* pessoas;
    /** @brief Ponteiro para a sessao compartilhada. */
    Sessao* sessao;

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container onde as contas estao guardadas.
     * @param sessao Sessao compartilhada com as outras controladoras.
     */
    CSAutenticacao(ContainerPessoas* pessoas, Sessao* sessao)
        : pessoas(pessoas), sessao(sessao) {}

    // Decisao de projeto: se a autenticacao falha, a sessao anterior tambem e
    //   encerrada. Assim, depois de autenticar(), ha alguem autenticado se e
    //   so se devolveu true.
    /**
     * @brief Autentica uma pessoa por email e senha.
     * @param email Email da conta.
     * @param senha Senha da conta.
     * @return true se o email existe e a senha confere (a pessoa passa a ser o
     *         usuario autenticado); false caso contrario.
     */
    bool autenticar(const Email& email, const Senha& senha) const override {
        const Pessoa* pessoa = pessoas->pesquisar(email);
        if (pessoa == nullptr || pessoa->getSenha().getValor() != senha.getValor()) {
            sessao->encerrar();
            return false;
        }
        sessao->iniciar(email);
        return true;
    }

    /**
     * @brief Encerra a sessao (logout); depois disso ninguem esta autenticado.
     */
    void encerrarSessao() override {
        sessao->encerrar();
    }
};

/**
 * @brief Controladora dos servicos 1 a 5 (CRUD e listagem de Pessoa).
 */
class CSPessoa : public IServicoPessoa {
private:
    /** @brief Ponteiro para o conteiner de pessoas. */
    ContainerPessoas* pessoas;
    /** @brief Ponteiro para o conteiner de projetos. */
    ContainerProjetos* projetos;
    /** @brief Ponteiro para o conteiner de cartoes. */
    ContainerCartoes* cartoes;
    /** @brief Ponteiro para a sessao compartilhada. */
    Sessao* sessao;

    /**
     * @brief Verifica se a pessoa e gestora de projetos ou desenvolvedora de cartoes.
     * @param email Email da pessoa a verificar.
     * @return true se possui vinculos ativos; false caso contrario.
     */
    bool temVinculos(const Email& email) const {
        return projetos->contarPorGestor(email) > 0 ||
               cartoes->contarPorDesenvolvedor(email) > 0;
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas.
     * @param projetos Container de projetos (para conferir vinculos de gestor).
     * @param cartoes Container de cartoes (para conferir vinculos de desenvolvedor).
     * @param sessao Sessao compartilhada com as outras controladoras.
     */
    CSPessoa(ContainerPessoas* pessoas, ContainerProjetos* projetos,
             ContainerCartoes* cartoes, Sessao* sessao)
        : pessoas(pessoas), projetos(projetos), cartoes(cartoes), sessao(sessao) {}

    // Decisao de projeto: criar nao exige autenticacao, pois a conta nasce antes
    //   do login (enunciado, secao 2).
    /**
     * @brief Cria uma conta.
     * @param pessoa Dados da conta.
     * @throw std::logic_error Se ja existe pessoa com o mesmo email.
     */
    void criar(const Pessoa& pessoa) override {
        if (!pessoas->incluir(pessoa)) {
            throw std::logic_error("Ja existe pessoa com este email.");
        }
    }

    // Decisao de projeto: qualquer pessoa autenticada le qualquer pessoa (o
    //   enunciado libera LER PESSOA para os dois papeis).
    // Decisao de projeto: o servico devolve a entidade completa (inclusive a
    //   senha, que e atributo da entidade); a camada de apresentacao nao exibe
    //   a senha ao mostrar os dados lidos.
    /**
     * @brief Le os dados de uma pessoa.
     * @param email Email da pessoa.
     * @return Copia da pessoa.
     * @throw std::logic_error Se ninguem esta autenticado ou a pessoa nao existe.
     */
    Pessoa ler(const Email& email) const override {
        sessao->exigirUsuario(*pessoas);
        const Pessoa* pessoa = pessoas->pesquisar(email);
        if (pessoa == nullptr) {
            throw std::logic_error("Pessoa nao encontrada.");
        }
        return *pessoa;
    }

    // Decisao de projeto: so a propria pessoa atualiza seus dados.
    // Decisao de projeto: o papel so pode mudar se a pessoa nao tiver vinculos
    //   (gestor de projeto ou desenvolvedor de cartao), para nao deixar o
    //   sistema inconsistente.
    /**
     * @brief Atualiza os dados da propria pessoa autenticada.
     * @param pessoa Novos dados (o email identifica a pessoa e nao muda).
     * @throw std::logic_error Se ninguem esta autenticado, se o email nao e o do
     *        usuario autenticado ou se o papel muda havendo vinculos.
     */
    void atualizar(const Pessoa& pessoa) override {
        Pessoa usuario = sessao->exigirUsuario(*pessoas);
        if (usuario.getEmail().getValor() != pessoa.getEmail().getValor()) {
            throw std::logic_error("Somente a propria pessoa pode atualizar seus dados.");
        }
        if (usuario.getPapel().getValor() != pessoa.getPapel().getValor() &&
            temVinculos(usuario.getEmail())) {
            throw std::logic_error("Pessoa com projetos ou cartoes nao pode mudar de papel.");
        }
        pessoas->atualizar(pessoa);
    }

    // Decisao de projeto: so a propria pessoa exclui a propria conta, e so se
    //   nao tiver vinculos. A exclusao e bloqueada (e nao feita em cascata)
    //   para nao apagar dados de outras pessoas sem aviso e para nao deixar
    //   projeto sem gestor ou cartao sem desenvolvedor (requisito 15 do
    //   enunciado). A sessao e encerrada junto.
    /**
     * @brief Exclui a propria conta da pessoa autenticada.
     * @param email Email da conta a excluir.
     * @throw std::logic_error Se ninguem esta autenticado, se o email nao e o do
     *        usuario autenticado ou se a pessoa e gestor de projeto ou
     *        desenvolvedor de cartao.
     */
    void excluir(const Email& email) override {
        Pessoa usuario = sessao->exigirUsuario(*pessoas);
        if (usuario.getEmail().getValor() != email.getValor()) {
            throw std::logic_error("Somente a propria pessoa pode excluir sua conta.");
        }
        if (temVinculos(email)) {
            throw std::logic_error("Pessoa com projetos ou cartoes nao pode ser excluida.");
        }
        pessoas->remover(email);
        sessao->encerrar();
    }

    /**
     * @brief Lista todas as pessoas.
     * @return Copias das pessoas, em ordem de email.
     * @throw std::logic_error Se ninguem esta autenticado.
     */
    std::vector<Pessoa> listar() const override {
        sessao->exigirUsuario(*pessoas);
        return pessoas->listar();
    }
};

#endif // CONTROLADORAS_PESSOA_H_INCLUDED
