#include "testes_servicos.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include "containers.h"
#include "sessao.h"
#include "controladoras_pessoa.h"
#include "controladoras_projeto.h"
#include "controladoras_quadro.h"
#include "controladoras_cartao.h"

// Decisao de projeto: cada caso de teste monta um "Cenario" novo (containers,
//   sessao e controladoras vazios) e prepara so o que precisa usando os
//   proprios servicos. Assim os casos nao dependem uns dos outros.
// Decisao de projeto: como os servicos avisam falhas por excecao, os testes de
//   falha usam lancaExcecao(): devolve true se o codigo lancou std::logic_error.

namespace {

const string GESTOR = "ana@teste.com";
const string OUTRO_GESTOR = "caio@teste.com";
const string DEV = "bia@teste.com";
const string SENHA = "abc12";
const string PROJETO = "PRJ001";
const string QUADRO = "QDR001";
const string CARTAO1 = "CRT001";
const string CARTAO2 = "CRT002";
const string ENTRADA = "01-JAN-2026-08:00";

// Tudo o que a camada de servico precisa, ja ligado.
struct Cenario {
    ContainerPessoas pessoas;
    ContainerProjetos projetos;
    ContainerQuadros quadros;
    ContainerCartoes cartoes;
    Sessao sessao;
    CSAutenticacao autenticacao;
    CSPessoa pessoa;
    CSProjeto projeto;
    CSQuadro quadro;
    CSCartao cartao;

    Cenario()
        : autenticacao(&pessoas, &sessao),
          pessoa(&pessoas, &projetos, &cartoes, &sessao),
          projeto(&pessoas, &projetos, &quadros, &sessao),
          quadro(&pessoas, &projetos, &quadros, &cartoes, &sessao),
          cartao(&pessoas, &projetos, &quadros, &cartoes, &sessao) {}
};

// Devolve true se o codigo lanca std::logic_error (falha de servico ou de dominio).
template <typename F>
bool lancaExcecao(F codigo) {
    try {
        codigo();
    } catch (const std::logic_error&) {
        return true;
    }
    return false;
}

Email em(const string& valor) { Email e; e.setValor(valor); return e; }
Identificador idn(const string& valor) { Identificador i; i.setValor(valor); return i; }
Estado est(const string& valor) { Estado e; e.setValor(valor); return e; }
Timestamp ts(const string& valor) { Timestamp t; t.setValor(valor); return t; }
Nome nm(const string& valor) { Nome n; n.setValor(valor); return n; }
Texto tx(const string& valor) { Texto t; t.setValor(valor); return t; }
Limite lm(int valor) { Limite l; l.setValor(valor); return l; }
Senha sn(const string& valor) { Senha s; s.setValor(valor); return s; }
Papel pp(const string& valor) { Papel p; p.setValor(valor); return p; }

Pessoa pessoaDe(const string& email, const string& papel) {
    return Pessoa(em(email), nm("Ana"), sn(SENHA), pp(papel));
}

// O gestorEmail aqui e so o valor inicial; o servico o troca pelo usuario autenticado.
Projeto projetoDe(const string& id, const string& nome) {
    return Projeto(idn(id), nm(nome), tx("Descricao."), ts("01-JAN-2026-08:00"),
                   ts("31-DEZ-2026-18:00"), em(GESTOR));
}

Quadro quadroDe(const string& id, int limite, const string& projeto) {
    return Quadro(idn(id), nm("Quadro"), lm(limite), idn(projeto));
}

CartaoAtividade cartaoDe(const string& id) {
    Prioridade prioridade; prioridade.setValor("MEDIA");
    Tamanho tamanho; tamanho.setValor("PEQUENO");
    return CartaoAtividade(idn(id), nm("Cartao"), tx("Descricao."), prioridade,
                           est("A FAZER"), tamanho, ts(ENTRADA), idn(QUADRO), em(DEV));
}

// Autentica alguem; devolve se deu certo.
bool entrar(Cenario& c, const string& email) {
    return c.autenticacao.autenticar(em(email), sn(SENHA));
}

// Preparos usados pelos casos de teste. Deixam o GESTOR autenticado.
void criarPessoas(Cenario& c) {
    c.pessoa.criar(pessoaDe(GESTOR, "GESTOR"));
    c.pessoa.criar(pessoaDe(DEV, "DESENVOLVEDOR"));
    c.pessoa.criar(pessoaDe(OUTRO_GESTOR, "GESTOR"));
}

void criarProjeto(Cenario& c) {
    criarPessoas(c);
    entrar(c, GESTOR);
    c.projeto.criar(projetoDe(PROJETO, "Projeto"));
}

void criarQuadro(Cenario& c) {
    criarProjeto(c);
    c.quadro.criar(quadroDe(QUADRO, 2, PROJETO));
}

void criarCartao(Cenario& c, const string& id) {
    entrar(c, GESTOR);
    c.cartao.criar(cartaoDe(id));
}

// Cartao do quadro: A FAZER -> FAZENDO (10:00).
void iniciarCartao(Cenario& c, const string& id) {
    entrar(c, DEV);
    c.cartao.mover(idn(id), est("FAZENDO"), ts("01-JAN-2026-10:00"));
}

// Cartao do quadro: FAZENDO -> FEITO (18:30). Ciclo = 510 min; lead time = 630 min.
void concluirCartao(Cenario& c, const string& id) {
    entrar(c, DEV);
    c.cartao.mover(idn(id), est("FEITO"), ts("01-JAN-2026-18:30"));
}

} // namespace

