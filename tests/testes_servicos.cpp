#include "testes_servicos.h"

#include <iostream>
#include <vector>
#include "containers.h"
#include "controladoras_pessoa.h"
#include "controladoras_projeto.h"
#include "controladoras_quadro.h"
#include "controladoras_cartao.h"

// TODO(Membro2): os testes usam as entidades e interfaces PROVISORIAS (via os
// includes acima). Se os nomes mudarem, ajustar aqui tambem.
//
// ESCOLHA(Membro3): cada caso de teste monta um "Cenario" novo (containers e
//   controladoras vazios) e prepara so o que precisa usando os proprios
//   servicos. Assim os casos nao dependem uns dos outros.

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
    CSAutenticacao autenticacao;
    CSPessoa pessoa;
    CSProjeto projeto;
    CSQuadro quadro;
    CSCartao cartao;

    Cenario()
        : autenticacao(&pessoas),
          pessoa(&pessoas, &projetos, &cartoes),
          projeto(&pessoas, &projetos, &quadros),
          quadro(&pessoas, &projetos, &quadros, &cartoes),
          cartao(&pessoas, &projetos, &quadros, &cartoes) {}
};

Email em(const string& valor) { Email e; e.setValor(valor); return e; }
Identificador idn(const string& valor) { Identificador i; i.setValor(valor); return i; }
Estado est(const string& valor) { Estado e; e.setValor(valor); return e; }
Timestamp ts(const string& valor) { Timestamp t; t.setValor(valor); return t; }

Pessoa pessoaDe(const string& email, const string& papel) {
    Nome nome; nome.setValor("Ana");
    Senha senha; senha.setValor(SENHA);
    Papel p; p.setValor(papel);
    Pessoa pessoa;
    pessoa.setEmail(em(email)); pessoa.setNome(nome); pessoa.setSenha(senha); pessoa.setPapel(p);
    return pessoa;
}

Projeto projetoDe(const string& id, const string& nome) {
    Nome n; n.setValor(nome);
    Projeto projeto;
    projeto.setIdentificador(idn(id)); projeto.setNome(n);
    return projeto;
}

Quadro quadroDe(const string& id, int limite) {
    Nome n; n.setValor("Quadro");
    Limite l; l.setValor(limite);
    Quadro quadro;
    quadro.setIdentificador(idn(id)); quadro.setNome(n); quadro.setLimite(l);
    return quadro;
}

Cartao cartaoDe(const string& id) {
    Nome n; n.setValor("Cartao");
    Cartao cartao;
    cartao.setIdentificador(idn(id)); cartao.setNome(n); cartao.setEntrada(ts(ENTRADA));
    return cartao;
}

// Preparos usados pelos casos de teste.
void criarPessoas(Cenario& c) {
    c.pessoa.criar(pessoaDe(GESTOR, "GESTOR"));
    c.pessoa.criar(pessoaDe(DEV, "DESENVOLVEDOR"));
    c.pessoa.criar(pessoaDe(OUTRO_GESTOR, "GESTOR"));
}

void criarProjeto(Cenario& c) {
    criarPessoas(c);
    c.projeto.criar(em(GESTOR), projetoDe(PROJETO, "Projeto"));
}

void criarQuadro(Cenario& c) {
    criarProjeto(c);
    c.quadro.criar(em(GESTOR), idn(PROJETO), quadroDe(QUADRO, 2));
}

void criarCartao(Cenario& c, const string& id) {
    c.cartao.criar(em(GESTOR), idn(QUADRO), em(DEV), cartaoDe(id));
}

// Cartao do quadro: A FAZER -> FAZENDO (10:00).
void iniciarCartao(Cenario& c, const string& id) {
    c.cartao.mover(em(DEV), idn(id), est("FAZENDO"), ts("01-JAN-2026-10:00"));
}

// Cartao do quadro: FAZENDO -> FEITO (18:30). Ciclo = 510 min; lead time = 630 min.
void concluirCartao(Cenario& c, const string& id) {
    c.cartao.mover(em(DEV), idn(id), est("FEITO"), ts("01-JAN-2026-18:30"));
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
    Senha certa; certa.setValor(SENHA);
    Senha errada; errada.setValor("xyz99");
    return c.autenticacao.autenticar(em(GESTOR), certa) &&
           !c.autenticacao.autenticar(em(GESTOR), errada);
}

