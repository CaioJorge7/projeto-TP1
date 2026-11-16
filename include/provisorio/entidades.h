#ifndef ENTIDADES_H_INCLUDED
#define ENTIDADES_H_INCLUDED

#include "dominios.h"

// TODO(Membro2): ARQUIVO PROVISORIO. Entidades escritas so para o Membro 3
// poder compilar a camada de servico. Trocar pelas entidades oficiais
// (nomes de classe e de metodos podem mudar).
/**
 * @brief Entidade Pessoa (usuario do sistema).
 */
class Pessoa {
private:
    Email email;      // chave primaria
    Nome nome;
    Senha senha;
    Papel papel;

public:
    void setEmail(const Email& email) { this->email = email; }
    Email getEmail() const { return email; }

    void setNome(const Nome& nome) { this->nome = nome; }
    Nome getNome() const { return nome; }

    void setSenha(const Senha& senha) { this->senha = senha; }
    Senha getSenha() const { return senha; }

    void setPapel(const Papel& papel) { this->papel = papel; }
    Papel getPapel() const { return papel; }
};

/**
 * @brief Entidade Projeto.
 */
class Projeto {
private:
    Identificador identificador;   // chave primaria
    Nome nome;
    Texto descricao;
    Timestamp inicio;
    Timestamp termino;

public:
    void setIdentificador(const Identificador& identificador) { this->identificador = identificador; }
    Identificador getIdentificador() const { return identificador; }

    void setNome(const Nome& nome) { this->nome = nome; }
    Nome getNome() const { return nome; }

    void setDescricao(const Texto& descricao) { this->descricao = descricao; }
    Texto getDescricao() const { return descricao; }

    void setInicio(const Timestamp& inicio) { this->inicio = inicio; }
    Timestamp getInicio() const { return inicio; }

    void setTermino(const Timestamp& termino) { this->termino = termino; }
    Timestamp getTermino() const { return termino; }
};

/**
 * @brief Entidade Quadro (quadro Kanban).
 */
class Quadro {
private:
    Identificador identificador;   // chave primaria
    Nome nome;
    Limite limite;                 // limite de WIP

public:
    void setIdentificador(const Identificador& identificador) { this->identificador = identificador; }
    Identificador getIdentificador() const { return identificador; }

    void setNome(const Nome& nome) { this->nome = nome; }
    Nome getNome() const { return nome; }

    void setLimite(const Limite& limite) { this->limite = limite; }
    Limite getLimite() const { return limite; }
};

/**
 * @brief Entidade Cartao de atividade.
 *
 * Os campos entrada, inicio e termino ficam com valor vazio ate o cartao
 * chegar em A FAZER, FAZENDO e FEITO, respectivamente.
 */
// TODO(Membro2): o diagrama chama de "Cartao de atividade"; nome da classe a definir.
class Cartao {
private:
    Identificador identificador;   // chave primaria
    Nome nome;
    Texto descricao;
    Prioridade prioridade;
    Estado estado;
    Tamanho tamanho;
    Timestamp entrada;             // entrou em A FAZER
    Timestamp inicio;              // entrou em FAZENDO
    Timestamp termino;             // chegou em FEITO

public:
    void setIdentificador(const Identificador& identificador) { this->identificador = identificador; }
    Identificador getIdentificador() const { return identificador; }

    void setNome(const Nome& nome) { this->nome = nome; }
    Nome getNome() const { return nome; }

    void setDescricao(const Texto& descricao) { this->descricao = descricao; }
    Texto getDescricao() const { return descricao; }

    void setPrioridade(const Prioridade& prioridade) { this->prioridade = prioridade; }
    Prioridade getPrioridade() const { return prioridade; }

    void setEstado(const Estado& estado) { this->estado = estado; }
    Estado getEstado() const { return estado; }

    void setTamanho(const Tamanho& tamanho) { this->tamanho = tamanho; }
    Tamanho getTamanho() const { return tamanho; }

    void setEntrada(const Timestamp& entrada) { this->entrada = entrada; }
    Timestamp getEntrada() const { return entrada; }

    void setInicio(const Timestamp& inicio) { this->inicio = inicio; }
    Timestamp getInicio() const { return inicio; }

    void setTermino(const Timestamp& termino) { this->termino = termino; }
    Timestamp getTermino() const { return termino; }
};

#endif // ENTIDADES_H_INCLUDED
