#pragma once

#include "gui/parameter/ParameterVirtualWidget.h"
#include <QWidget>
#include <QDoubleSpinBox>
#include <memory>

// Custom QDoubleSpinBox that displays values in hexadecimal format
class HexDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

public:
  explicit HexDoubleSpinBox(QWidget *parent = nullptr);

  void setHexWidth(int width) { hexWidth = width; }
  int getHexWidth() const { return hexWidth; }

protected:
  QString textFromValue(double val) const override;
  double valueFromText(const QString& text) const override;
  QValidator::State validate(QString& input, int& pos) const override;

private:
  int hexWidth = 2;  // Minimum number of hex digits (for zero-padding)
};

namespace Ui {
class ParameterHexSpinBox;
}

class ParameterHexSpinBox : public ParameterVirtualWidget
{
  Q_OBJECT

public:
  ParameterHexSpinBox(QWidget *parent, NumberParameter *parameter, DescriptionStyle descriptionStyle);
  ~ParameterHexSpinBox();

  void setValue() override;
  void valueApplied() override;

protected slots:
  void onSliderReleased();
  void onSliderMoved(int position);
  void onSliderChanged(int position);
  void onSpinBoxChanged(double value);
  void onSpinBoxEditingFinished();

private:
  void commitChange(bool immediate);
  int sliderPosition(double value);
  double parameterValue(int sliderPosition);

  std::unique_ptr<Ui::ParameterHexSpinBox> ui;
  NumberParameter *parameter;
  double minimum;
  double step;
  boost::optional<double> lastSent;
  boost::optional<double> lastApplied;
};
