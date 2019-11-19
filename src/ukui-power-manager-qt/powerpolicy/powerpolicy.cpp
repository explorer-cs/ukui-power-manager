#include "powerpolicy.h"
#include <stdio.h>
#include <stdlib.h>
#include "dbus_struct.h"

#define SAVEPOWER            0
#define PERFORMANCE          1
#define DEFAULT              2

PowerPolicy::PowerPolicy(QObject *parent) : QObject(parent)
{

}

void PowerPolicy:: set_integer(int argc)
{
    printf("set_integer: %d\n", argc);
}

void PowerPolicy:: set_string(QString argc)
{
    printf("set_string: %s\n", argc.toStdString().data());
}

void PowerPolicy:: set_variantlist(int cnt, QVariantList argc)
{
    printf("set_variantlist cnt: %d\n", cnt);
}

int PowerPolicy:: return_integer()
{
    int a1, a2;
    a1 = 10;
    a2 = 20;
    return a1 + a2;
}

bool PowerPolicy:: return_bool()
{
    bool result;
    result = true;
    return result;
}

QString PowerPolicy:: return_string()
{
    QString name = "zhangsan";
    return name;
}

QVariantList PowerPolicy::return_variantlist()
{
    QVariantList value;
    QVariant cnt;
    struct dbus_demo_example_struct demo[3];
    for(int i = 0; i < 3; i++)
    {
        demo[i].drv_ID = 1;
        demo[i].name   = "wangwu";
        demo[i].full_name = "lisi";
        demo[i].notify_mid = 2;
        cnt = QVariant::fromValue(demo[i]);
        value << cnt;
    }
    return value;
}

int PowerPolicy:: return_integet_and_set_integer(int argc_1, const QString &argc_2, QString &out1)
{
    int a;
    a = argc_1;
    out1 = argc_2;
    return a;
}

QString PowerPolicy:: return_string_and_set_string(const QString &argc_1)
{
    QString value;
    value = argc_1.toLower();
    return value;
}

QString PowerPolicy::control_tlp(int opt)
{
    if(opt == SAVEPOWER)
    {
        process("savepower");
    }
    else if(opt == PERFORMANCE)
    {
        process("performance");

    }
    else if(opt == DEFAULT)
    {
        process("default");

    }
    else
        ret.sprintf("undefined mode");
    return ret;

}

int PowerPolicy::process(char* option)
{
      QString cmd;
      cmd.sprintf("~/tlp/myshellc/test.sh %s",option);
      int rv = system(cmd.toStdString().c_str());
        if (WIFEXITED(rv))
        {
             printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
             if (0 == WEXITSTATUS(rv))
             {
                  // if command returning 0 means succeed
                  printf("command succeed");
                  ret.sprintf("command succeed");
             }
             else
             {
                  if(127 == WEXITSTATUS(rv))
                  {
                       printf("command not found\n");
                       ret.sprintf("command not found");
                       return WEXITSTATUS(rv);
                  }
                  else
                  {
                       printf("command failed: %d\n", strerror(WEXITSTATUS(rv)));
                       ret.sprintf("command failed");
                       return WEXITSTATUS(rv);
                  }
             }
         }
        else
        {
             printf("subprocess exit failed");
             ret.sprintf("subprocess exit failed");
             return -1;
        }
}

