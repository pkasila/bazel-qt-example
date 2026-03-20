#ifndef GREENHOUSE_CONTROL_APP_H_
#define GREENHOUSE_CONTROL_APP_H_

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDial>
#include <QEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>

class GreenhouseControlApp : public QWidget {
 public:
  explicit GreenhouseControlApp(QWidget *parent = nullptr);

 protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

 private:
  void buildInterface();
  void connectSignals();
  void growPlant();
  void waterPlant();
  void resetGreenhouse();
  void updateInterface();

  QBoxLayout *root_layout_;
  QWidget *control_panel_;
  QWidget *scene_panel_;
  QVBoxLayout *control_layout_;
  QVBoxLayout *scene_layout_;

  QLabel *title_label_;
  QLabel *name_label_;
  QLineEdit *name_edit_;
  QLabel *biome_label_;
  QComboBox *biome_box_;
  QLabel *light_label_;
  QDial *light_dial_;
  QCheckBox *night_mode_box_;
  QPushButton *grow_button_;
  QLabel *plant_label_;
  QLabel *status_label_;
  QLabel *hint_label_;

  QString plant_name_;
  QString biome_;
  int growth_stage_;
  int light_level_;
  int hydration_;
  int pose_index_;
  bool night_mode_;
};

#endif
