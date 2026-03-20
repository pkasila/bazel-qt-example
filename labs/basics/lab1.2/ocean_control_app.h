#ifndef OCEAN_CONTROL_APP_H_
#define OCEAN_CONTROL_APP_H_

#include <QString>
#include <QWidget>

class QCheckBox;
class QLineEdit;
class QLabel;
class QProgressBar;
class QPushButton;
class QResizeEvent;
class QSlider;
class QTimer;

class OceanControlApp : public QWidget {
  Q_OBJECT

 public:
  explicit OceanControlApp(QWidget *parent = nullptr);

 signals:
  void oxygenLevelChanged(int value);
  void statusTextChanged(const QString &text);

 protected:
  void resizeEvent(QResizeEvent *event) override;

 private slots:
  void accelerateDive();
  void restoreOxygen();
  void tickMission();
  void toggleMission();
  void updateDepth(int depth);
  void updateStealthMode(bool enabled);
  void updateVesselName(const QString &name);

 private:
  void applyResponsiveMetrics();
  void applyTheme();
  void buildUi();
  void connectUi();
  void setOxygenLevel(int value);
  void setStatusText(const QString &text);
  void stopMission(const QString &message);
  auto vesselName() const -> QString;

  QLabel *status_label_;
  QLineEdit *vessel_name_edit_;
  QPushButton *mission_button_;
  QCheckBox *stealth_checkbox_;
  QSlider *depth_slider_;
  QProgressBar *oxygen_bar_;
  QTimer *mission_timer_;
  int oxygen_level_;
  bool mission_active_;
};

#endif
