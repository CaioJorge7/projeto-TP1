#ifndef CONTROLADORAS_CARTAO_H_INCLUDED
#define CONTROLADORAS_CARTAO_H_INCLUDED

#include <string>
#include <vector>
#include "dominios.h"
#include "containers.h"
#include "tempo.h"
#include "provisorio/interfaces.h"

// TODO(Membro2): o include de provisorio/interfaces.h aponta para as interfaces
// PROVISORIAS. Trocar quando as oficiais existirem.

/**
 * @brief Controladora dos servicos 16 a 26 (CRUD, listagem, movimentacao e
 *        metricas de Cartao de atividade).
 */
class CSCartao : public ISCartao {
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
    bool ehGestorDoProjeto(const Email& solicitante, const Identificador& idProjeto) const {
        Pessoa pessoa;
        if (!pessoas->pesquisar(solicitante, &pessoa)) return false;
        if (pessoa.getPapel().getValor() != "GESTOR") return false;
        Email gestor;
        if (!projetos->pesquisarGestor(idProjeto, &gestor)) return false;
        return gestor.getValor() == solicitante.getValor();
    }

    // Verdadeiro se o solicitante e o gestor do projeto a que o quadro pertence.
    // Tambem e falso se o quadro nao existe.
    bool ehGestorDoQuadro(const Email& solicitante, const Identificador& idQuadro) const {
        Identificador projeto;
        if (!quadros->pesquisarProjeto(idQuadro, &projeto)) return false;
        return ehGestorDoProjeto(solicitante, projeto);
    }

    // Quantidade de cartoes do quadro na coluna FAZENDO (o WIP).
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

    // Minutos entre dois timestamps. Falha se algum esta vazio ou se "ate" e
    // anterior a "de".
    bool minutosEntre(const Timestamp& de, const Timestamp& ate, long* minutos) const {
        long a, b;
        if (!timestampParaMinutos(de, &a) || !timestampParaMinutos(ate, &b)) return false;
        if (b < a) return false;
        *minutos = b - a;
        return true;
    }

    // Tempo de ciclo (lead = false) ou lead time (lead = true) de um cartao.
    bool lerTempo(const Email& solicitante, const Identificador& id, bool lead, int* minutos) const {
        if (!solicitanteExiste(solicitante)) return false;
        Cartao cartao;
        if (!cartoes->pesquisar(id, &cartao)) return false;
        if (cartao.getEstado().getValor() != "FEITO") return false;
        Timestamp de = lead ? cartao.getEntrada() : cartao.getInicio();
        long total;
        if (!minutosEntre(de, cartao.getTermino(), &total)) return false;
        *minutos = static_cast<int>(total);
        return true;
    }

    // Media do tempo de ciclo (lead = false) ou do lead time (lead = true) dos
    // cartoes concluidos de um quadro.
    bool lerMedia(const Email& solicitante, const Identificador& idQuadro, bool lead, double* minutos) const {
        if (!solicitanteExiste(solicitante)) return false;
        Quadro quadro;
        if (!quadros->pesquisar(idQuadro, &quadro)) return false;
        vector<Identificador> ids = cartoes->listarPorQuadro(idQuadro);
        long soma = 0;
        int concluidos = 0;
        for (size_t i = 0; i < ids.size(); ++i) {
            Cartao cartao;
            if (!cartoes->pesquisar(ids[i], &cartao)) continue;
            if (cartao.getEstado().getValor() != "FEITO") continue;
            Timestamp de = lead ? cartao.getEntrada() : cartao.getInicio();
            long total;
            if (!minutosEntre(de, cartao.getTermino(), &total)) continue;
            soma += total;
            ++concluidos;
        }
        if (concluidos == 0) return false;
        *minutos = static_cast<double>(soma) / concluidos;
        return true;
    }

public:
    /**
     * @brief Cria a controladora.
     * @param pessoas Container de pessoas.
     * @param projetos Container de projetos (para conferir o gestor).
     * @param quadros Container de quadros (para conferir existencia e limite de WIP).
     * @param cartoes Container de cartoes.
     */
    CSCartao(ContainerPessoas* pessoas, ContainerProjetos* projetos,
             ContainerQuadros* quadros, ContainerCartoes* cartoes)
        : pessoas(pessoas), projetos(projetos), quadros(quadros), cartoes(cartoes) {}

    // ESCOLHA(Membro3): so o gestor do projeto do quadro cria cartao nele.
    //   O desenvolvedor precisa existir e ter papel DESENVOLVEDOR.
    //   TODO(Membro2): confirmar se um GESTOR tambem pode ser desenvolvedor de
    //   cartao (aqui nao pode).
    // ESCOLHA(Membro3): o cartao novo sempre entra em A FAZER, com inicio e
    //   termino vazios. O momento de entrada vem preenchido em "entrada" (sem
    //   ele nao da pra calcular o lead time, entao a criacao falha).
    bool criar(const Email& solicitante, const Identificador& quadro,
               const Email& desenvolvedor, const Cartao& cartao) override {
        if (!ehGestorDoQuadro(solicitante, quadro)) return false;
        Pessoa dev;
        if (!pessoas->pesquisar(desenvolvedor, &dev)) return false;
        if (dev.getPapel().getValor() != "DESENVOLVEDOR") return false;
        long entrada;
        if (!timestampParaMinutos(cartao.getEntrada(), &entrada)) return false;

        Cartao novo = cartao;
        Estado aFazer;
        aFazer.setValor("A FAZER");
        novo.setEstado(aFazer);
        novo.setInicio(Timestamp());
        novo.setTermino(Timestamp());
        return cartoes->incluir(novo, quadro, desenvolvedor);
    }