// ---------------------------------------------------------------------------
// Pessoa (servicos 1 a 5)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarPessoa() {
    Cenario c;
    Pessoa ana = pessoaDe(GESTOR, "GESTOR");
    return c.pessoa.criar(ana) && !c.pessoa.criar(ana);
}

bool TesteServicos::testarLerPessoa() {
    Cenario c; criarPessoas(c);
    Pessoa lida;
    return c.pessoa.ler(em(GESTOR), em(DEV), &lida) &&
           lida.getEmail().getValor() == DEV;
}

bool TesteServicos::testarAtualizarPessoa() {
    Cenario c; criarPessoas(c);
    Pessoa nova = pessoaDe(GESTOR, "GESTOR");
    Nome nome; nome.setValor("Ana Maria");
    nova.setNome(nome);
    if (c.pessoa.atualizar(em(DEV), nova)) return false;   // outra pessoa nao atualiza
    Pessoa lida;
    return c.pessoa.atualizar(em(GESTOR), nova) &&
           c.pessoa.ler(em(GESTOR), em(GESTOR), &lida) &&
           lida.getNome().getValor() == "Ana Maria";
}

bool TesteServicos::testarExcluirPessoa() {
    Cenario c; criarProjeto(c);
    if (c.pessoa.excluir(em(GESTOR), em(GESTOR))) return false;   // gestor de projeto nao exclui
    return c.pessoa.excluir(em(DEV), em(DEV));
}

bool TesteServicos::testarListarPessoa() {
    Cenario c; criarPessoas(c);
    std::vector<Email> emails;
    return c.pessoa.listar(em(GESTOR), &emails) && emails.size() == 3;
}

// ---------------------------------------------------------------------------
// Projeto (servicos 6 a 10)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarProjeto() {
    Cenario c; criarPessoas(c);
    Projeto projeto = projetoDe(PROJETO, "Projeto");
    if (c.projeto.criar(em(DEV), projeto)) return false;   // desenvolvedor nao cria
    return c.projeto.criar(em(GESTOR), projeto);
}

bool TesteServicos::testarLerProjeto() {
    Cenario c; criarProjeto(c);
    Projeto lido;
    return c.projeto.ler(em(DEV), idn(PROJETO), &lido) &&
           lido.getNome().getValor() == "Projeto";
}

bool TesteServicos::testarAtualizarProjeto() {
    Cenario c; criarProjeto(c);
    Projeto novo = projetoDe(PROJETO, "Novo");
    if (c.projeto.atualizar(em(OUTRO_GESTOR), novo)) return false;   // so o gestor do projeto
    Projeto lido;
    return c.projeto.atualizar(em(GESTOR), novo) &&
           c.projeto.ler(em(GESTOR), idn(PROJETO), &lido) &&
           lido.getNome().getValor() == "Novo";
}

bool TesteServicos::testarExcluirProjeto() {
    Cenario c; criarProjeto(c);
    return c.projeto.excluir(em(GESTOR), idn(PROJETO)) &&
           !c.projeto.excluir(em(GESTOR), idn(PROJETO));
}

bool TesteServicos::testarListarProjeto() {
    Cenario c; criarProjeto(c);
    std::vector<Identificador> ids;
    return c.projeto.listar(em(DEV), &ids) && ids.size() == 1;
}

// ---------------------------------------------------------------------------
// Quadro (servicos 11 a 15)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarQuadro() {
    Cenario c; criarProjeto(c);
    Quadro quadro = quadroDe(QUADRO, 2);
    if (c.quadro.criar(em(DEV), idn(PROJETO), quadro)) return false;   // desenvolvedor nao cria
    return c.quadro.criar(em(GESTOR), idn(PROJETO), quadro);
}

bool TesteServicos::testarLerQuadro() {
    Cenario c; criarQuadro(c);
    Quadro lido;
    return c.quadro.ler(em(DEV), idn(QUADRO), &lido) &&
           lido.getLimite().getValor() == 2;
}

bool TesteServicos::testarAtualizarQuadro() {
    Cenario c; criarQuadro(c);
    Quadro novo = quadroDe(QUADRO, 5);
    if (c.quadro.atualizar(em(OUTRO_GESTOR), novo)) return false;   // so o gestor do projeto
    Quadro lido;
    return c.quadro.atualizar(em(GESTOR), novo) &&
           c.quadro.ler(em(GESTOR), idn(QUADRO), &lido) &&
           lido.getLimite().getValor() == 5;
}

