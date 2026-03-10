#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupStWidget();
    setWindowTitle("топ 1 приложение для математике");
    resize(450, 300);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupStWidget()
{
    stackedWidget = new QStackedWidget(this);

    setupMenu();
    setupScene1Widget();
    setupScene2Widget();
    setupScene3Widget();
    setupMenuBar();

    stackedWidget->addWidget(menuWidget);
    stackedWidget->addWidget(Scene1Widget);
    stackedWidget->addWidget(Scene2Widget);
    stackedWidget->addWidget(Scene3Widget);

    setCentralWidget(stackedWidget);
    stackedWidget->setCurrentWidget(menuWidget);
}

void MainWindow::setupMenu()
{
    menuWidget = new QWidget(stackedWidget);

    button1 = new QPushButton("Факторизация", menuWidget);
    button2 = new QPushButton("КТО", menuWidget);
    button3 = new QPushButton("С днём рождения!", menuWidget);

    button1->setMinimumWidth(150);
    button2->setMinimumWidth(150);
    button3->setMinimumWidth(150);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(15);
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);
    buttonLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout *centerLayout = new QHBoxLayout(menuWidget);
    centerLayout->addStretch();
    centerLayout->addLayout(buttonLayout);
    centerLayout->addStretch();

    menuWidget->setLayout(centerLayout);


    QString imageFileName = "background.png";
    QString imagePathInQSS = "labs/basics/" + imageFileName;
    imagePathInQSS.replace("\\", "/");

    menuWidget->setStyleSheet(QString(
        "QWidget#menuWidgetId {"
        "    border-image: url(\"%1\") 0 0 0 0 stretch stretch;"
        "}"
    ).arg(imagePathInQSS));
    menuWidget->setObjectName("menuWidgetId");
    connect(button1, &QPushButton::clicked, this, &MainWindow::onButton1Clicked);
    connect(button2, &QPushButton::clicked, this, &MainWindow::onButton2Clicked);
    connect(button3, &QPushButton::clicked, this, &MainWindow::onButton3Clicked);
}