    // ESCOLHA(Membro3): qualquer pessoa logada le qualquer cartao.
    bool ler(const Email& solicitante, const Identificador& id, Cartao* cartao) override {
        if (!solicitanteExiste(solicitante)) return false;
        return cartoes->pesquisar(id, cartao);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro atualiza. Estado e os
    //   tres timestamps nao mudam por aqui (so o "mover" mexe neles), e o
    //   identificador (chave primaria), o quadro e o desenvolvedor tambem nao.
    bool atualizar(const Email& solicitante, const Cartao& cartao) override {
        Identificador quadro;
        if (!cartoes->pesquisarQuadro(cartao.getIdentificador(), &quadro)) return false;
        if (!ehGestorDoQuadro(solicitante, quadro)) return false;
        Cartao atual;
        if (!cartoes->pesquisar(cartao.getIdentificador(), &atual)) return false;

        Cartao novo = cartao;
        novo.setEstado(atual.getEstado());
        novo.setEntrada(atual.getEntrada());
        novo.setInicio(atual.getInicio());
        novo.setTermino(atual.getTermino());
        return cartoes->atualizar(novo);
    }

    // ESCOLHA(Membro3): so o gestor do projeto do quadro exclui. Cartao nao
    //   tem nada dependendo dele, entao pode ser excluido em qualquer coluna.
    bool excluir(const Email& solicitante, const Identificador& id) override {
        Identificador quadro;
        if (!cartoes->pesquisarQuadro(id, &quadro)) return false;
        if (!ehGestorDoQuadro(solicitante, quadro)) return false;
        return cartoes->remover(id);
    }

    // ESCOLHA(Membro3): listar devolve os identificadores de todos os cartoes
    //   para qualquer pessoa logada.
    //   TODO(Membro2): decidir se deve listar so os cartoes de um quadro.
    bool listar(const Email& solicitante, vector<Identificador>* ids) override {
        if (!solicitanteExiste(solicitante)) return false;
        *ids = cartoes->listar();
        return true;
    }

    // ESCOLHA(Membro3): qualquer pessoa logada move qualquer cartao (o
    //   enunciado libera MOVER para os dois papeis).
    //   TODO(Membro2): se preferir, restringir ao desenvolvedor do cartao ou
    //   ao gestor do projeto.
    // ESCOLHA(Membro3): so valem A FAZER -> FAZENDO e FAZENDO -> FEITO.
    //   A FAZER -> FAZENDO falha se o WIP ja chegou no limite do quadro e
    //   grava "inicio". FAZENDO -> FEITO grava "termino". O momento nao pode
    //   ser anterior ao timestamp anterior do cartao (evita tempo negativo).
    bool mover(const Email& solicitante, const Identificador& id,
               const Estado& destino, const Timestamp& momento) override {
        if (!solicitanteExiste(solicitante)) return false;
        Cartao cartao;
        if (!cartoes->pesquisar(id, &cartao)) return false;
        long agora;
        if (!timestampParaMinutos(momento, &agora)) return false;

        string atual = cartao.getEstado().getValor();
        string alvo = destino.getValor();

        if (atual == "A FAZER" && alvo == "FAZENDO") {
            long entrada;
            if (!timestampParaMinutos(cartao.getEntrada(), &entrada) || agora < entrada) return false;
            Identificador idQuadro;
            Quadro quadro;
            if (!cartoes->pesquisarQuadro(id, &idQuadro)) return false;
            if (!quadros->pesquisar(idQuadro, &quadro)) return false;
            if (contarFazendo(idQuadro) >= quadro.getLimite().getValor()) return false;
            cartao.setEstado(destino);
            cartao.setInicio(momento);
        } else if (atual == "FAZENDO" && alvo == "FEITO") {
            long inicio;
            if (!timestampParaMinutos(cartao.getInicio(), &inicio) || agora < inicio) return false;
            cartao.setEstado(destino);
            cartao.setTermino(momento);
        } else {
            return false;
        }
        return cartoes->atualizar(cartao);
    }

    // ESCOLHA(Membro3): tempo de ciclo = termino - inicio (em minutos). So
    //   existe para cartao em FEITO; senao devolve false.
    bool lerTempoCiclo(const Email& solicitante, const Identificador& cartao, int* minutos) override {
        return lerTempo(solicitante, cartao, false, minutos);
    }

    // ESCOLHA(Membro3): media so dos cartoes em FEITO do quadro. Sem nenhum
    //   cartao concluido devolve false (nao ha o que calcular).
    //   TODO(Membro2): se preferir devolver 0 nesse caso, mudar aqui.
    bool lerTempoCicloMedio(const Email& solicitante, const Identificador& quadro, double* minutos) override {
        return lerMedia(solicitante, quadro, false, minutos);
    }

    // ESCOLHA(Membro3): lead time = termino - entrada (em minutos). So existe
    //   para cartao em FEITO; senao devolve false.
    bool lerLeadTime(const Email& solicitante, const Identificador& cartao, int* minutos) override {
        return lerTempo(solicitante, cartao, true, minutos);
    }

    bool lerLeadTimeMedio(const Email& solicitante, const Identificador& quadro, double* minutos) override {
        return lerMedia(solicitante, quadro, true, minutos);
    }

    // ESCOLHA(Membro3): WIP = quantidade de cartoes do quadro em FAZENDO.
    bool lerWip(const Email& solicitante, const Identificador& quadro, int* wip) override {
        if (!solicitanteExiste(solicitante)) return false;
        Quadro q;
        if (!quadros->pesquisar(quadro, &q)) return false;
        *wip = contarFazendo(quadro);
        return true;
    }
};

#endif // CONTROLADORAS_CARTAO_H_INCLUDED
