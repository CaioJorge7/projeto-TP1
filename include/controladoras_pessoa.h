#ifndef CONTROLADORAS_PESSOA_H_INCLUDED
#define CONTROLADORAS_PESSOA_H_INCLUDED

#include <vector>
#include "dominios.h"
#include "containers.h"
#include "provisorio/interfaces.h"

// TODO(Membro2): o include de provisorio/interfaces.h aponta para as interfaces
// PROVISORIAS. Quando as oficiais existirem, trocar o caminho e conferir se os
// nomes e as assinaturas dos metodos continuam batendo.
//
// ESCOLHA(Membro3): nomes das controladoras: "CS" (Controladora de Servico) +
//   nome do servico, espelhando o "IS" das interfaces.
// ESCOLHA(Membro3): cada controladora recebe por ponteiro os containers de que
//   precisa. Assim todas compartilham os mesmos dados em memoria.
// ESCOLHA(Membro3): o "solicitante" so e conferido quanto a existir no
//   container de pessoas. Quem garante que o solicitante e mesmo quem fez login
//   e a camada de apresentacao.
//   TODO(Membro2): confirmar essa divisao de responsabilidade.

/**
 * @brief Controladora do servico de autenticacao.
 */
class CSAutenticacao : public ISAutenticacao {
private:
    ContainerPessoas* pessoas;

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container onde as contas estao guardadas.
     */
    explicit CSAutenticacao(ContainerPessoas* pessoas) : pessoas(pessoas) {}

    /**
     * @brief Confere se existe uma pessoa com este email e esta senha.
     * @return true se o email existe e a senha confere.
     */
    bool autenticar(const Email& email, const Senha& senha) override {
        Pessoa pessoa;
        if (!pessoas->pesquisar(email, &pessoa)) return false;
        return pessoa.getSenha().getValor() == senha.getValor();
    }
};

/**
 * @brief Controladora dos servicos 1 a 5 (CRUD e listagem de Pessoa).
 */
class CSPessoa : public ISPessoa {
private:
    ContainerPessoas* pessoas;
    ContainerProjetos* projetos;
    ContainerCartoes* cartoes;

    bool solicitanteExiste(const Email& solicitante) const {
        Pessoa pessoa;
        return pessoas->pesquisar(solicitante, &pessoa);
    }

    // Pessoa que e gestor de projeto ou desenvolvedor de cartao tem vinculos.
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
     */
    CSPessoa(ContainerPessoas* pessoas, ContainerProjetos* projetos, ContainerCartoes* cartoes)
        : pessoas(pessoas), projetos(projetos), cartoes(cartoes) {}

    // ESCOLHA(Membro3): criar nao confere solicitante, pois a conta nasce antes
    //   do login. Falha se ja existe pessoa com o mesmo email.
    bool criar(const Pessoa& pessoa) override {
        return pessoas->incluir(pessoa);
    }

    // ESCOLHA(Membro3): qualquer pessoa logada pode ler os dados de qualquer
    //   pessoa (o enunciado libera LER PESSOA para os dois papeis).
    //   TODO(Membro2): decidir se a senha deve aparecer na leitura.
    bool ler(const Email& solicitante, const Email& email, Pessoa* pessoa) override {
        if (!solicitanteExiste(solicitante)) return false;
        return pessoas->pesquisar(email, pessoa);
    }

    // ESCOLHA(Membro3): so a propria pessoa atualiza seus dados. O email
    //   (chave primaria) nao muda: a busca e feita por ele.
    // ESCOLHA(Membro3): o papel so pode mudar se a pessoa nao tiver vinculos
    //   (gestor de projeto ou desenvolvedor de cartao), para nao deixar o
    //   sistema inconsistente.
    bool atualizar(const Email& solicitante, const Pessoa& pessoa) override {
        if (solicitante.getValor() != pessoa.getEmail().getValor()) return false;
        Pessoa atual;
        if (!pessoas->pesquisar(solicitante, &atual)) return false;
        if (atual.getPapel().getValor() != pessoa.getPapel().getValor() &&
            temVinculos(solicitante)) {
            return false;
        }
        return pessoas->atualizar(pessoa);
    }

    // ESCOLHA(Membro3): so a propria pessoa exclui a propria conta, e so se nao
    //   tiver vinculos. Bloquear (em vez de apagar em cascata) evita
    //   inconsistencia (requisito 15 do enunciado).
    //   TODO(Membro2): se preferir cascata, mudar aqui.
    bool excluir(const Email& solicitante, const Email& email) override {
        if (solicitante.getValor() != email.getValor()) return false;
        if (!solicitanteExiste(solicitante)) return false;
        if (temVinculos(email)) return false;
        return pessoas->remover(email);
    }

    // ESCOLHA(Membro3): listar devolve so os emails (identificadores), como o
    //   enunciado pede.
    bool listar(const Email& solicitante, vector<Email>* emails) override {
        if (!solicitanteExiste(solicitante)) return false;
        *emails = pessoas->listar();
        return true;
    }
};

#endif // CONTROLADORAS_PESSOA_H_INCLUDED
