#include "widget.h"
#include "ui_widget.h"
#include <QDateTime>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    dbSQL=QSqlDatabase::addDatabase("QMYSQL");//创建数据库对象
    dbSQL.setHostName("120.55.45.151"); //为本机的 IP
    dbSQL.setPort(3306); //端口号，一般数据库都为 3306
    dbSQL.setDatabaseName("HouseKeeper"); //数据库名
    dbSQL.setUserName("root"); //登录用户名 在创建数据库时设置的用户名和密码
    dbSQL.setPassword("3342949"); //登录密码
    if(!dbSQL.open())
        qDebug()<<"this,warning,failure";
    else
        qDebug()<<"this,ok,success";

    query = QSqlQuery("SELECT * FROM V1");
    record = query.record();


    refreash_table();


}

Widget::~Widget()
{
    delete ui;
}




void Widget::refreash_table(void)
{
    query = QSqlQuery("SELECT * FROM V1");
    record = query.record();
    row = query.size();
    column = record.count();
    qDebug() << "表的列数：" << column;         // 表一共有多少列
    qDebug() << "表的行数：" << row;       // 表一共有多少行
    ui->tableWidget_db->setRowCount(row);
    ui->tableWidget_db->setColumnCount(column);//列

    QStringList header;
    //将表头写入表格
    for(int i = 0; i < column; i++){
        header.append(record.fieldName(i));
    }
//    qDebug()<< record.fieldName(0);
//    qDebug()<< record.fieldName(1);
//    qDebug()<< record.fieldName(2);
//    header<<"id"<<"opreation"<<"time";
    ui->tableWidget_db->setHorizontalHeaderLabels(header);
    //自动调整宽度
    ui->tableWidget_db->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_db->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_db->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    for (int i = 0; i < row; i++) {
        query.seek(i);
        for(int j = 0; j < column; j++){
            ui->tableWidget_db->setItem(i,j,new QTableWidgetItem(query.value(j).toString()));
        }
    }
}




void Widget::on_pushButton_del_clicked()
{
    int currentRow_id = ui->tableWidget_db->item(ui->tableWidget_db->currentRow(),0)->text().toInt();
    QString sql = QString("DELETE FROM test WHERE id=%1").arg(currentRow_id);
    query.exec(sql);
    refreash_table();

}

void Widget::on_pushButton_clear_clicked()
{
    ui->tableWidget_db->clear();
}

void Widget::on_pushButton_refreash_clicked()
{
    refreash_table();
}
