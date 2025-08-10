#pragma once

#include <vector>
#include "printer.h"


class PrinterCollection : public Printer {
protected:
    std::vector<Printer*> printers;

public:
    PrinterCollection() {}

    void addPrinter(Printer* printer) {
        this->printers.emplace_back(printer);
    }

    size_t write(uint8_t ch) override {
        for (auto printer : this->printers) {
            printer->write(ch);
        }
        return 1;
    }

    size_t write(const uint8_t* buffer, const size_t size) override {
        for (auto printer : this->printers) {
            printer->write(buffer, size);
        }
        return size;
    }

    void flush() override {
        for (auto printer : this->printers) {
            printer->flush();
        }
    }
};
