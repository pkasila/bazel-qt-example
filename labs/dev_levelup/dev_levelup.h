#ifndef DEV_LEVELUP_H
#define DEV_LEVELUP_H

#include <QKeyEvent>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QListWidget;
class QLabel;
class QListWidgetItem;
class QLCDNumber;
QT_END_NAMESPACE

class DevLevelUp : public QWidget {
    Q_OBJECT

   public:
    DevLevelUp(QWidget* parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private slots:
    void updateHeroTitle(const QString& name);
    void onHoursChanged(int hours);
    void onHardcoreToggled(bool checked);
    void onGrindClicked();
    void onSkillDoubleClicked(QListWidgetItem* item);

   private:
    void setupUI();
    void addExperience(int amount);
    void updateRank();

    QLineEdit* m_nameEdit;
    QSpinBox* m_hoursSpin;
    QCheckBox* m_hardcoreCheck;
    QPushButton* m_grindBtn;
    QLCDNumber* m_levelDisplay;
    QProgressBar* m_xpBar;
    QListWidget* m_skillList;

    QLabel* m_rankLabel;
    QLabel* m_heroLabel;

    int m_currentLevel = 1;
    int m_currentXP = 0;
    int m_totalHours = 0;
};

#endif