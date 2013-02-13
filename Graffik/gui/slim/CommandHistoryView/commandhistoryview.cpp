#include "commandhistoryview.h"

#include <QDebug>

CommandHistoryView::CommandHistoryView(QWidget *parent) : QTableView(parent) {

    m_modelSet = false;

    for( int i = 0; i <= 4; i++) {
        m_colWidths.append(60);
        setColumnWidth(i, 60);
    }

}

void CommandHistoryView::setModel(QAbstractItemModel *model) {
    QTableView::setModel(model);
    m_modelSet = true;
}

int CommandHistoryView::sizeHintForColumn(int p_col) {

    if( m_colWidths.count() >= p_col ) {
        return m_colWidths[p_col];
    }
    else
        return 0;
}

int CommandHistoryView::colWidth0() {
    return m_colWidths[0];
}

int CommandHistoryView::colWidth1() {
    return m_colWidths[1];
}

int CommandHistoryView::colWidth2() {
    return m_colWidths[2];
}

int CommandHistoryView::colWidth3() {
    return m_colWidths[3];
}

int CommandHistoryView::colWidth4() {
    return m_colWidths[4];
}

void CommandHistoryView::setColWidth0(int p_wid) {
    _setColWidth(0, p_wid);
}

void CommandHistoryView::setColWidth1(int p_wid) {
    _setColWidth(1, p_wid);
}

void CommandHistoryView::setColWidth2(int p_wid) {
    _setColWidth(2, p_wid);
}

void CommandHistoryView::setColWidth3(int p_wid) {
    _setColWidth(3, p_wid);
}

void CommandHistoryView::setColWidth4(int p_wid) {
    _setColWidth(4, p_wid);
}

void CommandHistoryView::_setColWidth(int p_col, int p_wid) {

    qDebug() << "SCHV: Got column size for" << p_col << p_wid;

    if( m_colWidths.count() < p_col )
        return;

    int wasWidth = m_colWidths[p_col];

    qDebug() << "SCHV: Old Size" << wasWidth;

    m_colWidths[p_col] = p_wid;

    setColumnWidth(p_col, p_wid);
    columnResized(p_col, wasWidth, p_wid);
}

QPixmap CommandHistoryView::errLED() {
    if( ! m_modelSet )
        return QPixmap();

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    return model->errLED();
}

QPixmap CommandHistoryView::okLED() {
    if( ! m_modelSet )
        return QPixmap();

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    return model->okLED();
}

QPixmap CommandHistoryView::bcastLED() {
    if( ! m_modelSet )
        return QPixmap();

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    return model->bcastLED();
}

QPixmap CommandHistoryView::queueLED() {
    if( ! m_modelSet )
        return QPixmap();

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    return model->queueLED();
}

void CommandHistoryView::setErrLED(QPixmap p_pix) {
    if( ! m_modelSet )
        return;

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    model->setErrLED(p_pix);
}

void CommandHistoryView::setOkLED(QPixmap p_pix) {
    if( ! m_modelSet )
        return;

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    model->setOkLED(p_pix);
}

void CommandHistoryView::setBcastLED(QPixmap p_pix) {
    if( ! m_modelSet )
        return;

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    model->setBcastLED(p_pix);
}

void CommandHistoryView::setQueueLED(QPixmap p_pix) {
    if( ! m_modelSet )
        return;

    CommandHistoryModel* model = reinterpret_cast<CommandHistoryModel*>( this->model() );
    model->setQueueLED(p_pix);
}
