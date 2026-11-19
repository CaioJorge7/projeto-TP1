#ifndef ENTIDADES_H_INCLUDED
#define ENTIDADES_H_INCLUDED

#include "dominios.h"

/**
 * @file entidades.h
 * @brief Entidades do sistema de gestao de projetos Kanban (TP1).
 *
 * Todos os campos de dados sao instancias dos dominios existentes no projeto.
 * Os identificadores principais nao podem ser alterados apos a construcao.
 * As associacoes sao representadas por referencias tipadas por dominio.
 */

/**
 * @brief Usuario do sistema, que atua como GESTOR ou DESENVOLVEDOR.
 * @details O email e a chave primaria (PK) da entidade Pessoa.
 */
class Pessoa {
private:
    /** @brief E-mail da pessoa (chave primaria). */
    Email email;
    /** @brief Nome completo da pessoa. */
    Nome nome;
    /** @brief Senha de acesso. */
    Senha senha;
    /** @brief Papel desempenhado no sistema (GESTOR ou DESENVOLVEDOR). */
    Papel papel;
public:
    /** @brief Constroi uma pessoa a partir de dominios previamente validados. */
    Pessoa(const Email& email, const Nome& nome, const Senha& senha, const Papel& papel);

    /** @brief Retorna a chave primaria. */
    const Email& getEmail() const;
    /** @brief Nao permite modificar a chave primaria; aceita apenas valor identico. */
    void setEmail(const Email& email);
    /** @brief Retorna o nome. */
    const Nome& getNome() const;
    /** @brief Atualiza o nome. */
    void setNome(const Nome& nome);
    /** @brief Retorna a senha. */
    const Senha& getSenha() const;
    /** @brief Atualiza a senha. */
    void setSenha(const Senha& senha);
    /** @brief Retorna o papel. */
    const Papel& getPapel() const;
    /** @brief Atualiza o papel. */
    void setPapel(const Papel& papel);
};

/**
 * @brief Projeto sob responsabilidade de exatamente um gestor (Pessoa).
 * @details A referencia gestorEmail corresponde a Pessoa.email.
 * O relacionamento 1 gestor : 0..* projetos e verificado pela camada de servico.
 */
class Projeto {
private:
    /** @brief Codigo identificador do projeto (chave primaria). */
    Identificador identificador;
    /** @brief Nome do projeto. */
    Nome nome;
    /** @brief Descricao textual dos objetivos do projeto. */
    Texto descricao;
    /** @brief Data e horario de inicio planejado. */
    Timestamp inicio;
    /** @brief Data e horario de termino planejado. */
    Timestamp termino;
    /** @brief E-mail da pessoa gestora responsavel pelo projeto. */
    Email gestorEmail;
public:
    /** @brief Constroi projeto com sua referencia de gestor. */
    Projeto(const Identificador& identificador, const Nome& nome,
            const Texto& descricao, const Timestamp& inicio,
            const Timestamp& termino, const Email& gestorEmail);

    /** @brief Retorna a chave primaria. */
    const Identificador& getIdentificador() const;
    /** @brief Impede alterar chave primaria; aceita somente o mesmo identificador. */
    void setIdentificador(const Identificador& identificador);
    /** @brief Retorna o nome. */
    const Nome& getNome() const;
    /** @brief Atualiza o nome. */
    void setNome(const Nome& nome);
    /** @brief Retorna a descricao. */
    const Texto& getDescricao() const;
    /** @brief Atualiza a descricao. */
    void setDescricao(const Texto& descricao);
    /** @brief Retorna o momento inicial planejado. */
    const Timestamp& getInicio() const;
    /** @brief Atualiza o momento inicial planejado. */
    void setInicio(const Timestamp& inicio);
    /** @brief Retorna o momento final planejado. */
    const Timestamp& getTermino() const;
    /** @brief Atualiza o momento final planejado. */
    void setTermino(const Timestamp& termino);
    /** @brief Retorna a referencia de email do gestor. */
    const Email& getGestorEmail() const;
    /** @brief Atualiza a referencia do gestor; servico deve verificar existencia e papel. */
    void setGestorEmail(const Email& gestorEmail);
};

/**
 * @brief Quadro Kanban pertencente a exatamente um Projeto.
 * @details O limite e o numero maximo de cartoes simultaneos em FAZENDO.
 */
class Quadro {
private:
    /** @brief Codigo identificador do quadro (chave primaria). */
    Identificador identificador;
    /** @brief Nome do quadro. */
    Nome nome;
    /** @brief Limite maximo de cartoes simultaneos em FAZENDO (WIP). */
    Limite limite;
    /** @brief Identificador do projeto ao qual o quadro pertence. */
    Identificador projetoId;
public:
    /** @brief Constroi um quadro associado a um projeto. */
    Quadro(const Identificador& identificador, const Nome& nome,
           const Limite& limite, const Identificador& projetoId);

