#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QSpinBox;
class QGroupBox;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    enum Rarity {
        Common = 0,
        Rare = 1,
        Epic = 2,
        Mythic = 3,
        Legendary = 4,
        Unknown = 5,
    };

    struct PersonCard {
        QString name;
        int age = 18;
        QString profession;
        QString status;
        QString description;
        QString image_path;
        Rarity rarity = Common;
    };

    QVector<PersonCard> cards_;
    int current_index_ = -1;

    QListWidget* cards_list_ = nullptr;

    QGroupBox* editor_box_ = nullptr;
    QWidget* editor_background_ = nullptr;
    QLabel* photo_label_ = nullptr;
    QLabel* name_preview_label_ = nullptr;

    QLineEdit* name_edit_ = nullptr;
    QSpinBox* age_spinbox_ = nullptr;
    QLineEdit* profession_edit_ = nullptr;
    QLineEdit* status_edit_ = nullptr;
    QComboBox* rarity_combo_ = nullptr;
    QTextEdit* description_edit_ = nullptr;

    QPushButton* new_card_button_ = nullptr;
    QPushButton* delete_card_button_ = nullptr;
    QPushButton* load_photo_button_ = nullptr;
    QPushButton* save_button_ = nullptr;
    QPushButton* reload_button_ = nullptr;

    void SetupUi();
    void UpdateCardsList();
    void UpdateSingleCardInList(int index);
    void UpdateEditor();
    void SelectCard(int index);

    QString ProjectRootPath() const;
    QString StoragePath() const;
    bool SaveCardsToFile();
    bool LoadCardsFromFile();
    void EnsureStorageFileExists();

    void LoadPhotoIntoLabel(const QString& path, Rarity rarity);
    QWidget* CreateListItemWidget(int index) const;

    QColor RarityColor(Rarity rarity) const;
    QColor RaritySecondaryColor(Rarity rarity) const;
    QColor RaritySoftBackground(Rarity rarity) const;
    QString RarityName(Rarity rarity) const;

    QString ResolveImagePath(const QString& path) const;

private slots:
    void OnNewCardClicked();
    void OnDeleteCardClicked();
    void OnCardClicked(QListWidgetItem* item);
    void OnNameChanged(const QString& text);
    void OnAgeChanged(int value);
    void OnProfessionChanged(const QString& text);
    void OnStatusChanged(const QString& text);
    void OnRarityChanged(int index);
    void OnDescriptionChanged();
    void OnLoadPhotoClicked();
    void OnSaveClicked();
    void OnReloadClicked();
};

#endif