void TesteServicos::registrar(const std::string& nome, bool passou) {
    std::cout << (passou ? "[PASSOU] " : "[FALHOU] ") << nome << std::endl;
    if (!passou) ++falhas;
}

// ---------------------------------------------------------------------------
// Autenticacao
// ---------------------------------------------------------------------------
bool TesteServicos::testarAutenticar() {
    Cenario c; criarPessoas(c);
    if (c.autenticacao.autenticar(em(GESTOR), sn("xyz99"))) return false;   // senha errada
    if (c.sessao.estaAtiva()) return false;
    return c.autenticacao.autenticar(em(GESTOR), sn(SENHA)) && c.sessao.estaAtiva();
}

// ---------------------------------------------------------------------------
// Pessoa (servicos 1 a 5)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarPessoa() {
    Cenario c;
    Pessoa ana = pessoaDe(GESTOR, "GESTOR");
    c.pessoa.criar(ana);   // nao exige autenticacao
    return lancaExcecao([&]() { c.pessoa.criar(ana); });   // email repetido
}

bool TesteServicos::testarLerPessoa() {
    Cenario c; criarPessoas(c);
    if (!lancaExcecao([&]() { c.pessoa.ler(em(DEV)); })) return false;   // sem login: deve falhar
    entrar(c, GESTOR);
    return c.pessoa.ler(em(DEV)).getEmail().getValor() == DEV;
}

bool TesteServicos::testarAtualizarPessoa() {
    Cenario c; criarPessoas(c);
    Pessoa nova = pessoaDe(GESTOR, "GESTOR");
    nova.setNome(nm("Ana Maria"));
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.pessoa.atualizar(nova); })) return false;   // outra pessoa nao atualiza
    entrar(c, GESTOR);
    c.pessoa.atualizar(nova);
    return c.pessoa.ler(em(GESTOR)).getNome().getValor() == "Ana Maria";
}

bool TesteServicos::testarExcluirPessoa() {
    Cenario c; criarProjeto(c);   // GESTOR autenticado, gestor de um projeto
    if (!lancaExcecao([&]() { c.pessoa.excluir(em(GESTOR)); })) return false;   // tem vinculo
    entrar(c, DEV);
    c.pessoa.excluir(em(DEV));
    return !c.sessao.estaAtiva();   // a sessao termina junto com a conta
}

bool TesteServicos::testarListarPessoa() {
    Cenario c; criarPessoas(c);
    entrar(c, GESTOR);
    return c.pessoa.listar().size() == 3;
}

// ---------------------------------------------------------------------------
// Projeto (servicos 6 a 10)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarProjeto() {
    Cenario c; criarPessoas(c);
    // O gestorEmail recebido (OUTRO_GESTOR) e trocado pelo usuario autenticado.
    Projeto projeto(idn(PROJETO), nm("Projeto"), tx("Descricao."), ts("01-JAN-2026-08:00"),
                    ts("31-DEZ-2026-18:00"), em(OUTRO_GESTOR));
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.projeto.criar(projeto); })) return false;   // desenvolvedor nao cria
    entrar(c, GESTOR);
    Projeto invertido(idn("PRJ002"), nm("Projeto"), tx("Descricao."), ts("31-DEZ-2026-18:00"),
                      ts("01-JAN-2026-08:00"), em(GESTOR));
    if (!lancaExcecao([&]() { c.projeto.criar(invertido); })) return false;   // termino antes do inicio
    c.projeto.criar(projeto);
    return c.projeto.ler(idn(PROJETO)).getGestorEmail().getValor() == GESTOR;
}