    /** @brief Retorna a chave primaria. */
    const Identificador& getIdentificador() const;
    /** @brief Impede alterar chave primaria; aceita apenas valor identico. */
    void setIdentificador(const Identificador& identificador);
    /** @brief Retorna o nome. */
    const Nome& getNome() const;
    /** @brief Atualiza o nome. */
    void setNome(const Nome& nome);
    /** @brief Retorna o limite de trabalho em andamento (WIP). */
    const Limite& getLimite() const;
    /** @brief Atualiza limite; servico deve respeitar WIP atual. */
    void setLimite(const Limite& limite);
    /** @brief Retorna a referencia para Projeto.identificador. */
    const Identificador& getProjetoId() const;
    /** @brief Atualiza a referencia; servico deve verificar integridade. */
    void setProjetoId(const Identificador& projetoId);
};

/**
 * @brief Cartao de atividade em um Quadro, atribuido a um Desenvolvedor.
 * @details Entrada existe desde a criacao. Inicio e termino ficam nao informados
 * (valor vazio) ate as transicoes de estado correspondentes; isso nao e um
 * formato Timestamp aceito para entrada, mas representa ausencia de valor.
 * A camada de servico garante transicoes e define esses timestamps.
 */
class CartaoAtividade {
private:
    /** @brief Codigo identificador do cartao (chave primaria). */
    Identificador identificador;
    /** @brief Nome descritivo da atividade. */
    Nome nome;
    /** @brief Detalhamento textual da atividade. */
    Texto descricao;
    /** @brief Prioridade da atividade (ALTA, MEDIA ou BAIXA). */
    Prioridade prioridade;
    /** @brief Estado no fluxo Kanban (A FAZER, FAZENDO ou FEITO). */
    Estado estado;
    /** @brief Tamanho estimado da atividade (GRANDE, MEDIO ou PEQUENO). */
    Tamanho tamanho;
    /** @brief Data e horario de criacao/entrada no quadro. */
    Timestamp entrada;
    /** @brief Data e horario em que o trabalho foi iniciado. */
    Timestamp inicio;
    /** @brief Data e horario em que o trabalho foi concluido. */
    Timestamp termino;
    /** @brief Identificador do quadro ao qual o cartao pertence. */
    Identificador quadroId;
    /** @brief E-mail da pessoa desenvolvedora responsavel pela execucao. */
    Email desenvolvedorEmail;
public:
    /** @brief Constroi cartao novo, sem inicio ou termino do trabalho. */
    CartaoAtividade(const Identificador& identificador, const Nome& nome,
                   const Texto& descricao, const Prioridade& prioridade,
                   const Estado& estado, const Tamanho& tamanho,
                   const Timestamp& entrada, const Identificador& quadroId,
                   const Email& desenvolvedorEmail);

    /** @brief Retorna chave primaria. */
    const Identificador& getIdentificador() const;
    /** @brief Impede alterar chave primaria; aceita apenas valor identico. */
    void setIdentificador(const Identificador& identificador);
    /** @brief Retorna o nome. */
    const Nome& getNome() const;
    /** @brief Atualiza o nome. */
    void setNome(const Nome& nome);
    /** @brief Retorna a descricao. */
    const Texto& getDescricao() const;
    /** @brief Atualiza a descricao. */
    void setDescricao(const Texto& descricao);
    /** @brief Retorna a prioridade. */
    const Prioridade& getPrioridade() const;
    /** @brief Atualiza a prioridade. */
    void setPrioridade(const Prioridade& prioridade);
    /** @brief Retorna o estado. */
    const Estado& getEstado() const;
    /** @brief Atualiza o estado; servico valida a transicao Kanban. */
    void setEstado(const Estado& estado);
    /** @brief Retorna o tamanho. */
    const Tamanho& getTamanho() const;
    /** @brief Atualiza o tamanho. */
    void setTamanho(const Tamanho& tamanho);
    /** @brief Retorna a data de entrada em A FAZER. */
    const Timestamp& getEntrada() const;
    /** @brief Atualiza a data de entrada; servico preserva a consistencia temporal. */
    void setEntrada(const Timestamp& entrada);
    /** @brief Retorna a data de entrada em FAZENDO, ou dominio vazio se ausente. */
    const Timestamp& getInicio() const;
    /** @brief Define o inicio do trabalho. */
    void setInicio(const Timestamp& inicio);
    /** @brief Informa se existe timestamp de inicio. */
    bool temInicio() const;
    /** @brief Retorna a data de entrada em FEITO, ou dominio vazio se ausente. */
    const Timestamp& getTermino() const;
    /** @brief Define o termino do trabalho. */
    void setTermino(const Timestamp& termino);
    /** @brief Informa se existe timestamp de termino. */
    bool temTermino() const;
    /** @brief Retorna referencia ao quadro que contem o cartao. */
    const Identificador& getQuadroId() const;
    /** @brief Atualiza quadro; servico deve validar a associacao e WIP. */
    void setQuadroId(const Identificador& quadroId);
    /** @brief Retorna email do desenvolvedor responsavel. */
    const Email& getDesenvolvedorEmail() const;
    /** @brief Atualiza responsavel; servico verifica existencia e papel. */
    void setDesenvolvedorEmail(const Email& desenvolvedorEmail);
};

#endif // ENTIDADES_H_INCLUDED
