#pragma once

#include <vector>
#include <Print.h>


class PrinterCollection : public Print {
protected:
    std::vector<Print*> printers;

public:
    PrinterCollection() {}

    void addPrinter(Print* printer) {
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
