#ifndef CONTAINERS_H_INCLUDED
#define CONTAINERS_H_INCLUDED

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "dominios.h"
#include "entidades.h"

/**
 * @file containers.h
 * @brief Containers em memoria da camada de servico (um por entidade).
 *
 * Os containers so guardam e buscam entidades. As regras de negocio (papel,
 * multiplicidade, WIP, exclusao) ficam nas controladoras.
 */

// ESCOLHA(Membro3): persistencia em memoria com std::map (permitido pelo
//   enunciado, requisito 23). Cada container guarda um tipo de entidade,
//   indexado pela chave primaria em texto (getValor()).
// ESCOLHA(Membro3): as entidades do Membro 2 ja trazem os vinculos (gestorEmail,
//   projetoId, quadroId, desenvolvedorEmail), entao os containers nao guardam
//   vinculos a parte. As buscas por vinculo (contar...) varrem o mapa.
// ESCOLHA(Membro3): as entidades nao tem construtor padrao, entao pesquisar
//   devolve ponteiro (nullptr se nao existe) em vez de preencher um objeto.
//   O ponteiro vale ate o proximo incluir/remover; copie o objeto se precisar
//   guardar.
// ESCOLHA(Membro3): incluir devolve false se a chave ja existe; remover e
//   atualizar devolvem false se a chave nao existe.

/**
 * @brief Container de Pessoas, indexado pelo email.
 */
class ContainerPessoas {
private:
    /** @brief Mapa de pessoas indexado pela string do email. */
    map<string, Pessoa> pessoas;

public:
    /**
     * @brief Guarda uma nova pessoa.
     * @param pessoa Pessoa a guardar.
     * @return true se guardou; false se ja existe pessoa com o mesmo email.
     */
    bool incluir(const Pessoa& pessoa) {
        return pessoas.insert(make_pair(pessoa.getEmail().getValor(), pessoa)).second;
    }

    /**
     * @brief Remove a pessoa com o email informado.
     * @param email Email da pessoa.
     * @return true se removeu; false se nao existe.
     */
    bool remover(const Email& email) {
        return pessoas.erase(email.getValor()) > 0;
    }

    /**
     * @brief Procura uma pessoa pelo email.
     * @param email Email da pessoa.
     * @return Ponteiro para a pessoa guardada, ou nullptr se nao existe.
     */
    const Pessoa* pesquisar(const Email& email) const {
        map<string, Pessoa>::const_iterator it = pessoas.find(email.getValor());
        return it == pessoas.end() ? nullptr : &it->second;
    }

    /**
     * @brief Troca os dados da pessoa que tem o mesmo email.
     * @param pessoa Novos dados.
     * @return true se atualizou; false se nao existe pessoa com este email.
     */
    bool atualizar(const Pessoa& pessoa) {
        map<string, Pessoa>::iterator it = pessoas.find(pessoa.getEmail().getValor());
        if (it == pessoas.end()) return false;
        it->second = pessoa;
        return true;
    }

    /**
     * @brief Devolve todas as pessoas guardadas, em ordem de email.
     */
    vector<Pessoa> listar() const {
        vector<Pessoa> resultado;
        for (map<string, Pessoa>::const_iterator it = pessoas.begin(); it != pessoas.end(); ++it) {
            resultado.push_back(it->second);
        }
        return resultado;
    }
};

/**
 * @brief Container de Projetos, indexado pelo identificador.
 */
class ContainerProjetos {
private:
    /** @brief Mapa de projetos indexado pela string do identificador. */
    map<string, Projeto> projetos;

public:
    /**
     * @brief Guarda um novo projeto.
     * @param projeto Projeto a guardar.
     * @return true se guardou; false se ja existe projeto com o mesmo identificador.
     */
    bool incluir(const Projeto& projeto) {
        return projetos.insert(make_pair(projeto.getIdentificador().getValor(), projeto)).second;
    }

    /**
     * @brief Remove o projeto com o identificador informado.
     * @param id Identificador do projeto.
     * @return true se removeu; false se nao existe.
     */
    bool remover(const Identificador& id) {
        return projetos.erase(id.getValor()) > 0;
    }

