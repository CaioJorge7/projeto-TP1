#ifndef CONTROLADORAS_QUADRO_H_INCLUDED
#define CONTROLADORAS_QUADRO_H_INCLUDED

#include <vector>
#include "dominios.h"
#include "containers.h"
#include "provisorio/interfaces.h"

// TODO(Membro2): o include de provisorio/interfaces.h aponta para as interfaces
// PROVISORIAS. Trocar quando as oficiais existirem.

/**
 * @brief Controladora dos servicos 11 a 15 (CRUD e listagem de Quadro).
 */
class CSQuadro : public ISQuadro {
private:
    ContainerPessoas* pessoas;
    ContainerProjetos* projetos;
    ContainerQuadros* quadros;
    ContainerCartoes* cartoes;

    bool solicitanteExiste(const Email& solicitante) const {
        Pessoa pessoa;
        return pessoas->pesquisar(solicitante, &pessoa);
    }

    // Verdadeiro se o solicitante e GESTOR e e o gestor deste projeto.
    // Tambem e falso se o projeto nao existe.
    bool ehGestorDoProjeto(const Email& solicitante, const Identificador& idProjeto) const {
        Pessoa pessoa;
        if (!pessoas->pesquisar(solicitante, &pessoa)) return false;
        if (pessoa.getPapel().getValor() != "GESTOR") return false;
        Email gestor;
        if (!projetos->pesquisarGestor(idProjeto, &gestor)) return false;
        return gestor.getValor() == solicitante.getValor();
    }

    // Quantidade de cartoes do quadro que estao na coluna FAZENDO (o WIP).
    int contarFazendo(const Identificador& idQuadro) const {
        int total = 0;
        vector<Identificador> ids = cartoes->listarPorQuadro(idQuadro);
        for (size_t i = 0; i < ids.size(); ++i) {
            Cartao cartao;
            if (cartoes->pesquisar(ids[i], &cartao) &&
                cartao.getEstado().getValor() == "FAZENDO") {
                ++total;
            }
        }
        return total;
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas (para conferir existencia e papel).
     * @param projetos Container de projetos (para conferir o gestor).
     * @param quadros Container de quadros.
     * @param cartoes Container de cartoes (para conferir WIP e exclusao).
     */
    CSQuadro(ContainerPessoas* pessoas, ContainerProjetos* projetos,
             ContainerQuadros* quadros, ContainerCartoes* cartoes)
        : pessoas(pessoas), projetos(projetos), quadros(quadros), cartoes(cartoes) {}

    // ESCOLHA(Membro3): so o gestor DO projeto cria quadro nele (nao basta ser
    //   GESTOR). Falha se o projeto nao existe ou se o identificador do quadro
    //   ja existe. Assim a multiplicidade 1 projeto : 0..* quadros e mantida.
    bool criar(const Email& solicitante, const Identificador& projeto, const Quadro& quadro) override {
        if (!ehGestorDoProjeto(solicitante, projeto)) return false;
        return quadros->incluir(quadro, projeto);
    }

    // ESCOLHA(Membro3): qualquer pessoa logada le qualquer quadro.
    bool ler(const Email& solicitante, const Identificador& id, Quadro* quadro) override {
        if (!solicitanteExiste(solicitante)) return false;
        return quadros->pesquisar(id, quadro);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro atualiza. O
    //   identificador (chave primaria) e o projeto do quadro nao mudam.
    // ESCOLHA(Membro3): o novo limite nao pode ser menor que o WIP atual
    //   (cartoes em FAZENDO), senao o quadro ficaria acima do limite.
    bool atualizar(const Email& solicitante, const Quadro& quadro) override {
        Identificador projeto;
        if (!quadros->pesquisarProjeto(quadro.getIdentificador(), &projeto)) return false;
        if (!ehGestorDoProjeto(solicitante, projeto)) return false;
        if (quadro.getLimite().getValor() < contarFazendo(quadro.getIdentificador())) return false;
        return quadros->atualizar(quadro);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro exclui, e so se o
    //   quadro nao tiver cartoes (evita inconsistencia, requisito 15).
    //   TODO(Membro2): se preferir cascata, mudar aqui.
    bool excluir(const Email& solicitante, const Identificador& id) override {
        Identificador projeto;
        if (!quadros->pesquisarProjeto(id, &projeto)) return false;
        if (!ehGestorDoProjeto(solicitante, projeto)) return false;
        if (!cartoes->listarPorQuadro(id).empty()) return false;
        return quadros->remover(id);
    }

    // ESCOLHA(Membro3): listar devolve os identificadores de todos os quadros
    //   para qualquer pessoa logada.
    //   TODO(Membro2): decidir se deve listar so os quadros de um projeto.
    bool listar(const Email& solicitante, vector<Identificador>* ids) override {
        if (!solicitanteExiste(solicitante)) return false;
        *ids = quadros->listar();
        return true;
    }
};

#endif // CONTROLADORAS_QUADRO_H_INCLUDED