bool TesteServicos::testarExcluirQuadro() {
    Cenario c; criarQuadro(c);
    return c.quadro.excluir(em(GESTOR), idn(QUADRO)) &&
           !c.quadro.excluir(em(GESTOR), idn(QUADRO));
}

bool TesteServicos::testarListarQuadro() {
    Cenario c; criarQuadro(c);
    std::vector<Identificador> ids;
    return c.quadro.listar(em(DEV), &ids) && ids.size() == 1;
}

// ---------------------------------------------------------------------------
// Cartao de atividade (servicos 16 a 26)
// ---------------------------------------------------------------------------
bool TesteServicos::testarCriarCartao() {
    Cenario c; criarQuadro(c);
    Cartao cartao = cartaoDe(CARTAO1);
    if (c.cartao.criar(em(DEV), idn(QUADRO), em(DEV), cartao)) return false;   // desenvolvedor nao cria
    Cartao lido;
    return c.cartao.criar(em(GESTOR), idn(QUADRO), em(DEV), cartao) &&
           c.cartao.ler(em(GESTOR), idn(CARTAO1), &lido) &&
           lido.getEstado().getValor() == "A FAZER";
}

bool TesteServicos::testarLerCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    Cartao lido;
    return c.cartao.ler(em(DEV), idn(CARTAO1), &lido) &&
           lido.getIdentificador().getValor() == CARTAO1;
}

bool TesteServicos::testarAtualizarCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    Cartao alterado = cartaoDe(CARTAO1);
    Nome nome; nome.setValor("Novo");
    alterado.setNome(nome);
    if (c.cartao.atualizar(em(DEV), alterado)) return false;   // desenvolvedor nao atualiza
    Cartao lido;
    return c.cartao.atualizar(em(GESTOR), alterado) &&
           c.cartao.ler(em(GESTOR), idn(CARTAO1), &lido) &&
           lido.getNome().getValor() == "Novo";
}

bool TesteServicos::testarExcluirCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    if (c.cartao.excluir(em(DEV), idn(CARTAO1))) return false;   // desenvolvedor nao exclui
    return c.cartao.excluir(em(GESTOR), idn(CARTAO1)) &&
           !c.cartao.excluir(em(GESTOR), idn(CARTAO1));
}

bool TesteServicos::testarListarCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1); criarCartao(c, CARTAO2);
    std::vector<Identificador> ids;
    return c.cartao.listar(em(DEV), &ids) && ids.size() == 2;
}

bool TesteServicos::testarMoverCartao() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    // A FAZER -> FEITO nao e permitido
    if (c.cartao.mover(em(DEV), idn(CARTAO1), est("FEITO"), ts("01-JAN-2026-09:00"))) return false;
    Cartao lido;
    return c.cartao.mover(em(DEV), idn(CARTAO1), est("FAZENDO"), ts("01-JAN-2026-10:00")) &&
           c.cartao.mover(em(DEV), idn(CARTAO1), est("FEITO"), ts("01-JAN-2026-18:30")) &&
           c.cartao.ler(em(DEV), idn(CARTAO1), &lido) &&
           lido.getEstado().getValor() == "FEITO";
}

bool TesteServicos::testarLerTempoCiclo() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    int minutos = 0;
    return c.cartao.lerTempoCiclo(em(DEV), idn(CARTAO1), &minutos) && minutos == 510;
}

bool TesteServicos::testarLerTempoCicloMedio() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    double minutos = 0;
    return c.cartao.lerTempoCicloMedio(em(DEV), idn(QUADRO), &minutos) && minutos == 510.0;
}

bool TesteServicos::testarLerLeadTime() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    int minutos = 0;
    return c.cartao.lerLeadTime(em(DEV), idn(CARTAO1), &minutos) && minutos == 630;
}

bool TesteServicos::testarLerLeadTimeMedio() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    iniciarCartao(c, CARTAO1); concluirCartao(c, CARTAO1);
    double minutos = 0;
    return c.cartao.lerLeadTimeMedio(em(DEV), idn(QUADRO), &minutos) && minutos == 630.0;
}

bool TesteServicos::testarLerWip() {
    Cenario c; criarQuadro(c); criarCartao(c, CARTAO1);
    int wip = -1;
    if (!c.cartao.lerWip(em(DEV), idn(QUADRO), &wip) || wip != 0) return false;
    iniciarCartao(c, CARTAO1);
    return c.cartao.lerWip(em(DEV), idn(QUADRO), &wip) && wip == 1;
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