bool TesteServicos::testarLerProjeto() {
    Cenario c; criarProjeto(c);
    entrar(c, DEV);
    return c.projeto.ler(idn(PROJETO)).getNome().getValor() == "Projeto";
}

bool TesteServicos::testarAtualizarProjeto() {
    Cenario c; criarProjeto(c);
    Projeto novo = projetoDe(PROJETO, "Novo");
    entrar(c, OUTRO_GESTOR);
    if (!lancaExcecao([&]() { c.projeto.atualizar(novo); })) return false;   // so o gestor do projeto
    entrar(c, GESTOR);
    c.projeto.atualizar(novo);
    return c.projeto.ler(idn(PROJETO)).getNome().getValor() == "Novo";
}

bool TesteServicos::testarExcluirProjeto() {
    Cenario c; criarProjeto(c);
    entrar(c, OUTRO_GESTOR);
    if (!lancaExcecao([&]() { c.projeto.excluir(idn(PROJETO)); })) return false;   // so o gestor do projeto
    entrar(c, GESTOR);
    c.projeto.excluir(idn(PROJETO));
    return lancaExcecao([&]() { c.projeto.ler(idn(PROJETO)); });
}

bool TesteServicos::testarListarProjeto() {
    Cenario c; criarProjeto(c);
    entrar(c, DEV);
    return c.projeto.listar().size() == 1;
}

// ---------------------------------------------------------------------------
// Quadro (servicos 11 a 15)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarQuadro() {
    Cenario c; criarProjeto(c);
    Quadro quadro = quadroDe(QUADRO, 2, PROJETO);
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.quadro.criar(quadro); })) return false;   // desenvolvedor nao cria
    entrar(c, OUTRO_GESTOR);
    if (!lancaExcecao([&]() { c.quadro.criar(quadro); })) return false;   // nao e o gestor do projeto
    entrar(c, GESTOR);
    c.quadro.criar(quadro);
    return c.quadro.ler(idn(QUADRO)).getProjetoId().getValor() == PROJETO;
}

bool TesteServicos::testarLerQuadro() {
    Cenario c; criarQuadro(c);
    entrar(c, DEV);
    return c.quadro.ler(idn(QUADRO)).getLimite().getValor() == 2;
}

bool TesteServicos::testarAtualizarQuadro() {
    Cenario c; criarQuadro(c);
    Quadro novo = quadroDe(QUADRO, 5, PROJETO);
    entrar(c, OUTRO_GESTOR);
    if (!lancaExcecao([&]() { c.quadro.atualizar(novo); })) return false;   // so o gestor do projeto
    entrar(c, GESTOR);
    c.quadro.atualizar(novo);
    return c.quadro.ler(idn(QUADRO)).getLimite().getValor() == 5;
}

bool TesteServicos::testarExcluirQuadro() {
    Cenario c; criarQuadro(c);
    entrar(c, GESTOR);
    c.quadro.excluir(idn(QUADRO));
    return lancaExcecao([&]() { c.quadro.ler(idn(QUADRO)); });
}

bool TesteServicos::testarListarQuadro() {
    Cenario c; criarQuadro(c);
    entrar(c, DEV);
    return c.quadro.listar().size() == 1;
}

// ---------------------------------------------------------------------------
// Cartao de atividade (servicos 16 a 26)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarCartao() {
    Cenario c; criarQuadro(c);
    CartaoAtividade cartao = cartaoDe(CARTAO1);
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.cartao.criar(cartao); })) return false;   // desenvolvedor nao cria
    entrar(c, GESTOR);
    c.cartao.criar(cartao);
    return c.cartao.ler(idn(CARTAO1)).getEstado().getValor() == "A FAZER";
}

bool TesteServicos::testarLerCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    entrar(c, DEV);
    return c.cartao.ler(idn(CARTAO1)).getIdentificador().getValor() == CARTAO1;
}

bool TesteServicos::testarAtualizarCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    CartaoAtividade alterado = cartaoDe(CARTAO1);
    alterado.setNome(nm("Novo"));
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.cartao.atualizar(alterado); })) return false;   // desenvolvedor nao atualiza
    entrar(c, GESTOR);
    c.cartao.atualizar(alterado);
    return c.cartao.ler(idn(CARTAO1)).getNome().getValor() == "Novo";
}