void MainWindow::setupScene1Widget()
{
    Scene1Widget = new QWidget(stackedWidget);
    QVBoxLayout *sceneLayout = new QVBoxLayout(Scene1Widget);

    QPushButton *crossButton;
    crossButton = new QPushButton(QString::fromUtf8("✕"), Scene1Widget);
    crossButton->setFixedSize(30, 30);
    crossButton->setToolTip("Вернуться в меню");
    crossButton->setStyleSheet(
        "QPushButton {"
        "  color: red;"
        "  background-color: transparent;"
        "  border: none;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { color: darkred; }"
        "QPushButton:pressed { color: firebrick; }"
    );
    connect(crossButton, &QPushButton::clicked, this, &MainWindow::showMenu);

    QHBoxLayout *topBarLayout = new QHBoxLayout();
    topBarLayout->addStretch();
    topBarLayout->addWidget(crossButton);

    QLabel *inputPromptLabel = new QLabel("Введите число не больше long long:", Scene1Widget);
    inputPromptLabel->setAlignment(Qt::AlignLeft);

    numberInputLineEdit = new QLineEdit(Scene1Widget);
    numberInputLineEdit->setPlaceholderText("42");
    numberInputLineEdit->setMinimumWidth(200);

    QGridLayout *keypadLayout = new QGridLayout();
    keypadLayout->setSpacing(5);

    const struct { QString text; int row; int col; } buttonData[] = {
        {"7", 0, 0}, {"8", 0, 1}, {"9", 0, 2},
        {"4", 1, 0}, {"5", 1, 1}, {"6", 1, 2},
        {"1", 2, 0}, {"2", 2, 1}, {"3", 2, 2},
        {"C",3, 0}, {"0", 3, 1}, {"←", 3, 2}
    };

    for (const auto& btnData : buttonData) {
        QPushButton* button = new QPushButton(btnData.text, Scene1Widget);
        button->setFixedSize(50, 40);

        if (btnData.text.length() == 1 && btnData.text.at(0).isDigit()) {
            connect(button, &QPushButton::clicked, this, [this, text = btnData.text]() {
                numberInputLineEdit->insert(text);
                numberInputLineEdit->setFocus();
            });
        } else if (btnData.text == "←") {
            button->setStyleSheet("color: red; font-weight: bold;");
            connect(button, &QPushButton::clicked, this, [this]() {
                numberInputLineEdit->backspace();
                numberInputLineEdit->setFocus();
            });
        } else if (btnData.text == "C") {
            button->setStyleSheet("color: orange; font-weight: bold;");
            connect(button, &QPushButton::clicked, this, [this]() {
                numberInputLineEdit->clear();
                numberInputLineEdit->setFocus();
            });
        };
        keypadLayout->addWidget(button, btnData.row, btnData.col);
    }

    QHBoxLayout* keypadCenteringLayout = new QHBoxLayout();
    keypadCenteringLayout->addStretch();
    keypadCenteringLayout->addLayout(keypadLayout);
    keypadCenteringLayout->addStretch();


    resultTextLabel = new QLabel("Результат появится здесь.", Scene1Widget);
    resultTextLabel->setAlignment(Qt::AlignCenter);
    resultTextLabel->setWordWrap(true);
    resultTextLabel->setMinimumHeight(50);
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(inputPromptLabel, 0, Qt::AlignHCenter);
    contentLayout->addWidget(numberInputLineEdit, 0, Qt::AlignHCenter);
    contentLayout->addLayout(keypadCenteringLayout, 0);
    contentLayout->addSpacing(20);
    contentLayout->addWidget(resultTextLabel);

    sceneLayout->addLayout(topBarLayout);
    sceneLayout->addLayout(contentLayout);

    sceneLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setContentsMargins(10, 0, 10, 10);

    Scene1Widget->setLayout(sceneLayout);

    connect(numberInputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::stage1Clicked);

}
void MainWindow::setupScene2Widget()
{
    Scene2Widget = new QWidget(stackedWidget);
    QVBoxLayout *mainSceneLayout = new QVBoxLayout(Scene2Widget);

    QHBoxLayout *topBarLayout = new QHBoxLayout();
    QPushButton *crossButton = new QPushButton(QString::fromUtf8("✕"), Scene2Widget);
    crossButton->setFixedSize(30, 30);
    crossButton->setToolTip("Вернуться в меню");
    crossButton->setStyleSheet(
        "QPushButton {"
        "  color: red;"
        "  background-color: transparent;"
        "  border: none;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { color: darkred; }"
        "QPushButton:pressed { color: firebrick; }"
    );
    connect(crossButton, &QPushButton::clicked, this, &MainWindow::showMenu);
    topBarLayout->addStretch();
    topBarLayout->addWidget(crossButton);

    resultLabelForScene2 = new QLabel("КТО", Scene2Widget);
    resultLabelForScene2->setAlignment(Qt::AlignCenter);
    resultLabelForScene2->setWordWrap(true);
    QFont titleFont = resultLabelForScene2->font();
    titleFont.setPointSize(12);
    resultLabelForScene2->setFont(titleFont);
    resultLabelForScene2->setMinimumHeight(40);

    QScrollArea *scrollArea = new QScrollArea(Scene2Widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *scrollAreaContentWidget = new QWidget();

    pairsLayout = new QVBoxLayout(scrollAreaContentWidget);
    pairsLayout->setAlignment(Qt::AlignTop);

    scrollAreaContentWidget->setLayout(pairsLayout);
    scrollArea->setWidget(scrollAreaContentWidget);

    QPushButton *addPairButton = new QPushButton(QChar(0x2795) + QString(" Добавить пару"), Scene2Widget);
    addPairButton->setMinimumHeight(30);
    QFont addPairButtonFont = addPairButton->font();
    addPairButtonFont.setPointSize(10);
    addPairButton->setFont(addPairButtonFont);
    connect(addPairButton, &QPushButton::clicked, this, &MainWindow::addInputPairRow);

    mainSceneLayout->addLayout(topBarLayout);
    mainSceneLayout->addWidget(resultLabelForScene2);
    mainSceneLayout->addWidget(scrollArea, 1);
    mainSceneLayout->addWidget(addPairButton);
    mainSceneLayout->setContentsMargins(10, 10, 10, 10);

    Scene2Widget->setLayout(mainSceneLayout);
    Scene2Widget->setStyleSheet("background-color: lightblue;");

    addInputPairRow();

    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), Scene2Widget);

    connect(enterShortcut, &QShortcut::activated, this, &MainWindow::crt);
}

