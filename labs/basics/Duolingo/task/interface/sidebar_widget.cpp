#include "sidebar_widget.h"

#include "styles.h"

#include <QStyle>

SidebarWidget::SidebarWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("sidebar");
    setStyleSheet(Styles::SIDEBAR);
    setupUi();
}

void SidebarWidget::setupUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* title = new QLabel("DuoLingo Clone", this);
    title->setObjectName("sidebar_title");
    layout->addWidget(title);

    auto* translation_btn = new QPushButton("Translation", this);
    translation_btn->setStyleSheet(Styles::SIDEBAR_BUTTON);
    connect(translation_btn, &QPushButton::clicked, this, &SidebarWidget::translationClicked);
    layout->addWidget(translation_btn);
    nav_buttons_.append(translation_btn);

    auto* grammar_btn = new QPushButton("Grammar", this);
    grammar_btn->setStyleSheet(Styles::SIDEBAR_BUTTON);
    connect(grammar_btn, &QPushButton::clicked, this, &SidebarWidget::grammarClicked);
    layout->addWidget(grammar_btn);
    nav_buttons_.append(grammar_btn);

    layout->addStretch();

    auto* stats_container = new QVBoxLayout();
    stats_container->setContentsMargins(30, 20, 30, 40);

    score_label_ = new QLabel("Score: 0", this);
    score_label_->setObjectName("stat_label");
    score_label_->setStyleSheet("color: #00ff88;");
    stats_container->addWidget(score_label_);

    hearts_label_ = new QLabel("Hearts: ❤️❤️❤️", this);
    hearts_label_->setObjectName("stat_label");
    hearts_label_->setStyleSheet("color: #ff0077;");
    stats_container->addWidget(hearts_label_);

    layout->addLayout(stats_container);
}

void SidebarWidget::setScore(int score) {
    score_label_->setText(QString("Score: %1").arg(score));
}

void SidebarWidget::setHearts(int hearts) {
    QString h = "Hearts: ";
    for (int i = 0; i < hearts; ++i) {
        h += "❤️";
    }
    hearts_label_->setText(h);
}

void SidebarWidget::setActiveButton(int index) {
    for (int i = 0; i < nav_buttons_.size(); ++i) {
        nav_buttons_[i]->setProperty("active", i == index);
        nav_buttons_[i]->style()->unpolish(nav_buttons_[i]);
        nav_buttons_[i]->style()->polish(nav_buttons_[i]);
    }
}
