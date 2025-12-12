#include "gui/parameter/ParameterHexSpinBox.h"
#include <QWidget>
#include <QRegularExpression>
#include <algorithm>
#include <limits>
#include <cmath>
#include "gui/IgnoreWheelWhenNotFocused.h"

// HexDoubleSpinBox implementation
HexDoubleSpinBox::HexDoubleSpinBox(QWidget *parent)
  : QDoubleSpinBox(parent)
{
  setPrefix("0x");
  // Force integer steps for hex values
  setDecimals(0);
}

QString HexDoubleSpinBox::textFromValue(double val) const
{
  int64_t intVal = static_cast<int64_t>(val);
  // Format as hex with zero-padding to hexWidth
  return QString("%1").arg(intVal, hexWidth, 16, QChar('0')).toUpper();
}

double HexDoubleSpinBox::valueFromText(const QString& text) const
{
  QString hex = text;
  // Remove "0x" or "0X" prefix if present
  hex.remove(QRegularExpression("^0[xX]"));
  hex = hex.trimmed();

  bool ok;
  int64_t intVal = hex.toLongLong(&ok, 16);
  return ok ? static_cast<double>(intVal) : 0.0;
}

QValidator::State HexDoubleSpinBox::validate(QString& input, int& pos) const
{
  // Remove prefix for validation
  QString hex = input;
  hex.remove(QRegularExpression("^0[xX]"));
  hex = hex.trimmed();

  if (hex.isEmpty()) {
    return QValidator::Intermediate;
  }

  // Check if all characters are valid hex digits
  QRegularExpression hexPattern("^[0-9A-Fa-f]+$");
  if (!hexPattern.match(hex).hasMatch()) {
    return QValidator::Invalid;
  }

  // Parse and check range
  bool ok;
  int64_t intVal = hex.toLongLong(&ok, 16);
  if (!ok) {
    return QValidator::Invalid;
  }

  double val = static_cast<double>(intVal);
  if (val < minimum() || val > maximum()) {
    return QValidator::Intermediate;
  }

  return QValidator::Acceptable;
}

// ParameterHexSpinBox implementation
ParameterHexSpinBox::ParameterHexSpinBox(QWidget *parent, NumberParameter *parameter,
                                         DescriptionStyle descriptionStyle)
  : ParameterVirtualWidget(parent, parameter), parameter(parameter)
{
  setupUi(this);
  descriptionWidget->setDescription(parameter, descriptionStyle);

  auto *ignoreWheelWhenNotFocused = new IgnoreWheelWhenNotFocused(this);
  hexSpinBox->installEventFilter(ignoreWheelWhenNotFocused);
  hexSpinBox->setKeyboardTracking(true);

  // Set up range for hex values (typically integers)
  double minimum;
  if (parameter->minimum) {
    minimum = *parameter->minimum;
  } else {
    minimum = 0;  // Default to 0 for hex
  }

  double maximum;
  if (parameter->maximum) {
    maximum = *parameter->maximum;
  } else {
    maximum = 0xFFFFFFFF;  // Default to 32-bit max
  }

  double step;
  if (parameter->step) {
    step = *parameter->step;
  } else {
    step = 1;  // Default step of 1 for hex
  }

  // Calculate hex width based on maximum value
  int64_t maxInt = static_cast<int64_t>(maximum);
  int hexWidth = 2;  // Minimum 2 digits
  if (maxInt > 0) {
    hexWidth = static_cast<int>(std::ceil(std::log2(maxInt + 1) / 4.0));
    hexWidth = std::max(2, hexWidth);  // At least 2 digits
  }
  hexSpinBox->setHexWidth(hexWidth);

  hexSpinBox->setRange(minimum, maximum);
  hexSpinBox->setSingleStep(step);

  connect(hexSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &ParameterHexSpinBox::onChanged);
  connect(hexSpinBox, &QDoubleSpinBox::editingFinished, this, &ParameterHexSpinBox::onEditingFinished);

  ParameterHexSpinBox::setValue();
}

void ParameterHexSpinBox::setValue()
{
  hexSpinBox->setValue(parameter->value);
}

void ParameterHexSpinBox::valueApplied()
{
  lastApplied = lastSent;
}

void ParameterHexSpinBox::onChanged(double value)
{
#ifdef DEBUG
  PRINTD(STR("[hex changed] value=", value, ", parameter->value=", parameter->value, ", lastSent=", lastSent,
             ", lastApplied=", lastApplied));
#endif
  parameter->value = value;
  if (lastSent != value) {
    lastSent = value;
    emit changed(false);  // Delayed change
  }
}

void ParameterHexSpinBox::onEditingFinished()
{
#ifdef DEBUG
  PRINTD(STR("[hex finished] parameter->value=", parameter->value, ", lastSent=", lastSent,
             ", lastApplied=", lastApplied));
#endif
  if (lastApplied != parameter->value) {
    lastSent = parameter->value;
    emit changed(true);  // Immediate change on editing finished
  }
}