    /**
     * @brief Procura um projeto pelo identificador.
     * @param id Identificador do projeto.
     * @return Ponteiro para o projeto guardado, ou nullptr se nao existe.
     */
    const Projeto* pesquisar(const Identificador& id) const {
        map<string, Projeto>::const_iterator it = projetos.find(id.getValor());
        return it == projetos.end() ? nullptr : &it->second;
    }

    /**
     * @brief Troca os dados do projeto que tem o mesmo identificador.
     * @param projeto Novos dados.
     * @return true se atualizou; false se nao existe projeto com este identificador.
     */
    bool atualizar(const Projeto& projeto) {
        map<string, Projeto>::iterator it = projetos.find(projeto.getIdentificador().getValor());
        if (it == projetos.end()) return false;
        it->second = projeto;
        return true;
    }

    /**
     * @brief Devolve todos os projetos guardados, em ordem de identificador.
     */
    vector<Projeto> listar() const {
        vector<Projeto> resultado;
        for (map<string, Projeto>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
            resultado.push_back(it->second);
        }
        return resultado;
    }

    /**
     * @brief Conta os projetos de um gestor.
     * @param gestor Email do gestor.
     */
    int contarPorGestor(const Email& gestor) const {
        int total = 0;
        for (map<string, Projeto>::const_iterator it = projetos.begin(); it != projetos.end(); ++it) {
            if (it->second.getGestorEmail().getValor() == gestor.getValor()) ++total;
        }
        return total;
    }
};

/**
 * @brief Container de Quadros, indexado pelo identificador.
 */
class ContainerQuadros {
private:
    /** @brief Mapa de quadros indexado pela string do identificador. */
    map<string, Quadro> quadros;

public:
    /**
     * @brief Guarda um novo quadro.
     * @param quadro Quadro a guardar.
     * @return true se guardou; false se ja existe quadro com o mesmo identificador.
     */
    bool incluir(const Quadro& quadro) {
        return quadros.insert(make_pair(quadro.getIdentificador().getValor(), quadro)).second;
    }

    /**
     * @brief Remove o quadro com o identificador informado.
     * @param id Identificador do quadro.
     * @return true se removeu; false se nao existe.
     */
    bool remover(const Identificador& id) {
        return quadros.erase(id.getValor()) > 0;
    }

    /**
     * @brief Procura um quadro pelo identificador.
     * @param id Identificador do quadro.
     * @return Ponteiro para o quadro guardado, ou nullptr se nao existe.
     */
    const Quadro* pesquisar(const Identificador& id) const {
        map<string, Quadro>::const_iterator it = quadros.find(id.getValor());
        return it == quadros.end() ? nullptr : &it->second;
    }

    /**
     * @brief Troca os dados do quadro que tem o mesmo identificador.
     * @param quadro Novos dados.
     * @return true se atualizou; false se nao existe quadro com este identificador.
     */
    bool atualizar(const Quadro& quadro) {
        map<string, Quadro>::iterator it = quadros.find(quadro.getIdentificador().getValor());
        if (it == quadros.end()) return false;
        it->second = quadro;
        return true;
    }

    /**
     * @brief Devolve todos os quadros guardados, em ordem de identificador.
     */
    vector<Quadro> listar() const {
        vector<Quadro> resultado;
        for (map<string, Quadro>::const_iterator it = quadros.begin(); it != quadros.end(); ++it) {
            resultado.push_back(it->second);
        }
        return resultado;
    }

    /**
     * @brief Conta os quadros de um projeto.
     * @param projeto Identificador do projeto.
     */
    int contarPorProjeto(const Identificador& projeto) const {
        int total = 0;
        for (map<string, Quadro>::const_iterator it = quadros.begin(); it != quadros.end(); ++it) {
            if (it->second.getProjetoId().getValor() == projeto.getValor()) ++total;
        }
        return total;
    }
};

/**
 * @brief Container de Cartoes de atividade, indexado pelo identificador.
 */
