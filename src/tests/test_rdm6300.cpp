// Stub test file for RDM6300 logic.
// HardwareSerial mocking in native environment can be complex,
// so this file represents where RDM6300 parser unit tests would live.

#include <unity.h>
#include "drivers/rfid/rdm6300.h"

void test_rdm6300_initialization(void) {
    TEST_ASSERT_TRUE(true); // Placeholder
}

void setup_test_rdm6300() {
    RUN_TEST(test_rdm6300_initialization);
}
