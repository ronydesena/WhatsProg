#include <QMessageBox>
#include <iostream>
#include "whatsprogmain.h"
#include "ui_whatsprogmain.h"
#include "modelconversas.h"
#include "modelmensagens.h"
#include "dados_cliente.h"
#include "socket_whatsprog.h"
#include "dados_whatsprog.h"
#include <QCloseEvent>

using namespace std;

// Esta é a thread que recebe as informacoes do socket e
// armazena na variavel DCliente, de onde elas poderao ser
// exibidas pela interface visual
DWORD WINAPI avisa_lida(LPVOID lpParameter)//avisa que a msg foi lida
{
    cerr<<"entrou na thread2\n";
    WhatsProgMain *prov;
    prov = (WhatsProgMain *)lpParameter;
    while (s.connected()){
        for (unsigned i=0; i<DCliente[DCliente.getIdConversa()].size(); i++){
            if (DCliente[DCliente.getIdConversa()][i].getRemetente() == DCliente[DCliente.getIdConversa()].getCorrespondente() &&
                DCliente[DCliente.getIdConversa()][i].getStatus() == MSG_ENTREGUE){
                DCliente[DCliente.getIdConversa()][i].setStatus(MSG_LIDA);
                if(s.write_int(CMD_MSG_LIDA1) != SOCKET_ERROR)
                    if(s.write_int(DCliente[DCliente.getIdConversa()][i].getId()) != SOCKET_ERROR)
                        if(s.write_string(DCliente[DCliente.getIdConversa()][i].getRemetente()) != SOCKET_ERROR)
                            cerr<<"ID: "<<DCliente[DCliente.getIdConversa()][i].getId()<<" - LIDA por vc\n"
                                <<"     conv: "<<DCliente.getIdConversa();

                        else cerr<<"\n\n\n\nnão enviou CMD_MSG_LIDA1\n\n\n\n-------------------------------------------\n";
                emit prov->conversasModificada();
                emit prov->mensagensModificada();
                emit prov->statusModificada();
            }
        }
    }
    cerr<<"saiu da thread\n";
    return 0;
}
DWORD WINAPI le_msg(LPVOID lpParameter){
    cerr<<"entrou na thread\n";
    WhatsProgMain *prov;
    prov = (WhatsProgMain *)lpParameter;
    int32_t cmd;
    bool conexaoOK=0;
    while (s.connected()){
        conexaoOK = (s.read_int(cmd,1000*TIMEOUT_WHATSPROG) == sizeof(cmd));
        if(conexaoOK){
            cerr<<"cmd: "<<cmd<<"  ";

            if(cmd==CMD_NOVA_MSG){//1005

                int32_t id;
                string usuario,text;
                conexaoOK = (s.read_int(id,1000*TIMEOUT_WHATSPROG) == sizeof(id));
                if(conexaoOK) conexaoOK = (s.read_string(usuario,1000*TIMEOUT_WHATSPROG));
                if(conexaoOK) conexaoOK = (s.read_string(text,1000*TIMEOUT_WHATSPROG));
    //Sera que vai?
                cerr<<"Usuario selecionado: "<<DCliente[DCliente.getIdConversa()].getCorrespondente()<<"\n";

                cerr<<"\n-------------"
                      "\nDe: \t"<<usuario<<
                      "\nTexto: "<<text<<
                      "\nID: "<<id<<
                      "\n-------------";
                Mensagem nova;
                nova.setRemetente(usuario);
                nova.setTexto(text);
                nova.setId(id);
                nova.setStatus(MSG_ENTREGUE);
                nova.setDestinatario(DCliente.getMeuUsuario());

                bool dest_ok=0;
                for(int i = 0; i<DCliente.size(); i++){
                    if(DCliente[i].getCorrespondente()==usuario){
                        DCliente[i].insertMessage(nova);
                        dest_ok =1;
                    }
                }
                if (!dest_ok){
                    DCliente.insertConversa(usuario);
                    DCliente[DCliente.size()-1].insertMessage(nova);
                }

                emit prov->conversasModificada();
                emit prov->mensagensModificada();
                emit prov->statusModificada();
            }

            else if((cmd==CMD_MSG_RECEBIDA) || (cmd==CMD_MSG_ENTREGUE) || (cmd==CMD_MSG_LIDA2)){//1006  ||  1007  ||  1009
                int32_t id;
                conexaoOK = (s.read_int(id,1000*TIMEOUT_WHATSPROG) == sizeof(id));
                if(conexaoOK){
                    for(int32_t conv=0; conv<DCliente.size(); conv++)
                        for(int32_t mens =0; mens<DCliente[conv].size(); mens++)
                            if((id==DCliente[conv][mens].getId())){
                                if(cmd==CMD_MSG_RECEBIDA)
                                    DCliente[conv][mens].setStatus(MSG_RECEBIDA);
                                else if(cmd==CMD_MSG_ENTREGUE)
                                    DCliente[conv][mens].setStatus(MSG_ENTREGUE);
                                else if(cmd==CMD_MSG_LIDA2)
                                    DCliente[conv][mens].setStatus(MSG_LIDA);
                            }
                    cerr<<"CMD: "<<cmd<<"  ID: "<<id<<"\n";
                    emit prov->conversasModificada();
                    emit prov->mensagensModificada();
                    emit prov->statusModificada();
                }
            }


        }
    }

    cerr<<"saiu da thread\n";
  return 0;
}

