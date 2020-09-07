#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:


    void on_pushButton_del_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_refreash_clicked();

private:
    Ui::Widget *ui;
    int row, column;
    QSqlDatabase dbSQL;
    QSqlQuery query;
    QSqlRecord record;

    void refreash_table(void);

};

#endif // WIDGET_H
