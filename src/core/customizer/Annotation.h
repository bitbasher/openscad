#pragma once

#include <ostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class Annotation
{
public:
  Annotation(std::string name, std::shared_ptr<class Expression> expr, std::string raw_text = "");

  void print(std::ostream& stream, const std::string& indent) const;
  [[nodiscard]] const std::string& getName() const { return name; }
  [[nodiscard]] const std::shared_ptr<Expression>& getExpr() const { return expr; }
  [[nodiscard]] const std::string& getRawText() const { return raw_text; }

private:
  std::string name;
  std::shared_ptr<Expression> expr;
  std::string raw_text;
};

using AnnotationList = std::vector<Annotation>;
using AnnotationMap = std::unordered_map<std::string, Annotation *>;