WhatsProgMain::WhatsProgMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WhatsProgMain)
{
    ui->setupUi(this);

    // A lista da esquerda (conversas)
    modelConversas = new ModelConversas(this);
    ui->tableViewConversas->setModel( modelConversas );
    ui->tableViewConversas->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableViewConversas->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableViewConversas->horizontalHeader()->setSectionsClickable(false);
    ui->tableViewConversas->setShowGrid(false);
    ui->tableViewConversas->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewConversas->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewConversas->setTabKeyNavigation(false);

    // A lista da direita (mensagens)
    modelMensagens = new ModelMensagens(this);
    ui->tableViewMensagens->setModel( modelMensagens );
    ui->tableViewMensagens->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tableViewMensagens->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableViewMensagens->horizontalHeader()->setSectionsClickable(false);
    ui->tableViewMensagens->setShowGrid(true);
    ui->tableViewMensagens->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableViewMensagens->setTabKeyNavigation(false);

    // A barra de status

    if (DCliente.getMeuUsuario() == "")
    {
        nomeUsuario = new QLabel("");
        conectado = new QLabel("NÃO CONECTADO");
    }
    else
    {
        conectado = new QLabel("CONECTADO: ");
        nomeUsuario = new QLabel(DCliente.getMeuUsuario().c_str());
    }
    statusBar()->insertWidget(0,conectado);
    statusBar()->insertWidget(1,nomeUsuario);


    if (DCliente.getServidor() == "")
    {
        ipServidor = new QLabel("");
        arroba = new QLabel("");
    }
    else
    {
        arroba = new QLabel("@");
        ipServidor = new QLabel(DCliente.getServidor().c_str());
    }
    statusBar()->insertWidget(2,arroba);
    statusBar()->insertWidget(3,ipServidor);


    if (DCliente.getIdConversa() < 0)
    {
        nomeConversa = new QLabel("");
    }
    else
    {
        nomeConversa = new QLabel(DCliente[DCliente.getIdConversa()].getCorrespondente().c_str());
    }

    // A caixa de dialogo de login
    loginDialog = new LoginDialog(this);
    novamsg = new nova_msg(this);

    // As conexoes
    connect(this, SIGNAL (conversasModificada()),
            modelConversas, SLOT (slotAtualizaConversas()));
    connect(this, SIGNAL (numMsgConversaModificado(unsigned)),
            modelConversas, SLOT (slotAtualizaNumMsgConversa(unsigned)));
    connect(this, SIGNAL (mensagensModificada()),
            modelMensagens, SLOT (slotAtualizaMensagens()));
    connect(this, SIGNAL (statusModificada()),
            this, SLOT (slotAtualizaBarraStatus()));
    connect(loginDialog, SIGNAL (aceitaUsuario(const string, const string, const string, bool)),
            this, SLOT (slotAceitaUsuario(const string, const string, const string, bool)));
    connect(this, SIGNAL (removerUsuario()),
            this, SLOT (slotRemoverUsuario()));
    connect(novamsg, SIGNAL (aceitaDestinatario(const string)),
            this, SLOT (slotAceitarDestinatario(const string)));
    connect(this, SIGNAL (mensagemClienteServidor(int32_t, const string, const string, const string, MsgStatus)),
            this, SLOT (slotMensagemClienteServidor(int32_t, const string, const string, const string, MsgStatus)));
}