class ContainerCartoes {
private:
    /** @brief Mapa de cartoes indexado pela string do identificador. */
    map<string, CartaoAtividade> cartoes;

public:
    /**
     * @brief Guarda um novo cartao.
     * @param cartao Cartao a guardar.
     * @return true se guardou; false se ja existe cartao com o mesmo identificador.
     */
    bool incluir(const CartaoAtividade& cartao) {
        return cartoes.insert(make_pair(cartao.getIdentificador().getValor(), cartao)).second;
    }

    /**
     * @brief Remove o cartao com o identificador informado.
     * @param id Identificador do cartao.
     * @return true se removeu; false se nao existe.
     */
    bool remover(const Identificador& id) {
        return cartoes.erase(id.getValor()) > 0;
    }

    /**
     * @brief Procura um cartao pelo identificador.
     * @param id Identificador do cartao.
     * @return Ponteiro para o cartao guardado, ou nullptr se nao existe.
     */
    const CartaoAtividade* pesquisar(const Identificador& id) const {
        map<string, CartaoAtividade>::const_iterator it = cartoes.find(id.getValor());
        return it == cartoes.end() ? nullptr : &it->second;
    }

    /**
     * @brief Troca os dados do cartao que tem o mesmo identificador.
     * @param cartao Novos dados.
     * @return true se atualizou; false se nao existe cartao com este identificador.
     */
    bool atualizar(const CartaoAtividade& cartao) {
        map<string, CartaoAtividade>::iterator it = cartoes.find(cartao.getIdentificador().getValor());
        if (it == cartoes.end()) return false;
        it->second = cartao;
        return true;
    }

    /**
     * @brief Devolve todos os cartoes guardados, em ordem de identificador.
     */
    vector<CartaoAtividade> listar() const {
        vector<CartaoAtividade> resultado;
        for (map<string, CartaoAtividade>::const_iterator it = cartoes.begin(); it != cartoes.end(); ++it) {
            resultado.push_back(it->second);
        }
        return resultado;
    }

    /**
     * @brief Devolve os cartoes de um quadro.
     * @param quadro Identificador do quadro.
     */
    vector<CartaoAtividade> listarPorQuadro(const Identificador& quadro) const {
        vector<CartaoAtividade> resultado;
        for (map<string, CartaoAtividade>::const_iterator it = cartoes.begin(); it != cartoes.end(); ++it) {
            if (it->second.getQuadroId().getValor() == quadro.getValor()) {
                resultado.push_back(it->second);
            }
        }
        return resultado;
    }

    /**
     * @brief Conta os cartoes de um quadro.
     * @param quadro Identificador do quadro.
     */
    int contarPorQuadro(const Identificador& quadro) const {
        int total = 0;
        for (map<string, CartaoAtividade>::const_iterator it = cartoes.begin(); it != cartoes.end(); ++it) {
            if (it->second.getQuadroId().getValor() == quadro.getValor()) ++total;
        }
        return total;
    }

    /**
     * @brief Conta os cartoes de um quadro que estao em um estado.
     * @param quadro Identificador do quadro.
     * @param estado Texto do estado ("A FAZER", "FAZENDO" ou "FEITO").
     */
    int contarPorEstado(const Identificador& quadro, const string& estado) const {
        int total = 0;
        for (map<string, CartaoAtividade>::const_iterator it = cartoes.begin(); it != cartoes.end(); ++it) {
            if (it->second.getQuadroId().getValor() == quadro.getValor() &&
                it->second.getEstado().getValor() == estado) {
                ++total;
            }
        }
        return total;
    }

    /**
     * @brief Conta os cartoes de um desenvolvedor.
     * @param desenvolvedor Email do desenvolvedor.
     */
    int contarPorDesenvolvedor(const Email& desenvolvedor) const {
        int total = 0;
        for (map<string, CartaoAtividade>::const_iterator it = cartoes.begin(); it != cartoes.end(); ++it) {
            if (it->second.getDesenvolvedorEmail().getValor() == desenvolvedor.getValor()) ++total;
        }
        return total;
    }
};

#endif // CONTAINERS_H_INCLUDED