bool TesteServicos::testarExcluirCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    entrar(c, DEV);
    if (!lancaExcecao([&]() { c.cartao.excluir(idn(CARTAO1)); })) return false;   // desenvolvedor nao exclui
    entrar(c, GESTOR);
    c.cartao.excluir(idn(CARTAO1));
    return lancaExcecao([&]() { c.cartao.ler(idn(CARTAO1)); });
}

bool TesteServicos::testarListarCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1); criarCartao(c, CARTAO2);
    entrar(c, DEV);
    return c.cartao.listar().size() == 2;
}

bool TesteServicos::testarMoverCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    entrar(c, DEV);
    // A FAZER -> FEITO nao e permitido
    if (!lancaExcecao([&]() { c.cartao.mover(idn(CARTAO1), est("FEITO"), ts("01-JAN-2026-09:00")); })) return false;
    c.cartao.mover(idn(CARTAO1), est("FAZENDO"), ts("01-JAN-2026-10:00"));
    c.cartao.mover(idn(CARTAO1), est("FEITO"), ts("01-JAN-2026-18:30"));
    return c.cartao.ler(idn(CARTAO1)).getEstado().getValor() == "FEITO";
}

bool TesteServicos::testarLerTempoCiclo() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    return c.cartao.lerTempoCiclo(idn(CARTAO1)) == 510;
}

bool TesteServicos::testarLerTempoCicloMedio() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    return c.cartao.lerTempoCicloMedio(idn(QUADRO)) == 510.0;
}

bool TesteServicos::testarLerLeadTime() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    return c.cartao.lerLeadTime(idn(CARTAO1)) == 630;
}

bool TesteServicos::testarLerLeadTimeMedio() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    return c.cartao.lerLeadTimeMedio(idn(QUADRO)) == 630.0;
}

bool TesteServicos::testarLerWip() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    entrar(c, DEV);
    if (c.cartao.lerWip(idn(QUADRO)) != 0u) return false;
    iniciarCartao(c, CARTAO1);
    return c.cartao.lerWip(idn(QUADRO)) == 1u;
}

// ---------------------------------------------------------------------------
int TesteServicos::executar() {
    falhas = 0;
    std::cout << "=== Testes da camada de servico ===" << std::endl;

    registrar("AUTENTICAR", testarAutenticar());

    registrar("01 CRIAR PESSOA", testarCriarPessoa());
    registrar("02 LER PESSOA", testarLerPessoa());
    registrar("03 ATUALIZAR PESSOA", testarAtualizarPessoa());
    registrar("04 EXCLUIR PESSOA", testarExcluirPessoa());
    registrar("05 LISTAR PESSOA", testarListarPessoa());

    registrar("06 CRIAR PROJETO", testarCriarProjeto());
    registrar("07 LER PROJETO", testarLerProjeto());
    registrar("08 ATUALIZAR PROJETO", testarAtualizarProjeto());
    registrar("09 EXCLUIR PROJETO", testarExcluirProjeto());
    registrar("10 LISTAR PROJETO", testarListarProjeto());

    registrar("11 CRIAR QUADRO", testarCriarQuadro());
    registrar("12 LER QUADRO", testarLerQuadro());
    registrar("13 ATUALIZAR QUADRO", testarAtualizarQuadro());
    registrar("14 EXCLUIR QUADRO", testarExcluirQuadro());
    registrar("15 LISTAR QUADRO", testarListarQuadro());

    registrar("16 CRIAR CARTAO DE ATIVIDADE", testarCriarCartao());
    registrar("17 LER CARTAO DE ATIVIDADE", testarLerCartao());
    registrar("18 ATUALIZAR CARTAO DE ATIVIDADE", testarAtualizarCartao());
    registrar("19 EXCLUIR CARTAO DE ATIVIDADE", testarExcluirCartao());
    registrar("20 LISTAR CARTAO DE ATIVIDADE", testarListarCartao());
    registrar("21 MOVER CARTAO DE ATIVIDADE", testarMoverCartao());
    registrar("22 LER TEMPO DE CICLO", testarLerTempoCiclo());
    registrar("23 LER TEMPO DE CICLO MEDIO", testarLerTempoCicloMedio());
    registrar("24 LER LEAD TIME", testarLerLeadTime());
    registrar("25 LER LEAD TIME MEDIO", testarLerLeadTimeMedio());
    registrar("26 LER WIP", testarLerWip());

    std::cout << "Total: 27 casos de teste, " << falhas << " falha(s)." << std::endl;
    return falhas;
}
