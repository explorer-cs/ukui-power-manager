#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QRadioButton>
#include <QLabel>

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    QDBusInterface *interface;
    QRadioButton* saveradio;
    QRadioButton* perfradio;
    QRadioButton* defaultradio;
    QLabel *status;
public slots:
    void turnOn();
    void turnOff();
    void onSaveChecked(bool checked);
    void onPerfChecked(bool checked);
    void onDefaultChecked(bool checked);
};

#endif // WIDGET_H
