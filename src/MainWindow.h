#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <QLabel>

DWIDGET_USE_NAMESPACE

class BlackjackWidget;

class MainWindow : public DMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    BlackjackWidget *blackjackWidget;
    QLabel *lblFichasMenu;
};

#endif // MAINWINDOW_H
