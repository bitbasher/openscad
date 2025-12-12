#include "gui/parameter/ParameterHexSpinBox.h"
#include "ui_ParameterHexSpinBox.h"
#include <QWidget>
#include <QRegularExpression>
#include <algorithm>
#include <limits>
#include <cmath>
#include "gui/IgnoreWheelWhenNotFocused.h"
#include "utils/printutils.h"

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
  : ParameterVirtualWidget(parent, parameter),
    ui(std::make_unique<Ui::ParameterHexSpinBox>()),
    parameter(parameter)
{
  ui->setupUi(this);
  ui->descriptionWidget->setDescription(parameter, descriptionStyle);

  auto *ignoreWheelWhenNotFocused = new IgnoreWheelWhenNotFocused(this);
  ui->slider->installEventFilter(ignoreWheelWhenNotFocused);
  ui->hexSpinBox->installEventFilter(ignoreWheelWhenNotFocused);
  ui->hexSpinBox->setKeyboardTracking(true);

  // Set up range for hex values (must be non-negative unsigned integers)
  this->minimum = parameter->minimum ? *parameter->minimum : 0.0;
  double maximum = parameter->maximum ? *parameter->maximum : 0xFFFFFFFF;
  this->step = parameter->step ? *parameter->step : 1.0;

  // Validate that hex values are non-negative and step is positive
  if (this->minimum < 0.0) {
    LOG(message_group::Warning, "Hex parameter '%1$s' has negative minimum %2$f, clamping to 0",
        parameter->name(), this->minimum);
    this->minimum = 0.0;
  }
  if (maximum < 0.0) {
    LOG(message_group::Warning, "Hex parameter '%1$s' has negative maximum %2$f, clamping to 0",
        parameter->name(), maximum);
    maximum = 0.0;
  }
  if (this->step < 1.0) {
    LOG(message_group::Warning, "Hex parameter '%1$s' has step %2$f < 1, clamping to 1",
        parameter->name(), this->step);
    this->step = 1.0;
  }

  // Calculate number of slider steps
  static constexpr auto maxSteps = static_cast<double>(std::numeric_limits<int>::max());
  double tempSteps = std::nextafter((maximum - this->minimum) / this->step, maxSteps) + 1.0;
  int numSteps = tempSteps >= maxSteps ? std::numeric_limits<int>::max() : static_cast<int>(tempSteps);
  double maximumValue = parameterValue(numSteps - 1);

  // Calculate hex width based on maximum value
  int64_t maxInt = static_cast<int64_t>(maximum);
  int hexWidth = 2;  // Minimum 2 digits
  if (maxInt > 0) {
    hexWidth = static_cast<int>(std::ceil(std::log2(maxInt + 1) / 4.0));
    hexWidth = std::max(2, hexWidth);  // At least 2 digits
  }
  ui->hexSpinBox->setHexWidth(hexWidth);

  // Configure slider
  ui->slider->setRange(0, numSteps - 1);
  ui->slider->setPageStep(std::ceil(0.1 * numSteps));

  // Configure spinbox
  ui->hexSpinBox->setRange(this->minimum, maximumValue);
  ui->hexSpinBox->setSingleStep(this->step);

  // Connect slider signals
  connect(ui->slider, &QSlider::sliderReleased, this, &ParameterHexSpinBox::onSliderReleased);
  connect(ui->slider, &QSlider::sliderMoved, this, &ParameterHexSpinBox::onSliderMoved);
  connect(ui->slider, &QSlider::valueChanged, this, &ParameterHexSpinBox::onSliderChanged);

  // Connect spinbox signals
  connect(ui->hexSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
          &ParameterHexSpinBox::onSpinBoxChanged);
  connect(ui->hexSpinBox, &QDoubleSpinBox::editingFinished, this,
          &ParameterHexSpinBox::onSpinBoxEditingFinished);

  ParameterHexSpinBox::setValue();
}

ParameterHexSpinBox::~ParameterHexSpinBox() = default;

void ParameterHexSpinBox::setValue()
{
  int position = sliderPosition(parameter->value);
  lastApplied = lastSent = parameter->value;
  ui->slider->setValue(position);
  ui->hexSpinBox->setValue(parameter->value);
}

void ParameterHexSpinBox::valueApplied()
{
  lastApplied = lastSent;
}

// Slider handle released
void ParameterHexSpinBox::onSliderReleased() { commitChange(true); }

// Slider handle dragged
void ParameterHexSpinBox::onSliderMoved(int position)
{
  double value = parameterValue(position);
  ui->hexSpinBox->blockSignals(true);
  ui->hexSpinBox->setValue(value);
  ui->hexSpinBox->blockSignals(false);
}

// Slider track clicked
void ParameterHexSpinBox::onSliderChanged(int position)
{
  double value = parameterValue(position);
  ui->hexSpinBox->blockSignals(true);
  ui->hexSpinBox->setValue(value);
  ui->hexSpinBox->blockSignals(false);
  commitChange(false);
}

// Spin button click or arrow keypress
void ParameterHexSpinBox::onSpinBoxChanged(double value)
{
  int position = sliderPosition(value);
  ui->slider->blockSignals(true);
  ui->slider->setValue(position);
  ui->slider->blockSignals(false);
  commitChange(false);
}

// Enter key pressed or spinbox focus lost
void ParameterHexSpinBox::onSpinBoxEditingFinished() { commitChange(true); }

void ParameterHexSpinBox::commitChange(bool immediate)
{
  double value = parameterValue(ui->slider->sliderPosition());
#ifdef DEBUG
  PRINTD(STR("[hex commit] value=", value, ", parameter->value=", parameter->value,
             ", lastSent=", lastSent, ", lastApplied=", lastApplied));
#endif
  if ((immediate && lastApplied != value) || (!immediate && lastSent != value)) {
    lastSent = parameter->value = value;
    emit changed(immediate);
  }
}

int ParameterHexSpinBox::sliderPosition(double value)
{
  return static_cast<int>(std::round((value - this->minimum) / this->step));
}

double ParameterHexSpinBox::parameterValue(int sliderPosition)
{
  return this->minimum + sliderPosition * this->step;
}
