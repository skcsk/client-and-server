#include "student_check_in.h"
#include "ui_student_check_in.h"

Student_Check_In::Student_Check_In(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Student_Check_In)
{
    this->setWindowTitle("qqq");
    ui->setupUi(this);
    this->hide();

}

Student_Check_In::~Student_Check_In()
{
    delete ui;
}

Student_Check_In &Student_Check_In::getInstance()
{
    static Student_Check_In instance;
    return instance;
}

void Student_Check_In::updateList(PDU *pdu)
{

    if(pdu==nullptr){
        return;
    }
    uint uiSize =pdu->uiMsgLen/64;
    char caName[65];
    ui->viewList->clear();
    for(uint i=0;i<uiSize;i++){
        memcpy(caName,(char*)(pdu->caMsg)+i*64,64);
        caName[64] = '\0';  // 确保字符串以空字符结尾
        ui->viewList->addItem(caName);

    }


}