void MainWindow::addInputPairRow()
{
    QWidget *rowContainerWidget = new QWidget();
    QHBoxLayout *rowLayout = new QHBoxLayout(rowContainerWidget);
    rowLayout->setContentsMargins(0,0,0,0);

    QLineEdit *lineEditA = new QLineEdit(rowContainerWidget);
    lineEditA->setPlaceholderText(QString("42"));
    lineEditA->setMinimumWidth(100);

    QLineEdit *lineEditB = new QLineEdit(rowContainerWidget);
    lineEditB->setPlaceholderText(QString("52"));
    lineEditB->setMinimumWidth(100);

    QPushButton *removeButton = new QPushButton(QString::fromUtf8("❌"), rowContainerWidget);
    removeButton->setFixedSize(25, 25);
    removeButton->setToolTip("Удалить эту пару");
    removeButton->setStyleSheet("QPushButton { color: red; background-color: transparent; border: none; font-size: 14px; }"
                                "QPushButton:hover { color: darkred; }");


    connect(removeButton, &QPushButton::clicked, this, [this, widgetToRemove = rowContainerWidget]() {
        removeInputPairRow(widgetToRemove);
    });

    rowLayout->addWidget(lineEditA);
    rowLayout->addWidget(lineEditB);
    rowLayout->addWidget(removeButton);

    rowContainerWidget->setLayout(rowLayout);

    pairsLayout->addWidget(rowContainerWidget);

    InputPairRow newRowData;
    newRowData.rowContainerWidget = rowContainerWidget;
    newRowData.lineEditA = lineEditA;
    newRowData.lineEditB = lineEditB;
    newRowData.removeButton = removeButton;
    inputPairRowsList.append(newRowData);
}

