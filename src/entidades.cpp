/**
 * @file entidades.cpp
 * @brief Implementacao dos construtores e metodos das entidades do sistema Kanban.
 */

#include "entidades.h"
#include <stdexcept>

// As chaves primarias nao podem ser alteradas apos a criacao da entidade.
// A atribuicao de um mesmo valor e permitida, para compatibilidade com set/get.
namespace {
    /**
     * @brief Garante que uma chave primaria nao seja alterada apos a construcao.
     * @param atual Valor atual da chave.
     * @param nova Novo valor pretendido.
     * @throw std::logic_error Caso o novo valor seja diferente do atual.
     */
    void exigirChaveInalterada(const std::string& atual, const std::string& nova) {
        if (atual != nova) {
            throw std::logic_error("Nao e permitido alterar uma chave primaria.");
        }
    }
}

// ============================ Pessoa ============================
Pessoa::Pessoa(const Email& email, const Nome& nome, const Senha& senha,
               const Papel& papel)
    : email(email), nome(nome), senha(senha), papel(papel) {}

const Email& Pessoa::getEmail() const { return email; }
void Pessoa::setEmail(const Email& novo) {
    exigirChaveInalterada(email.getValor(), novo.getValor());
}
const Nome& Pessoa::getNome() const { return nome; }
void Pessoa::setNome(const Nome& novo) { nome = novo; }
const Senha& Pessoa::getSenha() const { return senha; }
void Pessoa::setSenha(const Senha& nova) { senha = nova; }
const Papel& Pessoa::getPapel() const { return papel; }
void Pessoa::setPapel(const Papel& novo) { papel = novo; }

// ============================ Projeto ===========================
Projeto::Projeto(const Identificador& identificador, const Nome& nome,
                 const Texto& descricao, const Timestamp& inicio,
                 const Timestamp& termino, const Email& gestorEmail)
    : identificador(identificador), nome(nome), descricao(descricao),
      inicio(inicio), termino(termino), gestorEmail(gestorEmail) {}

const Identificador& Projeto::getIdentificador() const { return identificador; }
void Projeto::setIdentificador(const Identificador& novo) {
    exigirChaveInalterada(identificador.getValor(), novo.getValor());
}
const Nome& Projeto::getNome() const { return nome; }
void Projeto::setNome(const Nome& novo) { nome = novo; }
const Texto& Projeto::getDescricao() const { return descricao; }
void Projeto::setDescricao(const Texto& nova) { descricao = nova; }
const Timestamp& Projeto::getInicio() const { return inicio; }
void Projeto::setInicio(const Timestamp& novo) { inicio = novo; }
const Timestamp& Projeto::getTermino() const { return termino; }
void Projeto::setTermino(const Timestamp& novo) { termino = novo; }
const Email& Projeto::getGestorEmail() const { return gestorEmail; }
void Projeto::setGestorEmail(const Email& novo) { gestorEmail = novo; }

// ============================ Quadro ============================
Quadro::Quadro(const Identificador& identificador, const Nome& nome,
               const Limite& limite, const Identificador& projetoId)
    : identificador(identificador), nome(nome), limite(limite),
      projetoId(projetoId) {}

const Identificador& Quadro::getIdentificador() const { return identificador; }
void Quadro::setIdentificador(const Identificador& novo) {
    exigirChaveInalterada(identificador.getValor(), novo.getValor());
}
const Nome& Quadro::getNome() const { return nome; }
void Quadro::setNome(const Nome& novo) { nome = novo; }
const Limite& Quadro::getLimite() const { return limite; }
void Quadro::setLimite(const Limite& novo) { limite = novo; }
const Identificador& Quadro::getProjetoId() const { return projetoId; }
void Quadro::setProjetoId(const Identificador& novo) { projetoId = novo; }

// ======================== CartaoAtividade ========================
CartaoAtividade::CartaoAtividade(const Identificador& identificador,
                               const Nome& nome, const Texto& descricao,
                               const Prioridade& prioridade, const Estado& estado,
                               const Tamanho& tamanho, const Timestamp& entrada,
                               const Identificador& quadroId,
                               const Email& desenvolvedorEmail)
    : identificador(identificador), nome(nome), descricao(descricao),
      prioridade(prioridade), estado(estado), tamanho(tamanho), entrada(entrada),
      inicio(), termino(), quadroId(quadroId),
      desenvolvedorEmail(desenvolvedorEmail) {}

const Identificador& CartaoAtividade::getIdentificador() const { return identificador; }
void CartaoAtividade::setIdentificador(const Identificador& novo) {
    exigirChaveInalterada(identificador.getValor(), novo.getValor());
}
const Nome& CartaoAtividade::getNome() const { return nome; }
void CartaoAtividade::setNome(const Nome& novo) { nome = novo; }
const Texto& CartaoAtividade::getDescricao() const { return descricao; }
void CartaoAtividade::setDescricao(const Texto& nova) { descricao = nova; }
const Prioridade& CartaoAtividade::getPrioridade() const { return prioridade; }
void CartaoAtividade::setPrioridade(const Prioridade& nova) { prioridade = nova; }
const Estado& CartaoAtividade::getEstado() const { return estado; }
void CartaoAtividade::setEstado(const Estado& novo) { estado = novo; }
const Tamanho& CartaoAtividade::getTamanho() const { return tamanho; }
void CartaoAtividade::setTamanho(const Tamanho& novo) { tamanho = novo; }
const Timestamp& CartaoAtividade::getEntrada() const { return entrada; }
void CartaoAtividade::setEntrada(const Timestamp& nova) { entrada = nova; }
const Timestamp& CartaoAtividade::getInicio() const { return inicio; }
void CartaoAtividade::setInicio(const Timestamp& novo) { inicio = novo; }
bool CartaoAtividade::temInicio() const { return !inicio.getValor().empty(); }
const Timestamp& CartaoAtividade::getTermino() const { return termino; }
void CartaoAtividade::setTermino(const Timestamp& novo) { termino = novo; }
bool CartaoAtividade::temTermino() const { return !termino.getValor().empty(); }
const Identificador& CartaoAtividade::getQuadroId() const { return quadroId; }
void CartaoAtividade::setQuadroId(const Identificador& novo) { quadroId = novo; }
const Email& CartaoAtividade::getDesenvolvedorEmail() const { return desenvolvedorEmail; }
void CartaoAtividade::setDesenvolvedorEmail(const Email& novo) {
    desenvolvedorEmail = novo;
}
