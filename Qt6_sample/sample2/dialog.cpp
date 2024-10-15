#include "dialog.h"

// 槽函数
void Dialog::set_txt_underline(bool checked) {
    QFont font = txt->font();
    font.setUnderline(checked);
    txt->setFont(font);
}

void Dialog::set_txt_bold(bool checked) {
    QFont font = txt->font();
    font.setBold(checked);
    txt->setFont(font);
}

void Dialog::set_txt_italic(bool checked) {
    QFont font = txt->font();
    font.setItalic(checked);
    txt->setFont(font);
}

void Dialog::set_txt_color() {
    QPalette pal = txt->palette();
    if(radiobutton_red->isChecked()) {
        pal.setColor(QPalette::Text, Qt::red);
    }
    if(radiobutton_green->isChecked()) {
        pal.setColor(QPalette::Text, Qt::green);
    }
    if(radiobutton_blue->isChecked()) {
        pal.setColor(QPalette::Text, Qt::blue);
    }
    txt->setPalette(pal);
}

// 框架UI
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    // 多选框
    checkbox_bold = new QCheckBox("Bold");
    checkbox_italic = new QCheckBox("italic");
    checkbox_underline = new QCheckBox("underline");
    // 整合排列到水平组件框内
    QHBoxLayout *HLayC = new QHBoxLayout;
    HLayC->addWidget(checkbox_bold);
    HLayC->addWidget(checkbox_italic);
    HLayC->addWidget(checkbox_underline);

    // 单选框
    radiobutton_red = new QRadioButton("Red");
    radiobutton_green = new QRadioButton("Green");
    radiobutton_blue = new QRadioButton("Blue");
    // 整合排列到水平组件框内
    QHBoxLayout *HLayR = new QHBoxLayout;
    HLayR->addWidget(radiobutton_red);
    HLayR->addWidget(radiobutton_green);
    HLayR->addWidget(radiobutton_blue);

    // 文本框
    txt = new QPlainTextEdit;
    txt->setPlainText("This is a sentence");
    QFont font = txt->font();
    font.setPointSize(20);
    txt->setFont(font);

    // 按钮
    pushbutton_submit = new QPushButton("Submit");
    pushbutton_reset = new QPushButton("Reset");
    pushbutton_cancel = new QPushButton("Cancel");
    // 整合排列到水平组件框内
    QHBoxLayout *HLayP = new QHBoxLayout;
    HLayP->addWidget(pushbutton_submit);
    HLayP->addStretch();
    HLayP->addWidget(pushbutton_reset);
    HLayP->addStretch();
    HLayP->addWidget(pushbutton_cancel);

    // 整体排布并输出
    QVBoxLayout *VLay = new QVBoxLayout;
    VLay->addLayout(HLayC);
    VLay->addLayout(HLayR);
    VLay->addWidget(txt);     // 文本框不是排布，而是单独一个组件
    VLay->addLayout(HLayP);
    setLayout(VLay);        // 渲染输出

    // 槽函数连接
    connect(checkbox_bold, SIGNAL(clicked(bool)), this, SLOT(set_txt_bold(bool)));
    connect(checkbox_italic, SIGNAL(clicked(bool)), this, SLOT(set_txt_italic(bool)));
    connect(checkbox_underline, SIGNAL(clicked(bool)), this, SLOT(set_txt_underline(bool)));

    connect(radiobutton_red, SIGNAL(clicked(bool)), this, SLOT(set_txt_color()));
    connect(radiobutton_green, SIGNAL(clicked(bool)), this, SLOT(set_txt_color()));
    connect(radiobutton_blue, SIGNAL(clicked(bool)), this, SLOT(set_txt_color()));

    connect(pushbutton_submit, SIGNAL(clicked()), this, SLOT(accept()));
    connect(pushbutton_reset, SIGNAL(clicked()), this, SLOT(reject()));
    connect(pushbutton_cancel, SIGNAL(clicked()), this, SLOT(close()));
}

Dialog::~Dialog()
{
}

