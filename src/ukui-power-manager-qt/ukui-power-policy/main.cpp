#include <QCoreApplication>
#include "powerpolicy.h"
#include "policy_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<dbus_demo_example_struct>("dbus_demo_example_struct");
    qDBusRegisterMetaType<dbus_demo_example_struct>();
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.registerService("ukui.power.policy")) {
        qDebug() << "error:" << connection.lastError().message();
        exit(-1);
    }

    PowerPolicy *power= new PowerPolicy();
    new PolicyAdaptor(power);
    connection.registerObject("/ukui/power/policy", power);
    return a.exec();
}
