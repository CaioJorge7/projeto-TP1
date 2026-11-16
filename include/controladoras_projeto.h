#ifndef CONTROLADORAS_PROJETO_H_INCLUDED
#define CONTROLADORAS_PROJETO_H_INCLUDED

#include <vector>
#include "dominios.h"
#include "containers.h"
#include "provisorio/interfaces.h"

// TODO(Membro2): o include de provisorio/interfaces.h aponta para as interfaces
// PROVISORIAS. Trocar quando as oficiais existirem.

/**
 * @brief Controladora dos servicos 6 a 10 (CRUD e listagem de Projeto).
 */
class CSProjeto : public ISProjeto {
private:
    ContainerPessoas* pessoas;
    ContainerProjetos* projetos;
    ContainerQuadros* quadros;

    bool solicitanteExiste(const Email& solicitante) const {
        Pessoa pessoa;
        return pessoas->pesquisar(solicitante, &pessoa);
    }

    bool ehGestor(const Email& solicitante) const {
        Pessoa pessoa;
        if (!pessoas->pesquisar(solicitante, &pessoa)) return false;
        return pessoa.getPapel().getValor() == "GESTOR";
    }

    // Verdadeiro se o solicitante e GESTOR e e o gestor deste projeto.
    bool ehGestorDoProjeto(const Email& solicitante, const Identificador& id) const {
        if (!ehGestor(solicitante)) return false;
        Email gestor;
        if (!projetos->pesquisarGestor(id, &gestor)) return false;
        return gestor.getValor() == solicitante.getValor();
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas (para conferir existencia e papel).
     * @param projetos Container de projetos.
     * @param quadros Container de quadros (para impedir excluir projeto com quadros).
     */
    CSProjeto(ContainerPessoas* pessoas, ContainerProjetos* projetos, ContainerQuadros* quadros)
        : pessoas(pessoas), projetos(projetos), quadros(quadros) {}

    // ESCOLHA(Membro3): so GESTOR cria projeto, e o gestor do projeto e quem o
    //   criou (como diz o enunciado). Falha se o identificador ja existe.
    bool criar(const Email& solicitante, const Projeto& projeto) override {
        if (!ehGestor(solicitante)) return false;
        return projetos->incluir(projeto, solicitante);
    }

    // ESCOLHA(Membro3): qualquer pessoa logada le qualquer projeto
    //   (o enunciado libera LER PROJETO para os dois papeis).
    bool ler(const Email& solicitante, const Identificador& id, Projeto* projeto) override {
        if (!solicitanteExiste(solicitante)) return false;
        return projetos->pesquisar(id, projeto);
    }

    // ESCOLHA(Membro3): so o gestor DO projeto atualiza (nao basta ser GESTOR).
    //   O identificador (chave primaria) nao muda: a busca e feita por ele.
    //   TODO(Membro2): conferir se ha regra sobre inicio/termino (ex.: termino
    //   depois do inicio). Aqui nao foi validado.
    bool atualizar(const Email& solicitante, const Projeto& projeto) override {
        if (!ehGestorDoProjeto(solicitante, projeto.getIdentificador())) return false;
        return projetos->atualizar(projeto);
    }

    // ESCOLHA(Membro3): so o gestor DO projeto exclui, e so se o projeto nao
    //   tiver quadros. Bloquear (em vez de apagar em cascata) evita
    //   inconsistencia (requisito 15). TODO(Membro2): se preferir cascata,
    //   mudar aqui.
    bool excluir(const Email& solicitante, const Identificador& id) override {
        if (!ehGestorDoProjeto(solicitante, id)) return false;
        if (!quadros->listarPorProjeto(id).empty()) return false;
        return projetos->remover(id);
    }

    // ESCOLHA(Membro3): listar devolve os identificadores de todos os projetos
    //   para qualquer pessoa logada.
    //   TODO(Membro2): decidir se a lista deve ser filtrada por pessoa.
    bool listar(const Email& solicitante, vector<Identificador>* ids) override {
        if (!solicitanteExiste(solicitante)) return false;
        *ids = projetos->listar();
        return true;
    }
};

#endif // CONTROLADORAS_PROJETO_H_INCLUDED