void MainWindow::removeInputPairRow(QWidget* rowWidgetToRemove)
{

    int indexToRemove = -1;
    for (int i = 0; i < inputPairRowsList.count(); ++i) {
        if (inputPairRowsList.at(i).rowContainerWidget == rowWidgetToRemove) {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove != -1) {
        inputPairRowsList.removeAt(indexToRemove);
    }

    pairsLayout->removeWidget(rowWidgetToRemove);


    rowWidgetToRemove->deleteLater();
}

long long binpow (long long a, long long pw, long long md) {
    if (pw == 0) {
        return 1;
    }
    long long u = binpow(a, pw / 2, md);
    u = (u * u) % md;
    if (pw % 2) {
        u = (u * a) % md;
    }
    return u;
}

void MainWindow::crt()
{
    std::vector<int> r;//emainder
    std::vector<int> p;//rime

    if (inputPairRowsList.isEmpty()) {
        resultLabelForScene2->setText("хоть что-нибудь введите");
        return;
    }

    for (const auto& rowData : inputPairRowsList) {
        if (rowData.lineEditA && rowData.lineEditB) {
            QString textA = rowData.lineEditA->text();
            QString textB = rowData.lineEditB->text();

            if (textA.isEmpty() || textB.isEmpty()) {
                continue;
            }

            // Попытка конвертировать A
            bool ok = false;
            bool ok2 = false;
            long long rr = textA.toLongLong(&ok);
            long long pp = textB.toLongLong(&ok2);
            if (!ok || !ok2) {
                resultLabelForScene2->setText("зря");
                return;
            }
            r.push_back(rr);
            p.push_back(pp);
        }
    }
    long double pro = 1.0;
    for (auto pr : p) {
        if (pr>1000000100) {
            resultLabelForScene2->setText("слишком много");
            return;
        }
        pro*=sqrt(pr);
        if (pro>1000000100){
            resultLabelForScene2->setText("слишком много");
            return;
        }
    }
    int n = p.size();
    for (int i=0;i<n;i++) {
        r[i]=r[i]%p[i];
    }
    for (int i=0;i<n;i++) {
        int m=p[i];
        for (int j=2;j*j<m;j++) {
            if (m%j == 0) {
                resultLabelForScene2->setText("простое должно быть простое");
                return;
            }
        }
    }

    for (int i=0;i<n;i++) {
        for (int j=0;j<i;j++){
            if (p[i]==p[j]){
                resultLabelForScene2->setText("простые должны быть разными");
                return;
            }
        }
    }
    long long prd = 1;
    for (int i=0;i<n;i++) {
        prd *= p[i];
    }
    long long ans=0;
    for (int i=0;i<n;i++) {
        long long m = prd/p[i];
        long long m1 = binpow(m,p[i]-2,p[i]);
        ans+=r[i]*m*m1;
    }
    resultLabelForScene2->setText(QString::number(ans));
}

void MainWindow::onButton1Clicked()
{
    stackedWidget->setCurrentWidget(Scene1Widget);
    resultTextLabel->setText("введите текст");
}

QString fact(long long n) {
    if (n==1) {
        return "не очевидно, да?";
    }
    std::vector<std::pair<int,int>> ans;
    int cnt=0;
    for (int i=2;i*i<=n;i++) {
        cnt = 0;
        while (n % i == 0) { n/=i; cnt++;}
        if (cnt!=0) ans.push_back({i,cnt});
    }
    if (n!=1) {
        ans.push_back({n,1});
    }
    QString str="";
    for (int i=0;i<ans.size();i++){
        if (i!=0) str+=" * ";
        str+=QString::number(ans[i].first);
        if (ans[i].second>1) {
            str+=+ "^" + QString::number(ans[i].second);
        }
    }
    return str;
}

void MainWindow::stage1Clicked()
{
    QString inputText = numberInputLineEdit->text();
    bool conversionOk = false;
    long long number = inputText.toLongLong(&conversionOk);

    if (conversionOk && number >= 1) {
        QString response = fact(number);
        resultTextLabel->setText(response);
    } else {
        resultTextLabel->setText("Кринжанул");
    }
}

void MainWindow::onButton2Clicked()
{
    stackedWidget->setCurrentWidget(Scene2Widget);
}

void MainWindow::onButton3Clicked()
{
    if(scene3ResultLabel) scene3ResultLabel->setText("lorem ipsum dolor sit amet");
    stackedWidget->setCurrentWidget(Scene3Widget);
}

void MainWindow::showMenu()
{
    stackedWidget->setCurrentWidget(menuWidget);
}

void MainWindow::setupScene3Widget()
{
    Scene3Widget = new QWidget(stackedWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(Scene3Widget);

    QHBoxLayout *topBarLayout = new QHBoxLayout();
    QPushButton *crossButton = new QPushButton(QString::fromUtf8("✕"), Scene3Widget);
    crossButton->setFixedSize(30, 30);
    crossButton->setToolTip("Вернуться в меню");
    crossButton->setStyleSheet(
        "QPushButton { color: red; background-color: transparent; border: none; font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { color: darkred; }"
        "QPushButton:pressed { color: firebrick; }"
    );
    connect(crossButton, &QPushButton::clicked, this, &MainWindow::showMenu);
    topBarLayout->addStretch();
    topBarLayout->addWidget(crossButton);

    QHBoxLayout *spinBoxSentenceLayout = new QHBoxLayout();

    QLabel *prefixLabel = new QLabel("Представьте очень жизненную ситуацию. Предположим в некоторую дату вам было ", Scene3Widget);
    scene3SpinBox = new QSpinBox(Scene3Widget);
    scene3SpinBox->setRange(-52, 4613);
    scene3SpinBox->setMinimumWidth(60);
    QLabel *suffixLabel = new QLabel("лет.", Scene3Widget);
    QLabel *nextLabel = new QLabel(" Затем каждый день до сегодняшнего (включительно) у вас происходил день рождения. Разумеется, вам хочется узнать сколько вам лет. Итак, введите дату:", Scene3Widget);
    nextLabel->setWordWrap(true);
    nextLabel->setAlignment(Qt::AlignHCenter);
    scene3SpinBox->setValue(17);
    spinBoxSentenceLayout->addStretch(1);
    spinBoxSentenceLayout->addWidget(prefixLabel);
    spinBoxSentenceLayout->addWidget(scene3SpinBox);
    spinBoxSentenceLayout->addWidget(suffixLabel);
    spinBoxSentenceLayout->addStretch(1);


    scene3Calendar = new QCalendarWidget(Scene3Widget);
    scene3Calendar->setFixedWidth(scene3Calendar->sizeHint().width());
    scene3Calendar->setMinimumHeight(200);
    QDate defaultDate(2025, 2, 17);
    scene3Calendar->setSelectedDate(defaultDate);


    scene3ResultLabel = new QLabel("", Scene3Widget);
    scene3ResultLabel->setAlignment(Qt::AlignCenter);
    scene3ResultLabel->setWordWrap(true);
    scene3ResultLabel->setMinimumHeight(60);

    QShortcut *enterShortcutScene3 = new QShortcut(QKeySequence(Qt::Key_Return), Scene3Widget);
    connect(enterShortcutScene3, &QShortcut::activated, this, &MainWindow::stage3Clicked);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(15);
    contentLayout->addLayout(spinBoxSentenceLayout);
    contentLayout->addWidget(nextLabel);
    contentLayout->addWidget(scene3Calendar, 0, Qt::AlignHCenter);
    contentLayout->setSpacing(30);

    contentLayout->addStretch();
    contentLayout->addWidget(scene3ResultLabel);
    contentLayout->addStretch();

    mainLayout->addLayout(topBarLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch(1);

    mainLayout->setContentsMargins(15, 10, 15, 15);
}

void MainWindow::stage3Clicked() {
    int number = scene3SpinBox->value();
    QDate selectedDate = scene3Calendar->selectedDate();
    QDate today = QDate::currentDate();

    if (selectedDate > today) {
        scene3ResultLabel->setText("Фатальная ошибка, я специально же в прошедшем времени запрос формулировал.");
        return;
    }
    int ans = number-today.daysTo(selectedDate)+1;

    QString res = QString("Поздравляю, тебе %1 лет, с др!!!").arg(ans);

    scene3ResultLabel->setText(res);
}

void MainWindow::setupMenuBar()
{
    fileMenu = menuBar()->addMenu("Меню");
    helpAction = new QAction(tr("Хелп"), this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, &QAction::triggered, this, &MainWindow::showHelpDialog);
    fileMenu->addAction(helpAction);
}

void MainWindow::showHelpDialog()
{
    QString title = tr("Помогите пожалуйста");
    QString messageText = tr(
        "<html>"
        "<body>"

        "<p><big><b>Почему по первой части первой лабы в табличке галочка дизайн не стоит???</b></big></p>"

        "<p style=\"text-align: center;\">Красиво же было        </p>"

        "<big>В этой лабе все вычисления запускаются на enter</big></p>"
        "<p>Ещё хелп на f1 работает тоже да</p>"
        "<p>КТО голубое, а остальное нет, потому что с остальным некрасиво смотрится</p>"


        "</body>"
        "</html>"
    );

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(messageText);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}