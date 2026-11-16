#ifndef CONTAINERS_H_INCLUDED
#define CONTAINERS_H_INCLUDED

#include <map>
#include <string>
#include <vector>
#include "dominios.h"
#include "provisorio/entidades.h"

// TODO(Membro2): o include acima aponta para as entidades PROVISORIAS.
// Quando as entidades oficiais existirem, trocar o caminho.
//
// ESCOLHA(Membro3): persistencia em memoria com std::map (permitido pelo
//   enunciado, requisito 23). Cada container guarda um tipo de entidade,
//   indexado pela chave primaria em texto (getValor()).
// ESCOLHA(Membro3): os vinculos do diagrama (gestor do projeto, projeto do
//   quadro, quadro e desenvolvedor do cartao) ficam guardados nos containers,
//   em mapas separados, porque as entidades nao tem atributo para isso.
//   Se o Membro 2 colocar esses vinculos dentro das entidades, os mapas de
//   vinculo podem sair.
// ESCOLHA(Membro3): containers so guardam e buscam. As regras de negocio
//   (papel, multiplicidade, WIP, exclusao) ficam nas controladoras.
// ESCOLHA(Membro3): incluir devolve false se a chave ja existe; pesquisar,
//   atualizar e remover devolvem false se a chave nao existe.
// ESCOLHA(Membro3): atualizar troca todos os dados da entidade pela chave, mas
//   nao mexe nos vinculos.

/**
 * @brief Container de Pessoas (chave: email).
 */
class ContainerPessoas {
private:
    map<string, Pessoa> pessoas;

public:
    bool incluir(const Pessoa& pessoa) {
        string chave = pessoa.getEmail().getValor();
        if (pessoas.count(chave) > 0) return false;
        pessoas[chave] = pessoa;
        return true;
    }

    bool remover(const Email& email) {
        return pessoas.erase(email.getValor()) > 0;
    }

    bool pesquisar(const Email& email, Pessoa* pessoa) const {
        auto it = pessoas.find(email.getValor());
        if (it == pessoas.end()) return false;
        *pessoa = it->second;
        return true;
    }

    bool atualizar(const Pessoa& pessoa) {
        auto it = pessoas.find(pessoa.getEmail().getValor());
        if (it == pessoas.end()) return false;
        it->second = pessoa;
        return true;
    }

    vector<Email> listar() const {
        vector<Email> emails;
        for (auto it = pessoas.begin(); it != pessoas.end(); ++it) {
            Email e;
            e.setValor(it->first);
            emails.push_back(e);
        }
        return emails;
    }
};

/**
 * @brief Container de Projetos (chave: identificador). Guarda tambem o gestor de cada projeto.
 */
class ContainerProjetos {
private:
    map<string, Projeto> projetos;
    map<string, string> gestores;   // id do projeto -> email do gestor

public:
    bool incluir(const Projeto& projeto, const Email& gestor) {
        string chave = projeto.getIdentificador().getValor();
        if (projetos.count(chave) > 0) return false;
        projetos[chave] = projeto;
        gestores[chave] = gestor.getValor();
        return true;
    }

    bool remover(const Identificador& id) {
        string chave = id.getValor();
        if (projetos.erase(chave) == 0) return false;
        gestores.erase(chave);
        return true;
    }

    bool pesquisar(const Identificador& id, Projeto* projeto) const {
        auto it = projetos.find(id.getValor());
        if (it == projetos.end()) return false;
        *projeto = it->second;
        return true;
    }

    bool atualizar(const Projeto& projeto) {
        auto it = projetos.find(projeto.getIdentificador().getValor());
        if (it == projetos.end()) return false;
        it->second = projeto;
        return true;
    }

    vector<Identificador> listar() const {
        vector<Identificador> ids;
        for (auto it = projetos.begin(); it != projetos.end(); ++it) {
            Identificador id;
            id.setValor(it->first);
            ids.push_back(id);
        }
        return ids;
    }

    bool pesquisarGestor(const Identificador& id, Email* gestor) const {
        auto it = gestores.find(id.getValor());
        if (it == gestores.end()) return false;
        gestor->setValor(it->second);
        return true;
    }

    int contarPorGestor(const Email& gestor) const {
        int total = 0;
        string email = gestor.getValor();
        for (auto it = gestores.begin(); it != gestores.end(); ++it) {
            if (it->second == email) ++total;
        }
        return total;
    }
};

/**
 * @brief Container de Quadros (chave: identificador). Guarda tambem o projeto de cada quadro.
 */
