#pragma once

#include "gui/parameter/ParameterVirtualWidget.h"
#include <QWidget>
#include <QDoubleSpinBox>
#include "ui_ParameterHexSpinBox.h"

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

class ParameterHexSpinBox : public ParameterVirtualWidget, Ui::ParameterHexSpinBox
{
  Q_OBJECT

public:
  ParameterHexSpinBox(QWidget *parent, NumberParameter *parameter, DescriptionStyle descriptionStyle);
  void setValue() override;
  void valueApplied() override;

protected slots:
  void onChanged(double);
  void onEditingFinished();

private:
  NumberParameter *parameter;
  boost::optional<double> lastSent;
  boost::optional<double> lastApplied;
};
