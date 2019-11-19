#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#define BUTTON_HEIGHT 22        // 按钮高度;
#define BUTTON_WIDTH 30         // 按钮宽度;
#define TITLE_HEIGHT 32         // 标题栏高度;

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent);

    void setBackgroundColor(int r, int g, int b , bool isTransparent = false);
    void setTitleIcon(QString filePath , QSize IconSize = QSize(25 , 25));
    void setTitleContent(QString titleContent , int titleFontSize = 9);
    void setTitleWidth(int width);
    void setWindowBorderWidth(int borderWidth);

private:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    // 初始化控件;
public:

    void initControl();
    void connectSlots();
    // 加载样式文件;
    void loadStyleSheet(const QString &sheetName);

Q_SIGNALS:
    // 按钮触发的信号;

    void signalButtonHelpClicked();
    void signalButtonCloseClicked();

private Q_SLOTS:
    void onButtonHelpClicked();
    void onButtonCloseClicked();
private:
    QLabel* m_pIcon;                    // 标题栏图标;
    QLabel* m_pTitleContent;            // 标题栏内容;
    QPushButton* m_pButtonMax;          // 最大化按钮;
    QPushButton* m_pButtonClose;        // 关闭按钮;

    // 标题栏背景色;
    int m_colorR;
    int m_colorG;
    int m_colorB;

    // 移动窗口的变量;
    bool m_isPressed;
    QPoint m_startMovePos;
    // 标题栏内容;
    QString m_titleContent;
    // 窗口边框宽度;
    int m_windowBorderWidth;
    // 标题栏是否透明;
    bool m_isTransparent;
};
#endif // TITLEWIDGET_H