class ContainerQuadros {
private:
    map<string, Quadro> quadros;
    map<string, string> projetos;   // id do quadro -> id do projeto

public:
    bool incluir(const Quadro& quadro, const Identificador& projeto) {
        string chave = quadro.getIdentificador().getValor();
        if (quadros.count(chave) > 0) return false;
        quadros[chave] = quadro;
        projetos[chave] = projeto.getValor();
        return true;
    }

    bool remover(const Identificador& id) {
        string chave = id.getValor();
        if (quadros.erase(chave) == 0) return false;
        projetos.erase(chave);
        return true;
    }

    bool pesquisar(const Identificador& id, Quadro* quadro) const {
        auto it = quadros.find(id.getValor());
        if (it == quadros.end()) return false;
        *quadro = it->second;
        return true;
    }

    bool atualizar(const Quadro& quadro) {
        auto it = quadros.find(quadro.getIdentificador().getValor());
        if (it == quadros.end()) return false;
        it->second = quadro;
        return true;
    }

    vector<Identificador> listar() const {
        vector<Identificador> ids;
        for (auto it = quadros.begin(); it != quadros.end(); ++it) {
            Identificador id;
            id.setValor(it->first);
            ids.push_back(id);
        }
        return ids;
    }

    bool pesquisarProjeto(const Identificador& id, Identificador* projeto) const {
        auto it = projetos.find(id.getValor());
        if (it == projetos.end()) return false;
        projeto->setValor(it->second);
        return true;
    }

    vector<Identificador> listarPorProjeto(const Identificador& projeto) const {
        vector<Identificador> ids;
        string idProjeto = projeto.getValor();
        for (auto it = projetos.begin(); it != projetos.end(); ++it) {
            if (it->second == idProjeto) {
                Identificador id;
                id.setValor(it->first);
                ids.push_back(id);
            }
        }
        return ids;
    }
};

/**
 * @brief Container de Cartoes (chave: identificador). Guarda tambem o quadro e o desenvolvedor de cada cartao.
 */
class ContainerCartoes {
private:
    map<string, Cartao> cartoes;
    map<string, string> quadros;          // id do cartao -> id do quadro
    map<string, string> desenvolvedores;  // id do cartao -> email do desenvolvedor

public:
    bool incluir(const Cartao& cartao, const Identificador& quadro, const Email& desenvolvedor) {
        string chave = cartao.getIdentificador().getValor();
        if (cartoes.count(chave) > 0) return false;
        cartoes[chave] = cartao;
        quadros[chave] = quadro.getValor();
        desenvolvedores[chave] = desenvolvedor.getValor();
        return true;
    }

    bool remover(const Identificador& id) {
        string chave = id.getValor();
        if (cartoes.erase(chave) == 0) return false;
        quadros.erase(chave);
        desenvolvedores.erase(chave);
        return true;
    }

    bool pesquisar(const Identificador& id, Cartao* cartao) const {
        auto it = cartoes.find(id.getValor());
        if (it == cartoes.end()) return false;
        *cartao = it->second;
        return true;
    }

    bool atualizar(const Cartao& cartao) {
        auto it = cartoes.find(cartao.getIdentificador().getValor());
        if (it == cartoes.end()) return false;
        it->second = cartao;
        return true;
    }

    vector<Identificador> listar() const {
        vector<Identificador> ids;
        for (auto it = cartoes.begin(); it != cartoes.end(); ++it) {
            Identificador id;
            id.setValor(it->first);
            ids.push_back(id);
        }
        return ids;
    }

    bool pesquisarQuadro(const Identificador& id, Identificador* quadro) const {
        auto it = quadros.find(id.getValor());
        if (it == quadros.end()) return false;
        quadro->setValor(it->second);
        return true;
    }

    bool pesquisarDesenvolvedor(const Identificador& id, Email* desenvolvedor) const {
        auto it = desenvolvedores.find(id.getValor());
        if (it == desenvolvedores.end()) return false;
        desenvolvedor->setValor(it->second);
        return true;
    }

    vector<Identificador> listarPorQuadro(const Identificador& quadro) const {
        vector<Identificador> ids;
        string idQuadro = quadro.getValor();
        for (auto it = quadros.begin(); it != quadros.end(); ++it) {
            if (it->second == idQuadro) {
                Identificador id;
                id.setValor(it->first);
                ids.push_back(id);
            }
        }
        return ids;
    }

    int contarPorDesenvolvedor(const Email& desenvolvedor) const {
        int total = 0;
        string email = desenvolvedor.getValor();
        for (auto it = desenvolvedores.begin(); it != desenvolvedores.end(); ++it) {
            if (it->second == email) ++total;
        }
        return total;
    }
};

#endif // CONTAINERS_H_INCLUDED
