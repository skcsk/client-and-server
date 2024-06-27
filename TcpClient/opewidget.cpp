#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{

    this->m_pFriend = new Friend;
    this->m_pBook = new Book;
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("课程");


    m_pSW = new QStackedWidget;
    m_pSW->addWidget(this->m_pFriend);
    m_pSW->addWidget(this->m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);
    setLayout(pMain);
    connect(m_pListW,&QListWidget::currentRowChanged,
            m_pSW,&QStackedWidget::setCurrentIndex);

}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::pFriend() const
{
    return m_pFriend;
}

Book *OpeWidget::getBook() const
{
    return this->m_pBook;
}