WhatsProgMain::~WhatsProgMain()
{
    delete ui;
}

void WhatsProgMain::slotRemoverUsuario()
{
    s.write_int(CMD_LOGOUT_USER);
    ui->actionNovo->setEnabled(true);
    ui->actionConectar->setEnabled(true);
    ui->actionDesconectar->setDisabled(true);
    s.close();
}

void WhatsProgMain::slotAtualizaBarraStatus()
{
    if (DCliente.getServidor() == "")
    {
        conectado->setText("");
        ipServidor->setText("");
        arroba->setText("");
    }
    else
    {
        conectado->setText("CONECTADO: ");
        ipServidor->setText(DCliente.getServidor().c_str());
        arroba->setText("@");
    }

    if (DCliente.getMeuUsuario() == "")
    {
        nomeUsuario->setText("");
    }
    else
    {
        nomeUsuario->setText(DCliente.getMeuUsuario().c_str());
    }

    if (DCliente.getIdConversa() < 0)
    {
        nomeConversa->setText("");
    }
    else
    {
        nomeConversa->setText(DCliente[DCliente.getIdConversa()].getCorrespondente().c_str());
    }
}

void WhatsProgMain::slotAceitaUsuario(const string &IP, const string &login,
                                      const string &senha, bool novoUsuario )
{
    if (IP == "")
    {
        QMessageBox::warning(this, "Login", "IP inválido.");
        return;
    }
    if (login.size()<TAM_MIN_NOME_USUARIO || login.size()>TAM_MAX_NOME_USUARIO)
    {
        QMessageBox::warning(this, "Login", "Usuário inválido.");
        return;
    }
    if (login == DCliente.getMeuUsuario())
    {
        QMessageBox::warning(this, "Login", "Usuário já conectado.");
        return;
    }
    if (senha.size()<TAM_MIN_SENHA || senha.size()>TAM_MAX_SENHA)
    {
        QMessageBox::warning(this, "Login", "Senha inválida.");
        return;
    }

    // Desconecta o socket, encerra a thread de leitura de dados,
    // limpa o servidor e usuario atuais, limpa todas as conversas,
    // redesenha todas as janelas
    on_actionDesconectar_triggered();

    // Conecta o socket
    if (s.connect(IP.c_str(), PORTA_WHATSPROG) != SOCKET_OK)
    {
        cout << IP.c_str();
        cout << PORTA_WHATSPROG;
        if(s.connected())
        {
            cout << "ERRO ";
        }
        string msg = "Impossivel conectar com servidor "+IP+" na porta "+PORTA_WHATSPROG;
        QMessageBox::warning(this, "Erro de conexão", msg.c_str());
        return;
    }

    // Envia a msg de conexao para o servidor, atraves do socket
    bool conexaoOK = true;
    if (novoUsuario)
    {
        // Testa se eh possivel cadastrar um novo usuario com esses ip, login e senha
        // Envia o comando CMD_NEW_USER
        if (conexaoOK) conexaoOK = (s.write_int(CMD_NEW_USER) != SOCKET_ERROR);
    }
    else
    {
        // Testa se eh possivel se conectar com esses ip, login e senha
        // Envia o comando CMD_LOGIN_USER
        if (conexaoOK) conexaoOK = (s.write_int(CMD_LOGIN_USER) != SOCKET_ERROR);
    }
    // Envia os parametros do comando (login, senha)
    if (conexaoOK) conexaoOK = (s.write_string(login) != SOCKET_ERROR);
    if (conexaoOK) conexaoOK = (s.write_string(senha) != SOCKET_ERROR);
    if (!conexaoOK)
    {
        QMessageBox::warning(this, "Erro de conexão", "Erro no envio da conexão.");
        return;
    }

    int32_t cmd;
    conexaoOK = (s.read_int(cmd,1000*TIMEOUT_WHATSPROG) == sizeof(cmd));
    if (conexaoOK) conexaoOK = (cmd == CMD_LOGIN_OK);
    if (!conexaoOK)
    {
        QMessageBox::warning(this, "Erro de conexão", "Erro na leitura da resposta à conexão.");
        return;
    }

    // Lanca a thread de leitura dos dados do socket
    // Cria a thread que escreve as mensagens recebidas
    tHandle = CreateThread(NULL, 0, le_msg, this , 0, NULL);
    tHandle2 = CreateThread(NULL, 0, avisa_lida, this , 0, NULL);

    if (tHandle == NULL){
        s.close();
        QMessageBox::warning(this, "Erro de conexão", "Erro na criação da thread de leitura do socket.");
        return;
    }
    if (tHandle2 == NULL){
        s.close();
        QMessageBox::warning(this, "Erro de conexão", "Erro na criação da thread de leitura do socket.");
        return;
    }
    // Armazena o novo nome do servidor e do usuário
    DCliente.setServidorUsuario(IP,login);

    // Redesenha todas as janelas
    emit conversasModificada();
    emit mensagensModificada();
    emit statusModificada();

    // Informa que o novo usuario estah conectado
    QMessageBox::information(this, "Login", "Usuário conectado.");
    ui->actionNovo->setDisabled(true);
    ui->actionConectar->setDisabled(true);
    ui->actionDesconectar->setEnabled(true);
}

