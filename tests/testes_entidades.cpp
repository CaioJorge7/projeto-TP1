#include "entidades.h"
#include "interfaces.h"
#include "testes_entidades.h"
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <iostream>

// Compilacao e execucao requerem a implementacao REAL de src/dominios.cpp.
template <typename T>
T criarDominio(const std::string& texto) {
    T dominio;
    dominio.setValor(texto);
    return dominio;
}

void TestesEntidades::executar() {
    static_assert(std::is_abstract<IServicoPessoa>::value, "Interface deve ser abstrata");
    static_assert(std::is_abstract<IServicoProjeto>::value, "Interface deve ser abstrata");
    static_assert(std::is_abstract<IServicoQuadro>::value, "Interface deve ser abstrata");
    static_assert(std::is_abstract<IServicoCartaoAtividade>::value, "Interface deve ser abstrata");
    static_assert(std::is_abstract<IServicoAutenticacao>::value, "Interface deve ser abstrata");

    Email gestor = criarDominio<Email>("gestor@tp1.com");
    Email dev = criarDominio<Email>("dev@tp1.com");
    Nome nomePessoa = criarDominio<Nome>("Ana Silva");
    Senha senha = criarDominio<Senha>("abc12");
    Papel papelGestor = criarDominio<Papel>("GESTOR");
    Papel papelDev = criarDominio<Papel>("DESENVOLVEDOR");

    Pessoa pessoa(gestor, nomePessoa, senha, papelGestor);
    assert(pessoa.getEmail().getValor() == "gestor@tp1.com");
    assert(pessoa.getPapel().getValor() == "GESTOR");
    pessoa.setPapel(papelDev);
    assert(pessoa.getPapel().getValor() == "DESENVOLVEDOR");
    bool chaveBloqueada = false;
    try { pessoa.setEmail(dev); }
    catch (const std::logic_error&) { chaveBloqueada = true; }
    assert(chaveBloqueada);
    pessoa.setEmail(gestor); // Mesmo valor e permitido.

    Identificador idProjeto = criarDominio<Identificador>("PRJ001");
    Identificador idQuadro = criarDominio<Identificador>("QDR001");
    Identificador idCartao = criarDominio<Identificador>("CAR001");
    Nome nomeProjeto = criarDominio<Nome>("Projeto Um");
    Nome nomeQuadro = criarDominio<Nome>("Quadro Um");
    Nome nomeCartao = criarDominio<Nome>("Atividade Um");
    Texto descricao = criarDominio<Texto>("Descricao valida.");
    Timestamp entrada = criarDominio<Timestamp>("01-JAN-2026-09:00");
    Timestamp inicio = criarDominio<Timestamp>("02-JAN-2026-10:00");
    Timestamp termino = criarDominio<Timestamp>("03-JAN-2026-11:00");

    Projeto projeto(idProjeto, nomeProjeto, descricao, entrada, termino, gestor);
    assert(projeto.getGestorEmail().getValor() == "gestor@tp1.com");
    assert(projeto.getIdentificador().getValor() == "PRJ001");
    chaveBloqueada = false;
    try { projeto.setIdentificador(idQuadro); }
    catch (const std::logic_error&) { chaveBloqueada = true; }
    assert(chaveBloqueada);

    Limite limite;
    limite.setValor(3);
    Quadro quadro(idQuadro, nomeQuadro, limite, idProjeto);
    assert(quadro.getLimite().getValor() == 3);
    assert(quadro.getProjetoId().getValor() == "PRJ001");

    Prioridade prioridade = criarDominio<Prioridade>("ALTA");
    Estado estado = criarDominio<Estado>("A FAZER");
    Estado fazendo = criarDominio<Estado>("FAZENDO");
    Tamanho tamanho = criarDominio<Tamanho>("GRANDE");
    CartaoAtividade cartao(idCartao, nomeCartao, descricao, prioridade,
                           estado, tamanho, entrada, idQuadro, dev);
    assert(!cartao.temInicio());
    assert(!cartao.temTermino());
    assert(cartao.getDesenvolvedorEmail().getValor() == "dev@tp1.com");
    assert(cartao.getEstado().getValor() == "A FAZER");
    cartao.setEstado(fazendo);
    cartao.setInicio(inicio);
    cartao.setTermino(termino);
    assert(cartao.temInicio() && cartao.temTermino());
    assert(cartao.getInicio().getValor() == "02-JAN-2026-10:00");
    assert(cartao.getTermino().getValor() == "03-JAN-2026-11:00");
    chaveBloqueada = false;
    try { cartao.setIdentificador(idProjeto); }
    catch (const std::logic_error&) { chaveBloqueada = true; }
    assert(chaveBloqueada);

    // Validacao de formato continua sob responsabilidade dos dominios.
    bool formatoBloqueado = false;
    try { criarDominio<Identificador>("INVALIDO"); }
    catch (const std::invalid_argument&) { formatoBloqueado = true; }
    assert(formatoBloqueado);

    std::cout << "[OK] Testes de entidades, chaves e contratos abstratos.\n";
}
