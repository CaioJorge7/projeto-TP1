#ifndef SESSAO_H_INCLUDED
#define SESSAO_H_INCLUDED

#include <stdexcept>
#include "dominios.h"
#include "entidades.h"
#include "containers.h"

/**
 * @file sessao.h
 * @brief Guarda quem esta autenticado, para as controladoras conferirem o papel.
 */

// Decisao de projeto: as interfaces de servico nao recebem o usuario em cada
//   chamada. Por isso existe uma Sessao, compartilhada (por ponteiro) entre
//   CSAutenticacao e as outras controladoras: autenticar() guarda o email, e os
//   demais servicos consultam a Sessao para saber quem e e qual o papel.
// Decisao de projeto: o papel nao e guardado na Sessao; e lido do container de
//   pessoas a cada chamada. Assim, se a conta mudar ou sumir, a Sessao nao fica
//   desatualizada.
// Decisao de projeto: o logout faz parte do contrato IServicoAutenticacao
//   (encerrarSessao()), para que a apresentacao encerre a sessao sem conhecer
//   a classe concreta CSAutenticacao.

/**
 * @brief Usuario autenticado no momento (no maximo um).
 */
class Sessao {
private:
    /** @brief Indica se existe um usuario autenticado no momento. */
    bool ativa;
    /** @brief Email do usuario autenticado no momento. */
    Email usuario;

public:
    /** @brief Cria uma sessao sem ninguem autenticado. */
    Sessao() : ativa(false) {}

    /**
     * @brief Registra quem acabou de se autenticar.
     * @param email Email da pessoa autenticada.
     */
    void iniciar(const Email& email) {
        usuario = email;
        ativa = true;
    }

    /** @brief Encerra a sessao; depois disso ninguem esta autenticado. */
    void encerrar() {
        usuario = Email();
        ativa = false;
    }

    /** @brief Informa se ha alguem autenticado. */
    bool estaAtiva() const { return ativa; }

    /**
     * @brief Devolve a pessoa autenticada.
     * @param pessoas Container onde a conta esta guardada.
     * @return Copia da pessoa autenticada.
     * @throw std::logic_error Se ninguem esta autenticado ou a conta deixou de existir.
     */
    Pessoa exigirUsuario(const ContainerPessoas& pessoas) const {
        if (!ativa) {
            throw std::logic_error("Nenhum usuario autenticado.");
        }
        const Pessoa* pessoa = pessoas.pesquisar(usuario);
        if (pessoa == nullptr) {
            throw std::logic_error("A conta do usuario autenticado nao existe mais.");
        }
        return *pessoa;
    }

    /**
     * @brief Devolve a pessoa autenticada, exigindo que seja GESTOR.
     * @param pessoas Container onde a conta esta guardada.
     * @return Copia da pessoa autenticada.
     * @throw std::logic_error Se ninguem esta autenticado, a conta nao existe mais
     *        ou o papel nao e GESTOR.
     */
    Pessoa exigirGestor(const ContainerPessoas& pessoas) const {
        Pessoa pessoa = exigirUsuario(pessoas);
        if (pessoa.getPapel().getValor() != "GESTOR") {
            throw std::logic_error("Servico permitido somente para GESTOR.");
        }
        return pessoa;
    }
};

#endif // SESSAO_H_INCLUDED