void WhatsProgMain::on_actionNovo_triggered()
{
    loginDialog->setUsuario(true);
}

void WhatsProgMain::on_actionConectar_triggered()
{
    loginDialog->setUsuario(false);
}

// Desconecta o socket, encerra a thread de leitura de dados,
// limpa o servidor e usuario atuais, limpa todas as conversas,
// redesenha todas as janelas
void WhatsProgMain::on_actionDesconectar_triggered()
{
    if (s.connected())
    {
        // Envia o comando de logout
        emit removerUsuario();
        QMessageBox::information(this, "Logout", "Usuário desconectado.");
        // Fecha o socket e, consequentemente,
        // encerra a thread de leitura de dados do socket
        //s.close();
    }

    // Limpa os nomes do servidor e usuario
    DCliente.clearServidorUsuario();

    // Redesenha todas as janelas
    emit conversasModificada();
    emit mensagensModificada();
    emit statusModificada();
}

void WhatsProgMain::on_actionSair_triggered()
{
    on_actionDesconectar_triggered();
    QCoreApplication::quit();
}

void WhatsProgMain::on_tableViewConversas_activated(const QModelIndex &index)
{
    on_tableViewConversas_clicked(index);
}

void WhatsProgMain::on_tableViewConversas_clicked(const QModelIndex &index)
{
    if (index.row() >= (int)DCliente.size())
    {
        cerr << "ID de conversa invalida.\n";
        return;
    }

    if (index.row() != DCliente.getIdConversa())
    {
        DCliente.setIdConversa(index.row());

        // Muda o status de todas as msgs que foram enviadas para mim de RECEBIDA -> LIDA
        bool houve_leitura = false;
        /*for (unsigned i=0; i<DCliente[DCliente.getIdConversa()].size(); i++){
            if (DCliente[DCliente.getIdConversa()][i].getRemetente() == DCliente[DCliente.getIdConversa()].getCorrespondente() &&
                DCliente[DCliente.getIdConversa()][i].getStatus() == MSG_ENTREGUE)
            {
                DCliente[DCliente.getIdConversa()][i].setStatus(MSG_LIDA);
                if(s.connected()){
                    if(s.write_int(CMD_MSG_LIDA1) != SOCKET_ERROR)
                    if(s.write_int(DCliente[DCliente.getIdConversa()][i].getId()) != SOCKET_ERROR)
                    if(s.write_string(DCliente[DCliente.getIdConversa()][i].getRemetente()) != SOCKET_ERROR)
                        cerr<<"ID: "<<DCliente[DCliente.getIdConversa()][i].getId()<<" - LIDA1 pelo destinatario\n";

                }
                // Envia msg ao servidor informando que a msg foi lida


                houve_leitura = true;
            }
        }

    */
        if (houve_leitura)
        {
            emit numMsgConversaModificado(DCliente.getIdConversa());
        }

        // Sinaliza para atualizar a janela de mensagens e a barra de status
        emit mensagensModificada();
        emit statusModificada();
    }
}

