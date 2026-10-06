#pragma once
#include <Preferences.h>
#include "config.h"

// =============================================================
//  storage.h  -  Simpan/muat sequence ke NVS (Preferences)
// =============================================================

// Frame & seq dideklarasikan di config.h (sudah di-include di atas)


Preferences prefs;

// Simpan satu sequence (idx 0=A, 1=B) ke NVS
void saveSeq(uint8_t idx) {
    prefs.begin("robot", false);
    char keyLen[8], keyData[16];
    snprintf(keyLen,  sizeof(keyLen),  "len%d",  idx);
    snprintf(keyData, sizeof(keyData), "seq%d",  idx);
    prefs.putUChar(keyLen, seqLen[idx]);
    if (seqLen[idx] > 0) {
        prefs.putBytes(keyData, &seq[idx][0], seqLen[idx] * sizeof(Frame));
    }
    prefs.end();
}

// Muat semua sequence dari NVS saat boot
void loadAll() {
    prefs.begin("robot", true);
    for (uint8_t i = 0; i < NUM_SEQ; i++) {
        char keyLen[8], keyData[16];
        snprintf(keyLen,  sizeof(keyLen),  "len%d", i);
        snprintf(keyData, sizeof(keyData), "seq%d", i);
        seqLen[i] = prefs.getUChar(keyLen, 0);
        if (seqLen[i] > 0) {
            prefs.getBytes(keyData, &seq[i][0], seqLen[i] * sizeof(Frame));
        }
    }
    prefs.end();
}
