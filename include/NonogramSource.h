#pragma once

#include "Nonogram.h"
#include "core/TextFormatParseUtil.h"
#include <string>

class NonogramSource {
  public:
    explicit NonogramSource(std::string path);

    // Reads from the file at path_ and Returns false with out_error on failure
    bool read(Nonogram &out_puzzle, std::string &out_error) const;

  private:
    std::string path_;
};
