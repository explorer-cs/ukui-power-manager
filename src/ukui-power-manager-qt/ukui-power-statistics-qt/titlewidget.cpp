#include "titlewidget.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>

TitleWidget::TitleWidget(QWidget *parent)
    : QWidget(parent)
    , m_colorR(153)
    , m_colorG(153)
    , m_colorB(153)
    , m_isPressed(false)
    , m_windowBorderWidth(0)
    , m_isTransparent(false)
{
    // 初始化;
    initControl();
    connectSlots();

}


// 初始化控件;
void TitleWidget::initControl()
{
//    m_pIcon = new QLabel;
    m_pTitleContent = new QLabel;

    m_pButtonMax = new QPushButton;
    m_pButtonClose = new QPushButton;


    m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    m_pTitleContent->setObjectName("TitleContent");
    m_pButtonMax->setObjectName("ButtonHelp");
    m_pButtonClose->setObjectName("ButtonClose");

    m_pButtonMax->setToolTip(tr("help"));
    m_pButtonMax->setIcon(QIcon(":/resource/icon/__con.png"));
    m_pButtonClose->setToolTip(tr("close"));
    m_pButtonClose->setIcon(QIcon(":/resource/icon/1px_delete_con.png"));
    QHBoxLayout* mylayout = new QHBoxLayout(this);
//    mylayout->addWidget(m_pIcon);
    mylayout->addWidget(m_pTitleContent);

    mylayout->addWidget(m_pButtonMax);
    mylayout->addWidget(m_pButtonClose);

    mylayout->setContentsMargins(10, 0, 16, 0);
    mylayout->setSpacing(30);

    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitleContent->setText(tr("Power Statistics-device infomation"));
    this->setFixedHeight(TITLE_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

// 信号槽的绑定;
void TitleWidget::connectSlots()
{
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonHelpClicked()));
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}


void TitleWidget::setBackgroundColor(int r, int g, int b, bool isTransparent)
{
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
    m_isTransparent = isTransparent;
    update();
}

// 设置标题栏图标;
void TitleWidget::setTitleIcon(QString filePath, QSize IconSize)
{
    QPixmap titleIcon(filePath);
    m_pIcon->setPixmap(titleIcon.scaled(IconSize));
}

// 设置标题内容;
void TitleWidget::setTitleContent(QString titleContent, int titleFontSize)
{
    // 设置标题字体大小;
    QFont font = m_pTitleContent->font();
    font.setPointSize(titleFontSize);
    m_pTitleContent->setFont(font);
    // 设置标题内容;
    m_pTitleContent->setText(titleContent);
    m_titleContent = titleContent;
}

// 设置标题栏长度;
void TitleWidget::setTitleWidth(int width)
{
    this->setFixedWidth(width);
}

// 设置窗口边框宽度;
void TitleWidget::setWindowBorderWidth(int borderWidth)
{
    m_windowBorderWidth = borderWidth;
}

// 绘制标题栏背景色;
void TitleWidget::paintEvent(QPaintEvent *event)
{
    // 是否设置标题透明;
//    if (!m_isTransparent)
//    {
//        //设置背景色;
//        QPainter painter(this);
//        QPainterPath pathBack;
//        pathBack.setFillRule(Qt::WindingFill);
//        pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
//        painter.setRenderHint(QPainter::Antialiasing, true);
//        painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));
//    }

    if (this->width() != (this->parentWidget()->width() - m_windowBorderWidth))
    {
        this->setFixedWidth(this->parentWidget()->width() - m_windowBorderWidth);
    }
    QWidget::paintEvent(event);
}

void TitleWidget::mousePressEvent(QMouseEvent *event)
{

    m_isPressed = true;
    m_startMovePos = event->globalPos();

    return QWidget::mousePressEvent(event);
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }
    return QWidget::mouseMoveEvent(event);
}

void TitleWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

// 加载本地样式文件;
void TitleWidget::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/" + sheetName + ".qss");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void TitleWidget::onButtonHelpClicked()
{
    emit signalButtonHelpClicked();
}

void TitleWidget::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}


