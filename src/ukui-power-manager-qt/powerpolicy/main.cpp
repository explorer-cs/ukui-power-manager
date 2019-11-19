#include <QCoreApplication>
#include "powerpolicy.h"
#include "manager_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<dbus_demo_example_struct>("dbus_demo_example_struct");
    qDBusRegisterMetaType<dbus_demo_example_struct>();
    QDBusConnection connection = QDBusConnection::systemBus();

    if (!connection.registerService("tlp.local.manager")) {
        qDebug() << "error:" << connection.lastError().message();
        exit(-1);
    }

    PowerPolicy *power= new PowerPolicy();
    new ManagerAdaptor(power);
    connection.registerObject("/tlp/local/manager", power);
    return a.exec();
}
