#pragma once
#include <Arduino.h>

struct Unpacked {
    String name;    
    int value;
};

class RadioDataPacker {
private:
    String buffer;

public:
    RadioDataPacker() {
        buffer.reserve(64);
    }

    // Tambah data: "name + value + #"
    void add(const char* name, int value) {
        buffer += name;
        buffer += value;
        buffer += '#';
    }

    String get() {
        return buffer;
    }

    void clear() {
        buffer = "";
    }

    Unpacked unpack(const String& input, byte index) {
        Unpacked result;
        result.name = "";
        result.value = 0;

        int currentIndex = 0;
        int start = 0;

        for (unsigned int i = 0; i < input.length(); i++) {
            if (input[i] == '#') {
                if (currentIndex == index) {
                    String segment = input.substring(start, i);
                    unsigned int pos = 0;
                    while (pos < segment.length() && !isDigit(segment[pos])) {
                        pos++;
                    }

                    result.name = segment.substring(0, pos);
                    result.value = segment.substring(pos).toInt();
                    return result;
                }
                currentIndex++;
                start = i + 1;
            }
        }

        return result; // default
    }
};