void WhatsProgMain::on_lineEditMensagem_returnPressed()
{
    if (DCliente.getMeuUsuario().size() < TAM_MIN_NOME_USUARIO)
    {
        QMessageBox::warning(this, "Usuário inválido", "Usuário não definido.");
        return;
    }
    if (DCliente.getIdConversa()<0 || DCliente.getIdConversa()>=(int)DCliente.size())
    {
        QMessageBox::warning(this, "Conversa inválida", "Conversa não selecionada.");
        return;
    }
    string arg1 = ui->lineEditMensagem->text().toStdString();
    if (arg1.size()==0 || arg1.size()>TAM_MAX_MSG)
    {
        QMessageBox::warning(this, "Msg inválida", "Tamanho da mensagem inválido.");
        return;
    }
    ui->lineEditMensagem->clear();

    Mensagem M;
    // Define os campos da msg
    M.setId(DCliente.getNovaIdMensagem());
    // Nome do remetente
    M.setRemetente(DCliente.getMeuUsuario());
    // Nome do destinatario
    M.setDestinatario(DCliente[DCliente.getIdConversa()].getCorrespondente());
    // Texto da mensagem
    M.setTexto(arg1);
    // Estado (status) da mensagem
    M.setStatus(MSG_ENVIADA);

    emit mensagemClienteServidor(M.getId(), M.getRemetente(), M.getDestinatario(), M.getTexto(), M.getStatus());
    /** COLOQUEI ESSE EMIT AQUI **/
    // Inclui a msg na base de dados local

    // Acrescenta no final da conversa
    DCliente[DCliente.getIdConversa()].insertMessage(M);
    // Sinaliza que houve alteracao no numero de msgs de uma conversa na janela de conversas
    emit numMsgConversaModificado(DCliente.getIdConversa());
    // Sinaliza que houve alteracao na janela de Mensagens
    emit mensagensModificada();
}

//FAZENDO JANELA PARA CRIAR UMA NOVA MENSAGEMconst string &id, const string &rem, const string &dest, const string &txt, MsgStatus status
void WhatsProgMain::on_actionNova_conversa_triggered()
{
    novamsg->setNova();
}

void WhatsProgMain::slotAceitarDestinatario(const string &dest)
{
    DCliente.insertConversa(dest);
    emit conversasModificada();
}

void WhatsProgMain::slotMensagemClienteServidor(int32_t id, const string &rem, const string &dest, const string &txt, MsgStatus status)
{
    emit conversasModificada();
    bool conexaoOK = true;
    if(s.connected())
    {
        conexaoOK = (s.write_int(CMD_NOVA_MSG) != SOCKET_ERROR);
        if(conexaoOK) conexaoOK = (s.write_int(id) != SOCKET_ERROR);
        if(conexaoOK) conexaoOK = (s.write_string(dest) != SOCKET_ERROR);
        if(conexaoOK) conexaoOK = (s.write_string(txt) != SOCKET_ERROR);
        if (!conexaoOK)        {
            QMessageBox::warning(this, "Erro de conexão", "Erro no envio da conexão.");
            on_actionDesconectar_triggered();
            return;
        }
    }
}

void WhatsProgMain::on_actionRemover_conversa_triggered()
{
    //DCliente[DCliente.getIdConversa()].clearMessages();
    DCliente.eraseConversa(DCliente.getIdConversa());
    emit conversasModificada();
    emit mensagensModificada();
    emit statusModificada();
}

void WhatsProgMain::closeEvent (QCloseEvent *event)
{
    on_actionDesconectar_triggered();
    /*QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Fechar janela",
                                                                tr("Tem certeza?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }*/
}

